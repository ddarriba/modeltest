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

/* a callback function for performing a full traversal */
static int cb_full_traversal(pll_utree_t * node)
{
    UNUSED(node);
    return 1;
}

/* a callback function for resetting all branches */
static int cb_reset_branches(pll_utree_t * node)
{

    UNUSED(node);

    /* reset branches */
    node->length = 0.1;
    node->back->length = 0.1;

    return 1;
}

namespace modeltest
{

bool on_run = true;
static bool keep_branch_lengths = false;

ModelOptimizer::~ModelOptimizer() {}

ModelOptimizerPll::ModelOptimizerPll (MsaPll *_msa,
                                      TreePll *_tree,
                                      Model *_model,
                                      const partition_t &_partition,
                                      mt_size_t _n_cat_g,
                                      mt_index_t _thread_number)
    : ModelOptimizer(_model, _partition),
      msa(_msa),
      tree(_tree),
      thread_number(_thread_number)
{
    params = NULL;
    operations = NULL;
    branch_lengths = NULL;
    matrix_indices = NULL;

    mt_size_t n_tips = tree->get_n_tips ();
    mt_size_t n_sites = 0;
    for (const partition_region_t & region : partition.regions)
    {
        n_sites += (region.end - region.start + 1)/region.stride;
    }
    pll_partition = pll_partition_create (
                n_tips,                           /* tips */
                (n_tips - 2),                     /* clv buffers */
                model->get_n_states(),            /* states */
                n_sites,                          /* sites */
                0,                                /* mixture model */
                1,                                /* rate matrices */
                (2 * n_tips - 3),                 /* prob matrices */
                model->is_G () ? _n_cat_g : 1,    /* rate cats */
                n_tips - 2,                       /* scale buffers */
                PLL_ATTRIB_ARCH_SSE               /* attributes */
                );
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
                2 * n_tips - 2, sizeof(pll_utree_t *));

    /* additional stuff */
    mt_size_t traversal_size;
    if (!pll_utree_traverse (pll_tree,
                             cb_reset_branches,
                             travbuffer,
                             &traversal_size))
    {
      std::cerr << 
        "ERROR: PLL returned an error computing tree traversal" <<
        std::endl;
      assert(0);
    }

    assert(traversal_size > 0);

    branch_lengths = new double[2 * n_tips - 3];
    matrix_indices = new mt_index_t[2 * n_tips - 3];
    operations = new pll_operation_t[n_tips - 2];
    mt_size_t matrix_count;
    mt_size_t ops_count;

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

  double ModelOptimizerPll::opt_single_parameter(mt_index_t which_parameter,
                                                 double tolerance)
  {
      double cur_logl;

      if (params == NULL)
          build_parameters();

      params->pgtol = tolerance;

      if (which_parameter == PLL_PARAMETER_BRANCHES_ITERATIVE)
      {
          int smoothings = 2;
          mt_index_t matrix_count, ops_count;
          pll_utree_t* pll_tree;
          pll_utree_t ** travbuffer;
          mt_size_t traversal_size;

          /* move to random node */
          tree->reroot_random(thread_number);
          pll_tree = tree->get_pll_tree(thread_number);


          travbuffer = (pll_utree_t **) Utils::allocate (2*tree->get_n_tips() - 2, sizeof(pll_utree_t *));
          pll_utree_traverse (pll_tree, cb_full_traversal, travbuffer, &traversal_size);
          pll_utree_create_operations (travbuffer, traversal_size, branch_lengths,
                                       matrix_indices, operations, &matrix_count,
                                       &ops_count);
          pll_update_prob_matrices (pll_partition, 0, matrix_indices, branch_lengths,
                                    2 * tree->get_n_tips() - 3);
          pll_update_partials (pll_partition, operations, tree->get_n_tips() - 2);

          params->which_parameters = PLL_PARAMETER_BRANCHES_ITERATIVE;

          cur_logl = pll_optimize_branch_lengths_iterative (
                      pll_partition, pll_tree, params->params_index, params->lk_params.freqs_index,
                      params->pgtol, smoothings++, true);

          pll_utree_traverse (pll_tree, cb_full_traversal, travbuffer, &traversal_size);
          pll_utree_create_operations (travbuffer, traversal_size, branch_lengths,
                                       matrix_indices, operations, &matrix_count,
                                       &ops_count);
          params->lk_params.where.unrooted_t.parent_clv_index = pll_tree->clv_index;
          params->lk_params.where.unrooted_t.parent_scaler_index =
                  pll_tree->scaler_index;
          params->lk_params.where.unrooted_t.child_clv_index = pll_tree->back->clv_index;
          params->lk_params.where.unrooted_t.child_scaler_index =
                  pll_tree->back->scaler_index;
          params->lk_params.where.unrooted_t.edge_pmatrix_index =
                  pll_tree->pmatrix_index;

          free(travbuffer);
      }
      else if (which_parameter == PLL_PARAMETER_ALPHA || which_parameter == PLL_PARAMETER_PINV)
      {
          params->which_parameters = which_parameter;
          cur_logl = pll_optimize_parameters_brent (params);
      }
      else
      {
          params->which_parameters = which_parameter;
          cur_logl = pll_optimize_parameters_lbfgsb (params);
      }

      return cur_logl;
  }

  bool ModelOptimizerPll::build_parameters()
  {
      assert (params == NULL);

      double default_alpha = 0.5;
      pll_utree_t* pll_tree = tree->get_pll_tree(thread_number);

      params = new pll_optimize_options_t;
      if (!params)
          return false;

      /* pll stuff */
      params->lk_params.partition = pll_partition;
      params->lk_params.operations = operations;
      params->lk_params.branch_lengths = branch_lengths;
      params->lk_params.matrix_indices = matrix_indices;
      params->lk_params.alpha_value = default_alpha;
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

      return true;
  }

  bool ModelOptimizerPll::run(double epsilon,
                              double tolerance)
  {
#ifdef VERBOSE //TODO: Verbosity medium
      std::cout << "Optimizing model " << model->get_name() << std::endl;
#endif
      time_t start_time = time(NULL);

      if (params == NULL)
        build_parameters();

      double * rate_cats = (double *) Utils::c_allocate( pll_partition->rate_cats, sizeof(double));

      if (!model->is_I())
          pll_update_invariant_sites_proportion(pll_partition, 0, 0.0);
      else
          pll_update_invariant_sites_proportion(pll_partition, 0, 0.5);

      if (model->is_F())
          model->set_frequencies(partition.empirical_freqs);

      pll_set_frequencies (pll_partition, 0, 0, model->get_frequencies());
      pll_set_subst_params (pll_partition, 0, 0, model->get_subst_rates());

      if (model->is_G())
      {
          pll_compute_gamma_cats (params->lk_params.alpha_value,
                                  pll_partition->rate_cats,
                                  rate_cats);
      }
      else
      {
          assert( pll_partition->rate_cats == 1);
          rate_cats[0] = 1.0;
      }

      pll_set_category_rates (pll_partition, rate_cats);
      free(rate_cats);

#ifdef VERBOSE //TODO: Verbosity high
      std::cout << "Initial Frequencies:   ";
      for (mt_index_t i=0; i<pll_partition->states; i++)
        std::cout << pll_partition->frequencies[0][i] << " ";
      std::cout << std::endl;
#endif

//      tree->reroot_random(thread_number);
      pll_utree_t * pll_tree = tree->get_pll_start_tree(thread_number);

      tree->set_branches(0.15, thread_number);
      for (int i=0; i<2 * tree->get_n_tips() - 3; i++)
          branch_lengths[i] = 0.15;
      pll_update_prob_matrices (pll_partition, 0, matrix_indices, branch_lengths,
                                2 * tree->get_n_tips() - 3);
      pll_update_partials (pll_partition, operations, tree->get_n_tips() - 2);

      double logl = pll_compute_edge_loglikelihood (pll_partition,
                                                    pll_tree->clv_index,
                                                    pll_tree->scaler_index,
                                                    pll_tree->back->clv_index,
                                                    pll_tree->back->scaler_index,
                                                    pll_tree->pmatrix_index,
                                                    0);

      /* current logl changes the sign of the lk, such that the optimization
       * function can minimize the score
       */
      double cur_logl = logl * -1;
      /* logl intialized to an arbitrary value above the current lk */
      logl = cur_logl + 10;
      mt_index_t n_iters = 0;       /* iterations counter */

      double test_logl;         /* temporary variable */
      mt_size_t converged = 0;  /* bitvector for parameter convergence */

      std::vector<mt_index_t> params_to_optimize;

      if (model->get_datatype() == dt_dna || !tree->is_bl_optimized())
          params_to_optimize.push_back(PLL_PARAMETER_BRANCHES_ITERATIVE);
      if (model->get_datatype() == dt_dna && model->get_n_subst_params() > 0)
          params_to_optimize.push_back(PLL_PARAMETER_SUBST_RATES);
      if (model->is_G())
          params_to_optimize.push_back(PLL_PARAMETER_ALPHA);
      if (model->is_I())
          params_to_optimize.push_back(PLL_PARAMETER_PINV);
      if (model->get_datatype() == dt_dna && model->is_F())
          params_to_optimize.push_back(PLL_PARAMETER_FREQUENCIES);

      mt_index_t cur_parameter_index = 0;

      if (params_to_optimize.size())
      {
          mt_size_t iters_hard_limit = 200;
          while (n_iters < params_to_optimize.size() || (fabs (cur_logl - logl) > epsilon && cur_logl < logl))
          {
              n_iters++;
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

              mt_index_t cur_parameter = params_to_optimize[cur_parameter_index];
              if (!(converged & cur_parameter))
              {
                  test_logl = cur_logl;
                  cur_logl = opt_single_parameter(cur_parameter, tolerance);
//                  if (fabs(test_logl - cur_logl) < tolerance)
//                      converged |= cur_parameter;
              }

              cur_parameter_index++;
              cur_parameter_index %= params_to_optimize.size();

              iters_hard_limit--;
              assert(iters_hard_limit);
          }
          /* TODO: if bl are reoptimized */
          if (keep_branch_lengths)
            tree->set_bl_optimized();
      }
      cur_logl *= -1;

      time_t end_time = time(NULL);

      optimized = true;

      model->set_lnl(cur_logl);
      model->set_exec_time(end_time - start_time);

      if (model->is_G())
          model->set_alpha(params->lk_params.alpha_value);
      if (model->is_I())
          model->set_prop_inv(pll_partition->prop_invar[0]);

      if (model->get_datatype() == dt_dna)
      {
        model->set_frequencies(pll_partition->frequencies[0]);
        model->set_subst_rates(pll_partition->subst_params[0]);
      }

      model->evaluate_criteria(2*tree->get_n_tips() - 3, params->lk_params.partition->sites);

      return true;
  }
} /* namespace modeltest */
