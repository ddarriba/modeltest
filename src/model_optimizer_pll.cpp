/*
 * model_optimizer_pll.cpp
 *
 *  Created on: Jun 10, 2015
 *      Author: diego
 */

#include "model_optimizer_pll.h"

#include <cassert>
#include <iostream>

/* a callback function for performing a full traversal */
static int cb_full_traversal(pll_utree_t * node)
{
    UNUSED(node);
    return 1;
}

/* a callback function for performing a full traversal */
static int cb_reset_branches(pll_utree_t * node)
{

    UNUSED(node);
    /* reset branches */
//    node->length = 0.1;
//    node->back->length = 0.1;

    return 1;
}

namespace modeltest
{

bool on_run = true;

ModelOptimizerPll::ModelOptimizerPll (MsaPll *msa,
                                      TreePll *tree,
                                      Model *model,
                                      mt_size_t n_cat_g,
                                      mt_index_t thread_number)
    : ModelOptimizer(model), msa(msa), tree(tree),
      thread_number(thread_number)
{
    params = NULL;
    operations = NULL;
    branch_lengths = NULL;
    matrix_indices = NULL;

    mt_size_t n_tips = tree->get_n_tips ();
    pll_partition = pll_partition_create (n_tips, (n_tips - 2), 4,
                                          (mt_size_t) msa->get_n_sites (), 1,
                                          (2 * n_tips - 3),
                                          model->is_G () ? n_cat_g : 1,
                                          n_tips - 2,
                                          PLL_ATTRIB_ARCH_SSE);
    assert(pll_partition);

    pll_utree_t* pll_tree = tree->get_pll_tree (thread_number);

    pll_utree_t ** tipnodes = (pll_utree_t **) calloc (n_tips,
                                                       sizeof(pll_utree_t *));
    assert(pll_utree_query_tipnodes (pll_tree, tipnodes));

    /* find sequences and link them with the corresponding taxa */
    for (mt_index_t i = 0; i < n_tips; ++i)
    {
        int tip_clv_index = -1;
        const char * header = msa->get_header (i);

        for (mt_index_t j = 0; j < n_tips; ++j)
        {
            if (!strcmp (tipnodes[j]->label, header))
            {
                tip_clv_index = j;
                break;
            }
        }

        if (tip_clv_index == -1)
            std::cerr << "ERROR: Cannot find tip \"" << header << "\"" << std::endl;
        assert(tip_clv_index > -1);

        pll_set_tip_states (pll_partition, (unsigned int)tip_clv_index, pll_map_nt,
                            msa->get_sequence (i));
    }

    free (tipnodes);

    /* allocate a buffer for storing pointers to nodes of the tree in postorder
     traversal */
    pll_utree_t ** travbuffer = (pll_utree_t **) malloc (
                (2 * n_tips - 2) * sizeof(pll_utree_t *));

    /* additional stuff */
    mt_size_t traversal_size = pll_utree_traverse (pll_tree, cb_reset_branches,
                                             travbuffer);

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
          params->which_parameters = PLL_PARAMETER_BRANCHES_SINGLE;
          pll_utree_t* pll_tree = tree->get_pll_tree(thread_number);

          cur_logl = pll_optimize_branch_lengths_iterative (params, pll_tree, 5);

          pll_utree_t ** travbuffer = (pll_utree_t **)malloc((2*tree->get_n_tips() - 2) * sizeof(pll_utree_t *));
          int traversal_size = pll_utree_traverse (pll_tree,
                                                   cb_full_traversal,
                                                   travbuffer);
          mt_index_t matrix_count, ops_count;
          pll_utree_create_operations(travbuffer,
                                      traversal_size,
                                      branch_lengths,
                                      matrix_indices,
                                      operations,
                                      &matrix_count,
                                      &ops_count);
          free(travbuffer);
          params->lk_params.where.unrooted_t.parent_clv_index = pll_tree->clv_index;
          params->lk_params.where.unrooted_t.parent_scaler_index = pll_tree->scaler_index;
          params->lk_params.where.unrooted_t.child_clv_index = pll_tree->back->clv_index;
          params->lk_params.where.unrooted_t.child_scaler_index = pll_tree->back->scaler_index;
          params->lk_params.where.unrooted_t.edge_pmatrix_index = pll_tree->pmatrix_index;
      }
      else if (which_parameter == PLL_PARAMETER_FREQUENCIES)
      {
          params->freq_ratios = (double *) calloc ((size_t) N_STATES - 1,
                                                  sizeof(double));
          for (int i = 0; i < (N_STATES - 1); i++)
          {
              params->freq_ratios[i] =
                      pll_partition->frequencies[params->lk_params.freqs_index][i]
                      / pll_partition->frequencies[params->lk_params.freqs_index][N_STATES
                      - 1];
          }
          params->which_parameters = PLL_PARAMETER_FREQUENCIES;
          cur_logl = pll_optimize_parameters_lbfgsb (params);
          free (params->freq_ratios);
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
      params->params_index = 0;
      int *symmetries = new int[N_SUBST_RATES];
      memcpy(symmetries, model->get_symmetries(), N_SUBST_RATES * sizeof(int));
      params->subst_params_symmetries = symmetries;
      params->factr = 1e9;

      return true;
  }

  bool ModelOptimizerPll::run(double epsilon,
                              double tolerance)
  {
      time_t start_time = time(NULL);

      if (params == NULL)
        build_parameters();

      double * rate_cats = (double *) calloc( pll_partition->rate_cats, sizeof(double));

      if (!model->is_I())
          pll_update_invariant_sites_proportion(pll_partition, 0, 0.0);
      else
          pll_update_invariant_sites_proportion(pll_partition, 0, 0.5);

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
      pll_set_frequencies (pll_partition, 0, model->get_frequencies());
      pll_set_subst_params (pll_partition, 0, model->get_subst_rates());
      pll_set_category_rates (pll_partition, rate_cats);
      free(rate_cats);

      pll_update_prob_matrices (pll_partition, 0, matrix_indices, branch_lengths,
                                2 * tree->get_n_tips() - 3);
      pll_update_partials (pll_partition, operations, tree->get_n_tips() - 2);

      pll_utree_t* pll_tree = tree->get_pll_tree(thread_number);
      double logl = pll_compute_edge_loglikelihood (pll_partition,
                                                    pll_tree->clv_index,
                                                    pll_tree->scaler_index,
                                                    pll_tree->back->clv_index,
                                                    pll_tree->back->scaler_index,
                                                    pll_tree->pmatrix_index,
                                                    0);

      double cur_logl = logl * -1;
      logl = cur_logl + 10;
      mt_index_t n_iters = 0;

      double test_logl;
      int converged = 0;

      std::vector<int> params_to_optimize;
      params_to_optimize.push_back(PLL_PARAMETER_BRANCHES_ITERATIVE);
      if (model->get_n_subst_params() > 0)
          params_to_optimize.push_back(PLL_PARAMETER_SUBST_RATES);
      if (model->is_G())
          params_to_optimize.push_back(PLL_PARAMETER_ALPHA);
      if (model->is_I())
          params_to_optimize.push_back(PLL_PARAMETER_PINV);
      if (model->is_F())
          params_to_optimize.push_back(PLL_PARAMETER_FREQUENCIES);

      int cur_parameter_index = 0;

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

          int cur_parameter = params_to_optimize[cur_parameter_index];
          if (!(converged & cur_parameter))
          {
              test_logl = cur_logl;
              cur_logl = opt_single_parameter(cur_parameter, tolerance);
              if (fabs(test_logl - cur_logl) < tolerance)
                  converged |= cur_parameter;
          }

          cur_parameter_index++;
          cur_parameter_index %= params_to_optimize.size();
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
      model->set_frequencies(pll_partition->frequencies[0]);
      model->set_subst_rates(pll_partition->subst_params[0]);

      model->evaluate_criteria(2*tree->get_n_tips() - 3, params->lk_params.partition->sites);

      return true;
  }
} /* namespace modeltest */
