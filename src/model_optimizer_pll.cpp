/*
 * model_optimizer_pll.cpp
 *
 *  Created on: Jun 10, 2015
 *      Author: diego
 */

#include "model_optimizer_pll.h"
#include "utils.h"

#include <cassert>
#include <iostream>

#define CHECK_LOCAL_CONVERGENCE 1

#define JOB_WAIT             0
#define JOB_UPDATE_MATRICES  1
#define JOB_FULL_LK          2
#define JOB_UPDATE_PARTIALS  3
#define JOB_REDUCE_LK_EDGE   4
#define JOB_FINALIZE        -1

extern "C" void* thread_worker( void * d );

struct thread_wrap {
    void * data;
    modeltest::ModelOptimizerPll * instance;

    thread_wrap( void * d, modeltest::ModelOptimizerPll * i ) : data(d), instance(i) {}
};

extern "C" void* thread_worker( void * d )
{
    thread_wrap * w = static_cast< thread_wrap* >( d );
    void * return_val = w->instance->worker(w->data);

    return return_val;
}

/* a callback function for performing a full traversal */
static int cb_full_traversal(pll_utree_t * node)
{
    UNUSED(node);
    return 1;
}

/* a callback function for resetting missing branches */
static int cb_set_missing_branches(pll_utree_t * node)
{

    /* reset branches */
    if (!(node->length > 0.0))
    {
        node->length = 0.1;
        node->back->length = 0.1;
    }

    return 1;
}

static int barrier(thread_data_t * data)
{
  return pthread_barrier_wait (data->barrier_buf);
}

static void dealloc_partition_local(pll_partition_t * partition)
{
  if (partition->clv)
    free(partition->clv);
  if (partition->scale_buffer)
    free(partition->scale_buffer);
  free(partition);
}

static pll_partition_t * pll_partition_clone_partial(
                             pll_partition_t * partition,
                             unsigned int id,
                             unsigned int count)
{
  unsigned int i;
  unsigned int start, sites, offset;
  pll_partition_t * new_partition;

  sites = (partition->sites / count);
  offset = (partition->sites % count);
  start = sites * id + ((id<offset)?id:offset);
  sites += (id < offset)?1:0;

  new_partition = (pll_partition_t *) malloc(sizeof(pll_partition_t));
  new_partition->tips = partition->tips;
  new_partition->clv_buffers = partition->clv_buffers;
  new_partition->states = partition->states;
  new_partition->sites = sites;
  new_partition->rate_matrices = partition->rate_matrices;
  new_partition->prob_matrices = partition->prob_matrices;
  new_partition->rate_cats = partition->rate_cats;
  new_partition->scale_buffers = partition->scale_buffers;
  new_partition->attributes = partition->attributes;

    /* vectorization options */
  new_partition->alignment = partition->alignment;
  new_partition->states_padded = partition->states_padded;

  new_partition->mixture = partition->mixture;
  new_partition->clv = (double **)calloc(partition->tips + partition->clv_buffers,
                                            sizeof(double *));
  if (!new_partition->clv)
  {
    dealloc_partition_local (new_partition);
    return PLL_FAILURE;
  }
  for (i = 0; i < new_partition->tips + new_partition->clv_buffers; ++i)
    new_partition->clv[i] = partition->clv[i]
        + (start * partition->states_padded * partition->rate_cats);

  new_partition->scale_buffer = (unsigned int **) calloc (
      new_partition->scale_buffers, sizeof(unsigned int *));
  if (!new_partition->scale_buffer)
  {
    dealloc_partition_local (new_partition);
    return PLL_FAILURE;
  }
  for (i = 0; i < new_partition->scale_buffers; ++i)
    new_partition->scale_buffer[i] = &partition->scale_buffer[i][start];

  if (partition->invariant)
    new_partition->invariant = &partition->invariant[start];
  else
    new_partition->invariant = NULL;

  new_partition->pattern_weights = &partition->pattern_weights[start];

  new_partition->pmatrix = partition->pmatrix;
  new_partition->rates = partition->rates;
  new_partition->rate_weights = partition->rate_weights;
  new_partition->subst_params = partition->subst_params;
  new_partition->frequencies = partition->frequencies;
  new_partition->prop_invar = partition->prop_invar;

  new_partition->eigen_decomp_valid = partition->eigen_decomp_valid;
  new_partition->eigenvecs = partition->eigenvecs;
  new_partition->inv_eigenvecs = partition->inv_eigenvecs;
  new_partition->eigenvals = partition->eigenvals;

  return new_partition;
}

namespace modeltest
{

bool on_run = true;
static bool keep_branch_lengths = false;

ModelOptimizer::~ModelOptimizer() {}

ModelOptimizerPll::ModelOptimizerPll (MsaPll *_msa,
                                      TreePll *_tree,
                                      Model *_model,
                                      const partition_descriptor_t &_partition,
                                      mt_size_t _n_cat_g,
                                      mt_index_t _thread_number)
    : ModelOptimizer(_model, _partition, _thread_number),
      msa(_msa),
      tree(_tree)
{
    thread_job = 0;
    global_lnl = 0;

    params = NULL;
    operations = NULL;
    branch_lengths = NULL;
    matrix_indices = NULL;

    mt_size_t n_tips = tree->get_n_tips ();
    mt_size_t n_inner = tree->get_n_inner ();
    mt_size_t n_branches = tree->get_n_branches ();
    mt_size_t n_nodes = tree->get_n_nodes ();
    mt_size_t n_sites = 0;
    mt_size_t mixture = 0;
    unsigned int attributes = PLL_ATTRIB_ARCH_SSE;

    for (const partition_region_t & region : partition.regions)
    {
        n_sites += (region.end - region.start + 1)/region.stride;
    }

    /* create partition */
    if (model->is_mixture())
    {
        mixture = N_MIXTURE_CATS;
        attributes |= PLL_ATTRIB_MIXT_LINKED;
        assert(mixture == _n_cat_g);
    }

    pll_partition = model->build_partition(n_tips, n_sites, _n_cat_g);

    if (!pll_partition)
        std::cout << "  MIXTURE CATS " << pll_errno << " " << pll_errmsg << std::endl;
    assert(pll_partition);

    pll_utree_t* pll_tree = tree->get_pll_start_tree (_thread_number);

    pll_utree_t ** tipnodes = (pll_utree_t **) Utils::c_allocate (n_tips,
                                                       sizeof(pll_utree_t *));
    if (!pll_utree_query_tipnodes (pll_tree, tipnodes))
    {
        assert(0);
    }

    /* find sequences and link them with the corresponding taxa */
    for (mt_index_t i = 0; i < n_tips; ++i)
    {
        mt_index_t tip_clv_index = MT_SIZE_UNDEF;
        const char * header = msa->get_header (i);

        for (mt_index_t j = 0; j < n_tips; ++j)
        {
            if (!strcmp (tipnodes[j]->label, header))
            {
                tip_clv_index = j;
                break;
            }
        }

        if (tip_clv_index == MT_SIZE_UNDEF)
            std::cerr << "ERROR: Cannot find tip \"" << header << "\"" << std::endl;
        assert(tip_clv_index != MT_SIZE_UNDEF);

        //TODO: keep tip states between models
        char *seq;
        const char *c_seq;
        if (partition.regions.size() > 1 || partition.regions[0].stride > 1)
        {
            /* complex partition */
            seq = (char *) Utils::allocate(n_sites, sizeof(char));
            c_seq = seq;
            for (const partition_region_t & region : partition.regions)
            {
                mt_size_t region_sites = (region.end - region.start + 1)/region.stride;
                if (region.stride == 1)
                {
                    /* copy consecutive sites in sequence */
                    memcpy(seq, msa->get_sequence (i) + region.start - 1, region_sites);
                }
                else
                {
                    /* copy strided sites in sequence */
                    mt_index_t k = 0;
                    const char * f_seq = msa->get_sequence (i);
                    for (mt_index_t s = region.start; s <= region.end && s < n_sites; s+=region.stride)
                    {
                        seq[k++] = f_seq[s];
                    }
                }
                seq += region_sites;
            }
        }
        else
        {
            /* simple partition */
            c_seq = msa->get_sequence (i);
        }
        if (!pll_set_tip_states (pll_partition,
                            (unsigned int)tip_clv_index,
                            (model->get_datatype() == dt_dna)?pll_map_nt:pll_map_aa,
                             c_seq))
        {
            /* data type and tip states should be validated beforehand */
            std::cerr << "ERROR: Sequence does not match the datatype" << std::endl;
            assert(0);
        }
    }

    free (tipnodes);

    /* allocate a buffer for storing pointers to nodes of the tree in postorder
     traversal */
    pll_utree_t ** travbuffer = (pll_utree_t **) Utils::allocate(
                n_nodes, sizeof(pll_utree_t *));

    /* additional stuff */
    mt_size_t traversal_size;
    if (!pll_utree_traverse (pll_tree,
                             cb_set_missing_branches,
                             travbuffer,
                             &traversal_size))
    {
      std::cerr << 
        "ERROR: PLL returned an error computing tree traversal" <<
        std::endl;
      assert(0);
    }

    assert(traversal_size > 0);

    branch_lengths = new double[n_branches];
    matrix_indices = new mt_index_t[n_branches];
    operations = new pll_operation_t[n_inner];

    pll_utree_create_operations (travbuffer, traversal_size, branch_lengths,
                                 matrix_indices, operations, &matrix_count,
                                 &ops_count);

    free (travbuffer);
}

  ModelOptimizerPll::~ModelOptimizerPll ()
  {
      delete[] params->subst_params_symmetries;
      delete[] branch_lengths;
      delete[] matrix_indices;
      delete[] operations;
      delete params;

      pll_partition_destroy(pll_partition);
  }

  static void fill_rates   (double *rates,
                            double *x,
                            int *bt, double *lb, double *ub,
                            mt_size_t n_rates)
  {
    for (mt_index_t i = 0; i < n_rates; i++)
    {
      bt[i] = PLL_LBFGSB_BOUND_BOTH;
      lb[i] = 1e-3;
      ub[i] = 100;
      assert(rates[i] > lb[i] && rates[i] < ub[i]);
      x[i] = rates[i];
//      double r = rates[i];
//      x[i] = (r > lb[i] && r < ub[i]) ? r : 1.0;
    }
  }

  static void fill_weights (double *weights,
                            unsigned int * highest_weight_index,
                            double *x,
                            int *bt, double *lb, double *ub,
                            mt_size_t n_weights)
  {
    mt_index_t cur_index = 0;

    for (mt_index_t i = 1; i < (n_weights-1); i++)
    {
        bt[i] = PLL_LBFGSB_BOUND_BOTH;
        x[i] = 1.0;
        lb[i] = PLL_OPT_MIN_FREQ;
        ub[i] = PLL_OPT_MAX_FREQ;
    }
    return;

    *highest_weight_index = 0;
    for (mt_index_t i = 1; i < n_weights; i++)
      if (weights[i] > weights[*highest_weight_index])
        *highest_weight_index = i;

    for (mt_index_t i = 0; i < n_weights; i++)
    {
      if (i != *highest_weight_index)
      {
        bt[cur_index] = PLL_LBFGSB_BOUND_BOTH;
        double r = weights[i] / weights[*highest_weight_index];
        lb[cur_index] = PLL_OPT_MIN_FREQ;
        ub[cur_index] = PLL_OPT_MAX_FREQ;
        x[cur_index] = (r > lb[i] && r < ub[i]) ? r : 1.0;
        cur_index++;
      }
    }
  }

  typedef struct
  {
    pll_partition_t * partition;
    unsigned int parent_clv_index;
    unsigned int parent_scaler_index;
    unsigned int child_clv_index;
    unsigned int child_scaler_index;
    unsigned int edge_pmatrix_index;
    unsigned int freqs_index;
    unsigned int params_index;
    unsigned int mixture_index;

    /* traverse */
    unsigned int * matrix_indices;
    double * branch_lengths;
    pll_operation_t * operations;

    /* rate specific */
    unsigned int highest_weight_state;
  } my_params_t;

  static void update_clvs(pll_partition_t * partition,
                          unsigned int params_index,
                          unsigned int * matrix_indices,
                          double * branch_lengths,
                          pll_operation_t * operations)
  {
    unsigned int n_branches = 2*partition->tips - 3;
    unsigned int n_inner    = partition->tips - 2;

    if (matrix_indices)
      pll_update_prob_matrices (partition, 0,
                                matrix_indices,
                                branch_lengths,
                                n_branches);
    if (operations)
      pll_update_partials (partition,
                           operations,
                           n_inner);
  }

  static double target_rates_opt (void * p, double * x)
  {
    my_params_t * params = (my_params_t *) p;
    pll_partition_t * pll_partition = params->partition;
    double score;

//    /* set x to partition */
//    int n_rates = pll_partition->rate_cats;

    for (mt_index_t i=0; i<(pll_partition->mixture); i++)
       pll_partition->rates[i] = x[i];

//    /* rescale rates */
//    double sumWR = 0.0;
//    for (mt_index_t i=0; i<pll_partition->mixture; i++)
//      sumWR += x[i] * pll_partition->rate_weights[i];
//    for (mt_index_t i=0; i<pll_partition->mixture; i++)
//      pll_partition->rates[i] = x[i] / sumWR;

    update_clvs (pll_partition, params->params_index, params->matrix_indices,
                 params->branch_lengths, params->operations);

    score = -1
        * pll_compute_edge_loglikelihood (pll_partition, params->parent_clv_index,
                                          params->parent_scaler_index,
                                          params->child_clv_index,
                                          params->child_scaler_index,
                                          params->edge_pmatrix_index,
                                          params->freqs_index);

    return score;
  }

  static double target_weights_opt (void * p, double * x)
  {
    my_params_t * params = (my_params_t *) p;
    pll_partition_t * partition = params->partition;
    double score;
    unsigned int i;

    unsigned int n_weights = partition->mixture;
    unsigned int cur_index;
    double sum_ratios = 1.0;
    double *weights = (double *) malloc ((size_t) partition->mixture * sizeof(double));
    unsigned int highest_weight_state = params->highest_weight_state;

    for (i = 0; i < (n_weights - 1); ++i)
    {
      sum_ratios += x[i];
    }
    cur_index = 0;
    for (i = 0; i < (n_weights - 1); ++i)
    {
      weights[i] = x[i] / sum_ratios;
    }
    weights[n_weights - 1] = 1.0 / sum_ratios;

//    for (i = 0; i < (n_weights); ++i)
//    {
//      if (i != highest_weight_state)
//      {
//        weights[i] = x[cur_index] / sum_ratios;
//        cur_index++;
//      }
//    }
//    weights[highest_weight_state] = 1.0 / sum_ratios;
    pll_set_category_weights(partition, weights);
    free (weights);

//    /* rescale rates */
//    double sumWR = 0.0;
//    for (mt_index_t i=0; i<partition->mixture; i++)
//      sumWR += partition->rates[i] * partition->rate_weights[i];
//    for (mt_index_t i=0; i<partition->mixture; i++)
//      partition->rates[i] /= sumWR;

//    update_clvs (partition, params->params_index, params->matrix_indices,
//                 params->branch_lengths, params->operations);

    score = -1
        * pll_compute_edge_loglikelihood (partition, params->parent_clv_index,
                                          params->parent_scaler_index,
                                          params->child_clv_index,
                                          params->child_scaler_index,
                                          params->edge_pmatrix_index,
                                          params->freqs_index);

    return score;
  }

  double ModelOptimizerPll::opt_single_parameter(mt_parameter_t which_parameter,
                                                 double tolerance,
                                                 bool first_guess)
  {
      double cur_logl = 0.0;

      assert(params);

      switch(which_parameter)
      {
      case mt_param_branch_lengths:
        cur_logl = opt_branch_lengths(tolerance);
        break;
      case mt_param_alpha:
          cur_logl = opt_alpha(tolerance, first_guess);
          break;
      case mt_param_pinv:
          cur_logl = opt_pinv(tolerance, first_guess);
          break;
      case mt_param_subst_rates:
          params->which_parameters = PLL_PARAMETER_SUBST_RATES;
          cur_logl = pll_optimize_parameters_lbfgsb(params);
          break;
      case mt_param_frequencies:
          params->which_parameters = PLL_PARAMETER_FREQUENCIES;
          cur_logl = pll_optimize_parameters_lbfgsb(params);
          break;
      case mt_param_mixture_rates_weights:
      {
          double x[4], lb[4],
                ub[4];
            int bt[4];

          my_params_t my_params;
          my_params.partition = pll_partition;
          my_params.parent_clv_index = params->lk_params.where.unrooted_t.parent_clv_index;
          my_params.parent_scaler_index = params->lk_params.where.unrooted_t.parent_scaler_index;
          my_params.child_clv_index = params->lk_params.where.unrooted_t.child_clv_index;
          my_params.child_scaler_index = params->lk_params.where.unrooted_t.child_scaler_index;
          my_params.edge_pmatrix_index = params->lk_params.where.unrooted_t.edge_pmatrix_index;
          my_params.freqs_index = 0;
          my_params.params_index = 0;
          my_params.mixture_index = 0;

          my_params.matrix_indices = params->lk_params.matrix_indices;
          my_params.branch_lengths = params->lk_params.branch_lengths;
          my_params.operations = params->lk_params.operations;

          /* 2 step BFGS */

          /* optimize mixture weights */

          double * weights = pll_partition->rate_weights;
          fill_weights(weights, &(my_params.highest_weight_state), x,
                            bt, lb, ub, pll_partition->mixture);

          cur_logl = -1
              * pll_minimize_lbfgsb (x, lb, ub, bt, pll_partition->mixture-1, params->factr,
                                     params->pgtol, &my_params, target_weights_opt);

          /* optimize mixture rates */

          fill_rates (pll_partition->rates, x, bt, lb, ub, pll_partition->rate_cats);

          cur_logl = pll_minimize_lbfgsb(x, lb, ub, bt, pll_partition->mixture,
                                         params->factr, params->pgtol,
                                         &my_params, target_rates_opt);

//          /* rescale rates */
//          double sumWR = 0.0;
//          for (mt_index_t i=0; i<pll_partition->mixture; i++)
//            sumWR += pll_partition->rates[i] * pll_partition->rate_weights[i];
//          for (mt_index_t i=0; i<pll_partition->mixture; i++)
//            pll_partition->rates[i] /= sumWR;

//          update_clvs (pll_partition, my_params.params_index, my_params.matrix_indices,
//                       my_params.branch_lengths, my_params.operations);
      }
          break;

      }

      return cur_logl;
  }

#define MIN_ALPHA 0.02
#define MAX_ALPHA 100.0

  double ModelOptimizerPll::opt_alpha(double tolerance,
                           bool first_guess)
  {
      double cur_logl;
      params->pgtol = tolerance;
      params->which_parameters = PLL_PARAMETER_ALPHA;
      cur_logl = pll_optimize_parameters_brent_ranged(params,
                                                      MIN_ALPHA,
                                                      params->lk_params.alpha_value,
                                                      MAX_ALPHA);
      return cur_logl;
  }

#undef MIN_ALPHA
#undef MAX_ALPHA

#define MIN_PINV 0.0

  double ModelOptimizerPll::opt_pinv(double tolerance,
                                     bool first_guess)
  {
      double cur_logl;
      double max_pinv = std::min(partition.empirical_pinv, 0.99);
      params->pgtol = tolerance;
      params->which_parameters = PLL_PARAMETER_PINV;
      cur_logl = pll_optimize_parameters_brent_ranged(params,
                                                      MIN_PINV,
                                                      params->lk_params.partition->prop_invar[0],
                                                      max_pinv);
      return cur_logl;
  }

#undef MIN_PINV

  double ModelOptimizerPll::opt_branch_lengths(double tolerance)
  {
      int smoothings = 2;
      mt_index_t tmp_matrix_count, tmp_ops_count;
      pll_utree_t* pll_tree;
      pll_utree_t ** travbuffer;
      mt_size_t traversal_size;
      double cur_logl;

      params->pgtol = tolerance;

      /* move to random node */
      tree->reroot_random(thread_number);
      pll_tree = tree->get_pll_tree(thread_number);


      travbuffer = (pll_utree_t **) Utils::allocate (2*tree->get_n_tips() - 2, sizeof(pll_utree_t *));
      pll_utree_traverse (pll_tree, cb_full_traversal, travbuffer, &traversal_size);
      pll_utree_create_operations (travbuffer, traversal_size, branch_lengths,
                                   matrix_indices, operations, &tmp_matrix_count,
                                   &tmp_ops_count);
      pll_update_prob_matrices (pll_partition, 0, matrix_indices, branch_lengths,
                                tree->get_n_branches());

//          for (mt_index_t i=0; i<num_threads; i++)
//            thread_data[i].vroot = tree->get_pll_tree();
//          start_job_sync (JOB_UPDATE_PARTIALS, thread_data);

      pll_update_partials (pll_partition, operations, tree->get_n_tips() - 2);

      params->which_parameters = PLL_PARAMETER_BRANCHES_ITERATIVE;

      cur_logl = pll_optimize_branch_lengths_iterative (
                  pll_partition, pll_tree, params->params_index, params->lk_params.freqs_index,
                  params->pgtol, smoothings, true);

      pll_utree_traverse (pll_tree, cb_full_traversal, travbuffer, &traversal_size);
      pll_utree_create_operations (travbuffer, traversal_size, branch_lengths,
                                   matrix_indices, operations, &tmp_matrix_count,
                                   &tmp_ops_count);

      params->lk_params.where.unrooted_t.parent_clv_index = pll_tree->clv_index;
      params->lk_params.where.unrooted_t.parent_scaler_index =
              pll_tree->scaler_index;
      params->lk_params.where.unrooted_t.child_clv_index = pll_tree->back->clv_index;
      params->lk_params.where.unrooted_t.child_scaler_index =
              pll_tree->back->scaler_index;
      params->lk_params.where.unrooted_t.edge_pmatrix_index =
              pll_tree->pmatrix_index;

      free(travbuffer);

      return cur_logl;
  }

  bool ModelOptimizerPll::build_parameters(pll_utree_t * pll_tree)
  {
      double default_alpha = 1.0;

      if (!params)
        params = new pll_optimize_options_t;

      /* pll stuff */
      params->lk_params.partition = pll_partition;
      params->lk_params.operations = operations;
      params->lk_params.branch_lengths = branch_lengths;
      params->lk_params.matrix_indices = matrix_indices;
      params->lk_params.alpha_value = 1.0;
      if (model->is_G())
      {
          if (model->is_I())
            params->lk_params.alpha_value = (alpha_inv_guess > 0.0)?alpha_inv_guess:default_alpha;
          else
            params->lk_params.alpha_value = (alpha_guess > 0.0)?alpha_guess:default_alpha;
      }
      params->lk_params.freqs_index = 0;
      params->lk_params.rooted = 0;
      params->lk_params.where.unrooted_t.parent_clv_index = pll_tree->clv_index;
      params->lk_params.where.unrooted_t.parent_scaler_index = pll_tree->scaler_index;
      params->lk_params.where.unrooted_t.child_clv_index = pll_tree->back->clv_index;
      params->lk_params.where.unrooted_t.child_scaler_index = pll_tree->back->scaler_index;
      params->lk_params.where.unrooted_t.edge_pmatrix_index = pll_tree->pmatrix_index;

      /* optimization parameters */
      params->mixture_index = 0;
      params->params_index = 0;
      if (partition.datatype == dt_dna)
      {
        int *symmetries = new int[N_DNA_SUBST_RATES];
        memcpy(symmetries, model->get_symmetries(), N_DNA_SUBST_RATES * sizeof(int));
        params->subst_params_symmetries = symmetries;
      }
      else
      {
        params->subst_params_symmetries = 0;
      }
      params->factr = 1e9;

      /* set initial model parameters */
      double * rate_cats = (double *) Utils::c_allocate( pll_partition->rate_cats, sizeof(double));

      if (!model->is_I())
          pll_update_invariant_sites_proportion(pll_partition, 0, 0.0);
      else
      {
          if (model->is_G())
              pll_update_invariant_sites_proportion(pll_partition, 0, (pinv_alpha_guess > 0.0)?pinv_alpha_guess:(partition.empirical_pinv/2));
          else
              pll_update_invariant_sites_proportion(pll_partition, 0, (pinv_guess > 0.0)?pinv_guess:(partition.empirical_pinv/2));
      }

      if (model->is_F())
          model->set_frequencies(partition.empirical_freqs);

      if (model->is_mixture())
      {
          assert (pll_partition->mixture = N_MIXTURE_CATS);
          for (mt_index_t i = 0; i < N_MIXTURE_CATS; i++)
          {
              pll_set_frequencies (pll_partition, 0, i, model->get_mixture_frequencies(i));
              pll_set_subst_params (pll_partition, 0, i, model->get_mixture_subst_rates(i));
          }
      }
      else
      {
        pll_set_frequencies (pll_partition, 0, 0, model->get_frequencies());
        pll_set_subst_params (pll_partition, 0, 0, model->get_subst_rates());
      }

      if (model->is_G() || model->is_mixture())
      {
          pll_compute_gamma_cats (params->lk_params.alpha_value,
                                  pll_partition->rate_cats,
                                  rate_cats);
      }
      else
      {
          rate_cats[0] = 1.0;
      }

      pll_set_category_rates (pll_partition, rate_cats);

      free(rate_cats);

      return true;
  }

  bool ModelOptimizerPll::run(double epsilon,
                              double tolerance,
                              mt_size_t _num_threads)
  {
      /*test*/
      //num_threads = 2;

#ifdef VERBOSE //TODO: Verbosity medium
      std::cout << "Optimizing model " << model->get_name() <<  " with " << _num_threads << " threads" << std::endl;
#endif
      time_t start_time = time(NULL);
      mt_size_t n_branches = tree->get_n_branches();

      pthread_t * threads = NULL;
      pll_partition_t ** partition_local = NULL;
      double * result_buf = NULL;
      pthread_barrier_t barrier_buf;

      std::vector<mt_parameter_t> params_to_optimize;

      if ((model->get_datatype() == dt_dna || !tree->is_bl_optimized()))
          params_to_optimize.push_back(mt_param_branch_lengths);
      if (model->get_datatype() == dt_dna && model->get_n_subst_params() > 0)
          params_to_optimize.push_back(mt_param_subst_rates);
      if (model->is_G() && model->is_I())
      {
          model->set_alpha(alpha_inv_guess);
          model->set_prop_inv(pinv_alpha_guess);
          params_to_optimize.push_back(mt_param_alpha);
          params_to_optimize.push_back(mt_param_pinv);
      }
      else
      {
          if (model->is_G())
          {
              model->set_alpha(alpha_guess);
              params_to_optimize.push_back(mt_param_alpha);
          }
          if (model->is_I())
          {
              pinv_guess = partition.empirical_pinv/2;
              model->set_prop_inv(pinv_guess);
              params_to_optimize.push_back(mt_param_pinv);
          }
      }
      if (model->get_datatype() == dt_dna && model->is_F())
          params_to_optimize.push_back(mt_param_frequencies);

      if (model->is_mixture() && !model->is_G())
      {
          params_to_optimize.push_back(mt_param_mixture_rates_weights);
      }

#ifdef VERBOSE //TODO: Verbosity high
      std::cout << "Initial Frequencies:   ";
      for (mt_index_t i=0; i<pll_partition->states; i++)
        std::cout << pll_partition->frequencies[0][i] << " ";
      std::cout << std::endl;
#endif

      /* /PTHREADS */
      if (_num_threads > 1)
      {
        threads = (pthread_t *) Utils::allocate(_num_threads, sizeof(pthread_t));
        thread_data = (thread_data_t *) Utils::allocate(_num_threads, sizeof(thread_data_t));
        partition_local = (pll_partition_t **) Utils::allocate(_num_threads, sizeof(pll_partition_t *));
        result_buf = (double *) Utils::allocate(_num_threads, sizeof(double));
        thread_job = JOB_WAIT;
        pthread_barrier_init (&barrier_buf,
                              NULL,
                              _num_threads);
        for (mt_index_t i=0; i<_num_threads; i++)
        {
          partition_local[i] = pll_partition_clone_partial(pll_partition, i, _num_threads);

          thread_data[i].thread_id = i;
          thread_data[i].num_threads = (long) _num_threads;
          thread_data[i].partition = partition_local[i];
          thread_data[i].matrix_indices = matrix_indices;
          thread_data[i].matrix_count = matrix_count;
          thread_data[i].operations = operations;
          thread_data[i].ops_count = ops_count;
          thread_data[i].branch_lengths = branch_lengths;
          thread_data[i].vroot = tree->get_pll_tree();
          thread_data[i].barrier_buf = &barrier_buf;
          thread_data[i].trap = 1;
          thread_data[i].result_buf = result_buf;

          /* thread 0 is reserved for master process */
          if (i)
          {
              thread_wrap * w = new thread_wrap(&(thread_data[i]), this);
              if(pthread_create(&(threads[i]), NULL, thread_worker, w)) {
                std::cerr << "ERROR: Cannot create thread " << i << std::endl;
                return false;
              }
          }
        }
      }
      /* /PTHREADS */

      //tree->reroot_random(thread_number);
      pll_utree_t * pll_tree = tree->get_pll_start_tree(thread_number);
      build_parameters(pll_tree);

      pll_update_prob_matrices (pll_partition,
                                0,
                                matrix_indices,
                                branch_lengths,
                                n_branches);
      pll_update_partials (pll_partition,
                           operations,
                           tree->get_n_inner());

      double logl = pll_compute_edge_loglikelihood (pll_partition,
                                                    pll_tree->clv_index,
                                                    pll_tree->scaler_index,
                                                    pll_tree->back->clv_index,
                                                    pll_tree->back->scaler_index,
                                                    pll_tree->pmatrix_index,
                                                    0);

      /* current logl changes the sign of the lk, such that the optimization
       * function can minimize the score */
      double cur_logl = logl * -1;

      /* notify initial likelihood */
      opt_delta = cur_logl;
      notify();

      /* logl intialized to an arbitrary value above the current lk */
      logl = cur_logl + 10;
      mt_index_t n_iters = 0;   /* iterations counter */

#if(CHECK_LOCAL_CONVERGENCE)
      double test_logl;         /* temporary variable */
      mt_size_t converged = 0;  /* bitvector for parameter convergence */
#endif

      mt_index_t cur_parameter_index = 0;

      if (params_to_optimize.size())
      {
          mt_size_t iters_hard_limit = 200;
          while ((!interrupt_optimization) &&
                  ((n_iters % params_to_optimize.size()) ||
                (fabs (cur_logl - logl) > epsilon && cur_logl < logl)))
          {
              double iter_logl;
              if (!(n_iters % params_to_optimize.size()))
                logl = cur_logl;

              if (!on_run)
              {
                  optimized = false;

                  model->set_lnl(0.0);
                  model->set_exec_time(0);

                   if (model->is_G())
                      model->set_alpha(0.0);
                  if (model->is_I())
                      model->set_prop_inv(0.0);
                  return false;
              }

              mt_parameter_t cur_parameter = params_to_optimize[cur_parameter_index];
#if(CHECK_LOCAL_CONVERGENCE)
              if (!(converged & cur_parameter))
              {
                  test_logl = cur_logl;
#endif
                  bool full_range_search = n_iters<params_to_optimize.size();
                  if (false)
                  {
                      if (cur_parameter == mt_param_alpha)
                      {
                          if (model->is_I())
                              full_range_search &= !(alpha_inv_guess > 0.0);
                          else
                              full_range_search &= !(alpha_guess > 0.0);
                      }
                      else if (cur_parameter == mt_param_pinv)
                      {
                          if (model->is_G())
                              full_range_search &= !(pinv_alpha_guess > 0.0);
                          else
                              full_range_search &= !(pinv_guess > 0.0);
                      }
                  }

                  iter_logl = opt_single_parameter(cur_parameter, tolerance, full_range_search);

                  //printf(" iteration %3d %3d %.10f %.10f\n", cur_parameter, params_to_optimize.size(), iter_logl, cur_logl);

                  /* ensure we never get a worse likelihood score */
                  assert(iter_logl - cur_logl < 1e-5);
                  cur_logl = iter_logl;

                  // notify parameter optimization
                  opt_delta = cur_logl;
                  notify();

#if(CHECK_LOCAL_CONVERGENCE)
                  if (fabs(test_logl - cur_logl) < tolerance)
                      converged |= cur_parameter;
              }
#endif

              cur_parameter_index++;
              cur_parameter_index %= params_to_optimize.size();

              n_iters++;
              iters_hard_limit--;
              if (!iters_hard_limit)
                  break;
          }

          /* TODO: if bl are reoptimized */
          if (keep_branch_lengths)
            tree->set_bl_optimized();
      }
      cur_logl *= -1;

      time_t end_time = time(NULL);

      if (_num_threads > 1)
      {
          /* finalize */
          thread_job = JOB_FINALIZE;

          /* join threads */
          for (mt_index_t i=1; i<_num_threads; i++)
            pthread_join (threads[i], NULL);

          /* clean */
          for (mt_index_t i=0; i<_num_threads; i++)
            dealloc_partition_local(thread_data[i].partition);

          free(result_buf);
          free(threads);
          free(thread_data);
          free(partition_local);
      }

      if (interrupt_optimization)
      {
          return false;
      }
      else
      {
          optimized = true;
          model->set_lnl(cur_logl);
          model->set_exec_time(end_time - start_time);

          if (model->is_G())
              model->set_alpha(params->lk_params.alpha_value);
          if (model->is_I())
              model->set_prop_inv(pll_partition->prop_invar[0]);

//          if (model->is_G() && model->is_I())
//          {
//              alpha_inv_guess = params->lk_params.alpha_value;
//              pinv_alpha_guess = pll_partition->prop_invar[0];
//          }
//          else
//          {
//              if (model->is_G())
//                  alpha_guess = params->lk_params.alpha_value;
//              if (model->is_I())
//                  pinv_guess = pll_partition->prop_invar[0];
//          }
          if (model->get_datatype() == dt_dna)
          {
            model->set_frequencies(pll_partition->frequencies[0]);
            model->set_subst_rates(pll_partition->subst_params[0]);
          }

          model->evaluate_criteria(n_branches, params->lk_params.partition->sites);
          model->set_tree((pll_utree_t *) tree->extract_tree(thread_number));
          return true;
      }
  }

  /* PTHREADS */

  void ModelOptimizerPll::start_job_sync(int JOB, thread_data_t * td)
  {
    thread_job = JOB;
    td->trap = 0;
    worker(td);
  }

  void * ModelOptimizerPll::worker(void * void_data)
  {
    thread_data_t * local_thread_data;
    unsigned int i;
    unsigned int id, mat_count, mat_offset, mat_start;
    unsigned int * local_matrix_indices;
    double * local_branch_lengths;

    local_thread_data = (thread_data_t *) void_data;
    id = (unsigned int) local_thread_data->thread_id;
    mat_count  = (unsigned int) (local_thread_data->matrix_count / local_thread_data->num_threads);
    mat_offset = (unsigned int) (local_thread_data->matrix_count % local_thread_data->num_threads);
    mat_start = id * mat_count + ((id<mat_offset)?id:mat_offset);
    mat_count += (id < mat_offset)?1:0;
    local_matrix_indices = local_thread_data->matrix_indices + mat_start;
    local_branch_lengths = local_thread_data->branch_lengths + mat_start;

    do
    {
      /* wait */
      switch (thread_job)
        {
        case JOB_FINALIZE:
          {
            /* finalize */
            local_thread_data->trap = 0;
            break;
          }
        case JOB_UPDATE_MATRICES:
        {
          barrier (local_thread_data);

            /* check eigen */
            if (!local_thread_data->partition->eigen_decomp_valid[0])
            {
              barrier (local_thread_data);
              if (!id)
                pll_update_eigen (local_thread_data->partition, 0);
              barrier (local_thread_data);
              if (!id)
                local_thread_data->partition->eigen_decomp_valid[0] = 1;
            }

            barrier (local_thread_data);

            pll_update_prob_matrices (local_thread_data->partition, 0,
                                      local_matrix_indices,
                                      local_branch_lengths,
                                      mat_count);

            if (!id)
              thread_job = JOB_WAIT;
            barrier (local_thread_data);
            break;
          }
        case JOB_UPDATE_PARTIALS:
          {
            barrier (local_thread_data);

            pll_update_partials (local_thread_data->partition,
                                 local_thread_data->operations,
                                 local_thread_data->ops_count);
            if (!id)
              thread_job = JOB_WAIT;
            barrier (local_thread_data);
            break;
          }
        case JOB_REDUCE_LK_EDGE:
          {
            if (!id)
              global_lnl = 0;
            barrier (local_thread_data);

            local_thread_data->result_buf[id] =
                pll_compute_edge_loglikelihood (
                  local_thread_data->partition,
                  local_thread_data->vroot->clv_index,
                  local_thread_data->vroot->scaler_index,
                  local_thread_data->vroot->back->clv_index,
                  local_thread_data->vroot->back->scaler_index,
                  local_thread_data->vroot->pmatrix_index, 0);

            /* reduce */
            barrier (local_thread_data);
            if (!id)
              for (i=0; i<local_thread_data->num_threads; i++)
                global_lnl += local_thread_data->result_buf[i];
            barrier (local_thread_data);

            if (!id)
              thread_job = JOB_WAIT;
            barrier (local_thread_data);
            break;
          }
        case JOB_FULL_LK:
          {
            /* execute */
            if (!id)
              global_lnl = 0;
            barrier (local_thread_data);

            pll_update_partials (local_thread_data->partition, local_thread_data->operations,
                                 local_thread_data->ops_count);

            local_thread_data->result_buf[id] =
                pll_compute_edge_loglikelihood (
                  local_thread_data->partition,
                  local_thread_data->vroot->clv_index,
                  local_thread_data->vroot->scaler_index,
                  local_thread_data->vroot->back->clv_index,
                  local_thread_data->vroot->back->scaler_index,
                  local_thread_data->vroot->pmatrix_index, 0);

            /* reduce */
            barrier (local_thread_data);
            if (!id)
              for (i = 0; i < local_thread_data->num_threads; i++)
                global_lnl += local_thread_data->result_buf[i];
            barrier (local_thread_data);

            if (!id)
              thread_job = JOB_WAIT;
            barrier (local_thread_data);
            break;
          }
        }
    } while (local_thread_data->trap);

    return 0;
  }

} /* namespace modeltest */

#undef JOB_WAIT
#undef JOB_UPDATE_MATRICES
#undef JOB_FULL_LK
#undef JOB_UPDATE_PARTIALS
#undef JOB_REDUCE_LK_EDGE
#undef JOB_FINALIZE
