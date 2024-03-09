/*
  Copyright (C) 2016 Diego Darriba

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU Affero General Public License as
  published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Affero General Public License for more details.

  You should have received a copy of the GNU Affero General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  Contact: Diego Darriba <Diego.Darriba@h-its.org>,
  Heidelberg Institute for Theoretical Studies,
  Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

#include "model_optimizer_pll.h"
#include "../utils.h"
#include "../genesis/logging.h"
#include "../thread/parallel_context.h"

#include <cassert>
#include <iostream>

#define CHECK_LOCAL_CONVERGENCE 0

#define DO_THOROUGH_ML_SEARCH true
#define ML_SEARCH_PARAM_EPS   0.1
#define ML_SEARCH_PARAM_TOL   0.1

using namespace std;

namespace modeltest
{

bool on_run = true;

static mt_mask_t build_attributes(asc_bias_t asc_bias)
{
  mt_mask_t attributes = 0;

  /* subtree repeats */
  attributes |= disable_repeats?PLL_ATTRIB_PATTERN_TIP:PLL_ATTRIB_SITE_REPEATS;

  /* vector intrinsics (prioritized) */
  attributes |= have_avx2?PLL_ATTRIB_ARCH_AVX2
                 :have_avx?PLL_ATTRIB_ARCH_AVX
                 :have_sse3?PLL_ATTRIB_ARCH_SSE
                 :PLL_ATTRIB_ARCH_CPU;

  switch (asc_bias)
  {
      case asc_lewis:
        attributes |= PLL_ATTRIB_AB_FLAG | PLL_ATTRIB_AB_LEWIS; break;
      case asc_felsenstein:
        attributes |= PLL_ATTRIB_AB_FLAG | PLL_ATTRIB_AB_FELSENSTEIN; break;
      case asc_stamatakis:
        attributes |= PLL_ATTRIB_AB_FLAG | PLL_ATTRIB_AB_STAMATAKIS; break;
      case asc_none:
        break;
      default:
        /* unknown AB correction */
        assert(0);
   }

    return attributes;

}

const int radius_step = 5;

void thread_infer_topology(ModelOptimizerPll * optimizer, pllmod_treeinfo_t * tree_info, double cur_loglh, double epsilon, double tolerance)
{
    int thorough_insertion = false;
    int radius_min = 2;
    int radius_max = 5;
    int radius_limit;
    int ntopol_keep = 15;
    int spr_round_id = 0;
    double cutoff_thr = 10;
    int smoothings = 32;

    double new_loglh, old_loglh;
    Model & model = *(optimizer->get_model());
    pll_unode_t ** pll_tree_ptr = optimizer->get_pll_tree_ptr();

    assert(!tree_info->root->data);
    radius_limit = (tree_info->tip_count>25)?22:(tree_info->tip_count-3);

    new_loglh = optimizer->optimize_parameters(tree_info, 1.0, 1.0, true, cur_loglh);

    cutoff_info_t * cutoff_info = NULL;
    cutoff_info = (cutoff_info_t *) calloc(1, sizeof(cutoff_info_t));
    cutoff_info->lh_dec_count = 0;
    cutoff_info->lh_dec_sum = 0.;
    cutoff_info->lh_cutoff = new_loglh / -1000.0;

    do
    {
      ++spr_round_id;
      old_loglh = new_loglh;

      LOG_DBG << "[" << model.get_name() << "] " << fixed << setprecision(5) << new_loglh << " SPR params:"
          << " round=" << spr_round_id
          << " rad=[" << radius_min << "," << radius_max << "]"
          << " bl=[" << scientific << setprecision(1) << MT_MIN_BRANCH_LENGTH << "," << scientific << setprecision(1) << MT_MAX_BRANCH_LENGTH << "]"
          << " ntopos=" << ntopol_keep
          << " smooth=" << smoothings
          << " tol=" << tolerance
          << endl;

      new_loglh = pllmod_algo_spr_round(tree_info,
                                        radius_min, radius_max,
                                        ntopol_keep,
                                        thorough_insertion,
                                        PLLMOD_OPT_BLO_NEWTON_FAST,
                                        MT_MIN_BRANCH_LENGTH,
                                        MT_MAX_BRANCH_LENGTH,
                                        smoothings,
                                        tolerance,
                                        cutoff_info, cutoff_thr, /* cutoff */
                                        tolerance, 
                                        PLL_TRUE);

      if (pll_errno && new_loglh == 0)
      {
        LOG_DBG2 << "[" << model.get_name() << "] Error " << pll_errno << " in SPR cycle: " << pll_errmsg << endl;
        pllmod_reset_error();
        new_loglh = pllmod_treeinfo_compute_loglh(tree_info, 0);
      }

      new_loglh = optimizer->optimize_parameters(tree_info, 1.0, 1.0, true, new_loglh);

      bool impr = (new_loglh - old_loglh > epsilon);
      if (impr && thorough_insertion)
      {
        radius_min = 1;
        radius_max = radius_step;
        LOG_DBG2 << "[" << model.get_name() << "] Reset radius: " << radius_min << "," << radius_max << endl;
      }
      else if (!impr && DO_THOROUGH_ML_SEARCH)
      {
        if (!thorough_insertion)
        {
          thorough_insertion = true;
          spr_round_id = 0;
          radius_min = 1;
          radius_max = radius_step;

          LOG_DBG2 << "[" << model.get_name() << "] " << fixed << setprecision(5) << new_loglh << " reset insertion and radius: "
                  << radius_min << "," << radius_max << endl;
          new_loglh = optimizer->optimize_parameters(tree_info,
                                                     ML_SEARCH_PARAM_EPS,
                                                     ML_SEARCH_PARAM_TOL,
                                                     true,
                                                     new_loglh);
          LOG_DBG2 << "[" << model.get_name() << "] " << fixed << setprecision(5) << new_loglh << " thorough insertion [" << ML_SEARCH_PARAM_EPS
                  << "/" << ML_SEARCH_PARAM_TOL << "] " << endl;
        }
        else
        {
          radius_min += radius_step;
          radius_max += radius_step;
          LOG_DBG2 << "[" << model.get_name() << "] Update radius: " << radius_min << "," << radius_max << endl;
        }
      }
    } while (radius_min >= 0 && radius_min < radius_limit && fabs(old_loglh - new_loglh) > epsilon );

    LOG_DBG2 << "[" << model.get_name() << "] SPR done" << endl;

    free (cutoff_info);
}

void thread_main(ModelOptimizerPll * optimizer, double epsilon, double tolerance)
{
  size_t num_threads = ParallelContext::num_threads();

  pll_partition_t * pll_partition;

  Model & model = *(optimizer->get_model());
  TreePll & tree = *(optimizer->get_tree());
  Partition & partition = *(optimizer->get_partition());
  pll_unode_t ** pll_tree_ptr = optimizer->get_pll_tree_ptr();

  // synchronize and reset barrier state
  ParallelContext::thread_barrier(true);

  mt_size_t n_tips = tree.get_n_tips();
  mt_size_t n_patterns = partition.get_n_patterns();
  mt_index_t thread_number = optimizer->get_thread_number();

  *pll_tree_ptr = tree.get_pll_tree(thread_number)->nodes[0]->back;

  pllmod_treeinfo_t * tree_info = pllmod_treeinfo_create(optimizer->is_keep_model_parameters()
                                        ?*pll_tree_ptr
                                        :pll_utree_graph_clone(*pll_tree_ptr),
                                      tree.get_n_tips(),
                                      1,
                                      1);

  assert(tree_info);

  pllmod_treeinfo_set_parallel_context(tree_info, (void *) nullptr,
                                       ParallelContext::parallel_reduce_cb);

  /* build pll partition */

  mt_mask_t attributes = build_attributes(model.get_asc_bias_corr());

  mt_size_t my_length = n_patterns / num_threads;
  mt_size_t my_start = ParallelContext::thread_id() * my_length;
  if ((ParallelContext::thread_id() == num_threads - 1) && (n_patterns % num_threads))
  {
    my_length = n_patterns - (my_length * (num_threads-1));
  }

  pll_partition = pll_partition_create(
      n_tips,                      /* tips */
      n_tips - 2,                  /* clv buffers */
      model.get_n_states(),        /* states */
      my_length,                   /* sites */
      model.get_n_rate_matrices(), /* rate matrices */
      2 * n_tips - 3,              /* prob matrices */
      model.get_n_categories(),    /* rate cats */
      n_tips - 2,                  /* scale buffers */
      attributes                   /* attributes */
  );

  if (model.get_asc_weights())
    pll_set_asc_state_weights(pll_partition, model.get_asc_weights());

  if (!pll_partition)
  {
    mt_errno = MT_ERROR_LIBPLL;
    snprintf(mt_errmsg, ERR_MSG_SIZE, "[PLL %d] %s",
              pll_errno, pll_errmsg);
    throw EXCEPTION_INTERNAL_ERROR;
  }

  /* find sequences and link them with the corresponding taxa */
  for (mt_index_t i = 0; i < n_tips; ++i)
  {
    const char *c_seq = partition.get_sequence(i) + my_start;
    const pll_state_t *states_map =
        (model.get_datatype() == dt_dna) ? (model.is_gap_aware() ? extended_dna_map : pll_map_nt)
                                          : pll_map_aa;

    if (!pll_set_tip_states(pll_partition,
                            i,
                            states_map,
                            c_seq))
    {
      /* data type and tip states should be validated beforehand */
      mt_errno = MT_ERROR_IO_FORMAT;
      snprintf(mt_errmsg, ERR_MSG_SIZE, "Data type mismatch");
      throw EXCEPTION_DATA_MISMATCH;
    }
  }

  /* set weights */
  const mt_size_t *weights = partition.get_weights() + my_start;
  pll_set_pattern_weights(pll_partition, weights);

  ParallelContext::thread_barrier();

  LOG_DBG2 << "[" << model.get_name() << "][" << ParallelContext::thread_id() 
           << "] build partition" << endl;

  int retval = pllmod_treeinfo_init_partition(tree_info,
                                                 0,
                                                 pll_partition,
                                                 0xFFFF,
                                                 optimizer->get_gamma_rates_mode(),
                                                 model.get_alpha(),
                                                 model.get_params_indices(),
                                                 model.get_symmetries());

  if (!retval)
  {
    assert(pll_errno);
    Utils::exit_with_error("Init partition error: %s\n", pll_errmsg);
  }

  if (!model.optimize_init(tree_info,
                           optimizer->get_gamma_rates_mode(),
                           partition))
  {
    assert(0);
  }
  
  LOG_DBG2 << "[" << model.get_name() << "][" << ParallelContext::thread_id() 
           << "] compute initial LogLH" << endl;

  double cur_loglh = pllmod_treeinfo_compute_loglh(tree_info, 0);

  if (ParallelContext::master_thread())
  {
    stringstream ss;
    ss << "[" << model.get_name() << "] initial Frequencies: {";
    for (mt_index_t i=0; i<pll_partition->states; i++)
      ss << " " << fixed << setprecision(4) << pll_partition->frequencies[0][i];
    ss << " }" << endl;
    LOG_DBG2 << ss.str();

    LOG_DBG2 << "[" << model.get_name() << "] initial log-Likelihood: "
             << fixed << setprecision(4) << cur_loglh << endl; 
  }
             
  if (optimizer->is_optimize_topology())
  {
    thread_infer_topology(optimizer, tree_info, cur_loglh, epsilon, tolerance);

    if (ParallelContext::master_thread())
        model.set_tree(tree_info->root);
  }

  /* final thorough parameter optimization */
  LOG_DBG2 << "[" << model.get_name() << "][" << ParallelContext::thread_id() << "] thorough parameter optimization" << endl;
  cur_loglh = optimizer->optimize_parameters(tree_info,
                                 epsilon,
                                 tolerance,
                                 true,
                                 cur_loglh);

  if (ParallelContext::master_thread())
    model.set_loglh(cur_loglh);
  
  if (!optimizer->is_keep_model_parameters() && !optimizer->is_optimize_topology())
  {
    pll_utree_destroy(tree_info->tree, NULL);
    tree_info->tree = 0;
  }

  pll_partition_destroy(tree_info->partitions[0]);
  pllmod_treeinfo_destroy(tree_info);
}


ModelOptimizer::~ModelOptimizer() {}

ModelOptimizerPll::ModelOptimizerPll (MsaPll &_msa,
                                      TreePll &_tree,
                                      Model &_model,
                                      Partition &_partition,
                                      bool _optimize_topology,
                                      bool _keep_model_parameters,
                                      int _gamma_rates,
                                      mt_size_t _n_threads,
                                      mt_index_t _thread_number)
    : ModelOptimizer(_msa, _model, _partition,
                     _optimize_topology, _keep_model_parameters,
                     _gamma_rates, _n_threads, _thread_number),
      tree(_tree)
{

  
}

  ModelOptimizerPll::~ModelOptimizerPll ()
  {

  }

  bool ModelOptimizerPll::run(double epsilon,
                              double tolerance)
  {
    time_t start_time = time(NULL);
    mt_size_t n_branches = tree.get_n_branches();

    ParallelContext::init_threads(n_threads, std::bind(thread_main, this, epsilon, tolerance));

    LOG_DBG2 << "[" << model.get_name() << "] start optimization with " << n_threads << " threads" << endl;

    thread_main(this, epsilon, tolerance);

    ParallelContext::finalize_threads();

    //LOG_DBG << "[" << model.get_name() << "] building parameters and computing initial lk score" << endl;
    //pllmod_utree_compute_lk(pll_partition, pll_tree, model.get_params_indices(), 1, 1);

#if(CHECK_LOCAL_CONVERGENCE)
    double test_loglh;         /* temporary variable */
    mt_size_t converged = 0;  /* bitvector for parameter convergence */
#endif

    //double cur_loglh = optimize_model(epsilon, tolerance, true);

    /* TODO: if bl are reoptimized */
    if (keep_model_parameters)
      tree.set_bl_optimized();

    time_t end_time = time(NULL);

    if (interrupt_optimization)
    {
        return false;
    }
    else
    {
      optimized = true;
      //model.set_loglh(cur_loglh);
      model.set_exec_time(end_time - start_time);

      model.evaluate_criteria(n_branches, msa.get_n_sites());

      // if (optimize_topology)
      //   model.set_tree(pll_tree);

      return true;
    }
  }

  double ModelOptimizerPll::optimize_parameters( pllmod_treeinfo_t * tree_info,
                                                 double epsilon,
                                                 double tolerance,
                                                 bool opt_per_param,
                                                 double start_loglh )
{
    /* current loglh changes the sign of the lk, such that the optimization
     * function can minimize the score */
    double save_loglh = start_loglh;
    double cur_loglh = save_loglh;

    if (start_loglh && ParallelContext::master_thread())
      model.set_loglh(start_loglh);

    /* notify initial likelihood */
    opt_delta = cur_loglh;
    notify();

    /* loglh intialized to an arbitrary value above the current lk */
    save_loglh = cur_loglh - epsilon - 1;

    if (opt_per_param)
    {
      bool all_params_done = false;
      while ((fabs (cur_loglh - save_loglh) > epsilon && cur_loglh > save_loglh))
      {
        save_loglh = cur_loglh;
        all_params_done = false;
        while ((!all_params_done) && (!interrupt_optimization))
        {
          all_params_done = model.optimize_oneparameter(tree_info,
                                                        tolerance);
          cur_loglh = model.get_loglh();

          /* ensure we never get a worse likelihood score */
          if (save_loglh - cur_loglh > 1e-5)
          {
              LOG_ERR << "[" << model.get_name() << "] "
                      << fixed << setprecision(5) << save_loglh << " vs "
                      << setprecision(5) << cur_loglh
                      << " [" << cur_parameter << "]" << endl;
              Utils::exit_with_error("Optimization converged to a worse likelihood score by %.5lf units",
                                     cur_loglh - save_loglh);
          }
          opt_delta = cur_loglh;
          notify();
        }
      }
    }
    else
    {
      while ((!interrupt_optimization) &&
        (fabs (cur_loglh - save_loglh) > epsilon && cur_loglh > save_loglh))
      {
          save_loglh = cur_loglh;
          model.optimize(tree_info, tolerance);
          opt_delta = cur_loglh;
          notify();
          cur_loglh = model.get_loglh();
      }
    }

    return cur_loglh;
  }

  double ModelOptimizerPll::optimize_model( double epsilon,
                                            double tolerance,
                                            bool opt_per_param )
  {
    double loglh;
    double old_loglh,
           new_loglh;

    int thorough_insertion = false;
    int radius_min = 2;
    int radius_max = 5;
    int radius_limit;
    int ntopol_keep = 15;
    int spr_round_id = 0;
    int smoothings = 32;
    double cutoff_thr = 10;
    pllmod_treeinfo_t * tree_info;

    loglh = pll_compute_edge_loglikelihood (pll_partition,
                                           pll_tree->clv_index,
                                           pll_tree->scaler_index,
                                           pll_tree->back->clv_index,
                                           pll_tree->back->scaler_index,
                                           pll_tree->pmatrix_index,
                                           model.get_params_indices(),
                                           NULL);

    new_loglh = loglh;

    LOG_DBG << "[" << model.get_name() << "] " << fixed << setprecision(5) << loglh << " initial log likelihood" << endl;
    tree_info = pllmod_treeinfo_create(keep_model_parameters
                                         ?pll_tree
                                         :pll_utree_graph_clone(pll_tree),
                                       tree.get_n_tips(),
                                       1,
                                       1);

     assert(gamma_rates == PLL_GAMMA_RATES_MEAN ||
            gamma_rates == PLL_GAMMA_RATES_MEDIAN);

     int retval = pllmod_treeinfo_init_partition(tree_info,
                                                 0,
                                                 pll_partition,
                                                 0xFFFF,
                                                 gamma_rates,
                                                 model.get_alpha(),
                                                 model.get_params_indices(),
                                                 model.get_symmetries());

     if (!retval)
     {
       assert(pll_errno);
       Utils::exit_with_error("Init partition error: %s\n", pll_errmsg);
     }

    if (optimize_topology)
    {
      assert(!pll_tree->data);
      radius_limit = (tree_info->tip_count>25)?22:(tree_info->tip_count-3);

      new_loglh = optimize_parameters(tree_info, 1.0, 1.0, opt_per_param, new_loglh);

      cutoff_info_t * cutoff_info = NULL;
      cutoff_info = (cutoff_info_t *) calloc(1, sizeof(cutoff_info_t));
      cutoff_info->lh_dec_count = 0;
      cutoff_info->lh_dec_sum = 0.;
      cutoff_info->lh_cutoff = loglh / -1000.0;

      do
      {
        ++spr_round_id;
        old_loglh = new_loglh;

        LOG_DBG << "[" << model.get_name() << "] " << fixed << setprecision(5) << new_loglh << " SPR params:"
            << " round=" << spr_round_id
            << " rad=[" << radius_min << "," << radius_max << "]"
            << " bl=[" << scientific << setprecision(1) << MT_MIN_BRANCH_LENGTH << "," << scientific << setprecision(1) << MT_MAX_BRANCH_LENGTH << "]"
            << " ntopos=" << ntopol_keep
            << " smooth=" << smoothings
            << " tol=" << tolerance
            << endl;

        new_loglh = pllmod_algo_spr_round(tree_info,
                                          radius_min, radius_max,
                                          ntopol_keep,
                                          thorough_insertion,
                                          PLLMOD_OPT_BLO_NEWTON_FAST,
                                          MT_MIN_BRANCH_LENGTH,
                                          MT_MAX_BRANCH_LENGTH,
                                          smoothings,
                                          tolerance,
                                          cutoff_info, cutoff_thr, /* cutoff */
                                          tolerance, 
                                          PLL_TRUE);

        if (pll_errno && new_loglh == 0)
        {
          LOG_DBG2 << "[" << model.get_name() << "] Error " << pll_errno << " in SPR cycle: " << pll_errmsg << endl;
          pllmod_reset_error();
          new_loglh = pllmod_treeinfo_compute_loglh(tree_info, 0);
        }

        new_loglh = optimize_parameters(tree_info, 1.0, 1.0, opt_per_param, new_loglh);

        bool impr = (new_loglh - old_loglh > epsilon);
        if (impr && thorough_insertion)
        {
          radius_min = 1;
          radius_max = radius_step;
          LOG_DBG2 << "[" << model.get_name() << "] Reset radius: " << radius_min << "," << radius_max << endl;
        }
        else if (!impr && DO_THOROUGH_ML_SEARCH)
        {
          if (!thorough_insertion)
          {
            thorough_insertion = true;
            spr_round_id = 0;
            radius_min = 1;
            radius_max = radius_step;

            LOG_DBG2 << "[" << model.get_name() << "] " << fixed << setprecision(5) << new_loglh << " reset insertion and radius: "
                    << radius_min << "," << radius_max << endl;
            new_loglh = optimize_parameters(tree_info,
                                            ML_SEARCH_PARAM_EPS,
                                            ML_SEARCH_PARAM_TOL,
                                            opt_per_param,
                                            new_loglh);
            LOG_DBG2 << "[" << model.get_name() << "] " << fixed << setprecision(5) << new_loglh << " thorough insertion [" << ML_SEARCH_PARAM_EPS
                    << "/" << ML_SEARCH_PARAM_TOL << "] " << endl;
          }
          else
          {
            radius_min += radius_step;
            radius_max += radius_step;
            LOG_DBG2 << "[" << model.get_name() << "] Update radius: " << radius_min << "," << radius_max << endl;
          }
        }
      } while (radius_min >= 0 && radius_min < radius_limit && fabs(old_loglh - new_loglh) > epsilon );

      LOG_DBG2 << "[" << model.get_name() << "] SPR done" << endl;

      free (cutoff_info);

      pll_tree = tree_info->root;
    }

    /* final thorough parameter optimization */
    model.set_loglh(new_loglh);

    LOG_DBG << "[" << model.get_name() << "] " << fixed << setprecision(5) << new_loglh << " final parameter optimization: " << endl;
    loglh = optimize_parameters(tree_info, epsilon, tolerance, opt_per_param, new_loglh);
    LOG_DBG << "[" << model.get_name() << "] " << fixed << setprecision(5) << loglh << " model done: [" << epsilon
            << "/" << tolerance << "] " << endl;

    if (!(optimize_topology || keep_model_parameters))
      pll_utree_graph_destroy(tree_info->root, NULL);
    pllmod_treeinfo_destroy(tree_info);

    return loglh;
  }

} /* namespace modeltest */

#undef JOB_WAIT
#undef JOB_UPDATE_MATRICES
#undef JOB_FULL_LK
#undef JOB_UPDATE_PARTIALS
#undef JOB_REDUCE_LK_EDGE
#undef JOB_FINALIZE
