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
static bool keep_branch_lengths = false;

ModelOptimizer::~ModelOptimizer() {}

ModelOptimizerPll::ModelOptimizerPll (MsaPll &_msa,
                                      TreePll &_tree,
                                      Model &_model,
                                      Partition &_partition,
                                      bool _optimize_topology,
                                      mt_index_t _thread_number)
    : ModelOptimizer(_msa, _model, _partition,
                     _optimize_topology, _thread_number),
      tree(_tree)
{
  mt_size_t n_tips = tree.get_n_tips ();
  mt_size_t n_patterns = partition.get_n_patterns();

  pll_partition = model.build_partition(n_tips, n_patterns);

  if (!pll_partition)
  {
    mt_errno = MT_ERROR_LIBPLL;
    snprintf(mt_errmsg, ERR_MSG_SIZE, "[PLL %d] %s",
             pll_errno, pll_errmsg);
    throw EXCEPTION_INTERNAL_ERROR;
  }

  pll_tree = tree.get_pll_tree(_thread_number)->nodes[0]->back;

  // if (pllmod_utree_is_tip(pll_tree))
  //   pll_tree = pll_tree->back;

  /* find sequences and link them with the corresponding taxa */
  for (mt_index_t i = 0; i < n_tips; ++i)
  {
      const char *c_seq = partition.get_sequence(i);
      const pll_state_t * states_map =
        (model.get_datatype() == dt_dna)?
          (model.is_gap_aware()?extended_dna_map:pll_map_nt)
            :pll_map_aa;

      if (!pll_set_tip_states (pll_partition,
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
  const mt_size_t * weights = partition.get_weights();
  pll_set_pattern_weights(pll_partition, weights);
}

  ModelOptimizerPll::~ModelOptimizerPll ()
  {
      pll_partition_destroy(pll_partition);
  }

  bool ModelOptimizerPll::run(double epsilon,
                              double tolerance,
                              mt_size_t _num_threads)
  {
    /*test*/
    //num_threads = 2;

    LOG_DBG2 << "[dbg] Optimizing model " << model.get_name() <<  " with " << _num_threads << " threads" << endl;
    time_t start_time = time(NULL);
    mt_size_t n_branches = tree.get_n_branches();

    if (!model.optimize_init(pll_partition,
                             NULL,
                             partition))
    {
      return false;
    }

    LOG_DBG2 << "[dbg] Initial Frequencies:   ";
    for (mt_index_t i=0; i<pll_partition->states; i++)
      LOG_DBG2 << pll_partition->frequencies[0][i] << " ";
    LOG_DBG2 << endl;

    /* /PTHREADS */
    if (_num_threads > 1)
    {
      LOG_INFO << "Multithread optimization is temporary unavailable" << endl;
      assert(_num_threads == 1);
    }
    /* /PTHREADS */
    LOG_DBG << "[dbg] Building parameters and computing initial lk score" << endl;
    double initial_lk = pllmod_utree_compute_lk(pll_partition, pll_tree, model.get_params_indices(), 1, 1);
    LOG_DBG << "[dbg] Initial score: " << initial_lk << endl;

#if(CHECK_LOCAL_CONVERGENCE)
    double test_loglh;         /* temporary variable */
    mt_size_t converged = 0;  /* bitvector for parameter convergence */
#endif

    double cur_loglh = optimize_model(epsilon, tolerance, true);

    LOG_DBG << "[dbg] Model optimization done: " << cur_loglh << endl;

    /* TODO: if bl are reoptimized */
    if (keep_branch_lengths)
      tree.set_bl_optimized();

    time_t end_time = time(NULL);

    if (_num_threads > 1)
    {
      assert(false);
    }

    if (interrupt_optimization)
    {
        return false;
    }
    else
    {
      optimized = true;
      model.set_loglh(cur_loglh);
      model.set_exec_time(end_time - start_time);

      model.evaluate_criteria(n_branches, msa.get_n_sites());

      if (optimize_topology)
        model.set_tree(pll_tree);

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

    if (start_loglh)
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
          all_params_done = model.optimize_oneparameter(pll_partition,
                                          tree_info,
                                          tolerance);
          cur_loglh = model.get_loglh();

          /* ensure we never get a worse likelihood score */
          if (save_loglh - cur_loglh > 1e-5)
          {
              LOG_ERR << "Error: " << fixed << setprecision(5) << save_loglh << " vs " <<
                      setprecision(5) << cur_loglh <<
                      " [" << cur_parameter << "]" << endl;
              assert(cur_loglh - save_loglh < 1e-5);
          }
          opt_delta = cur_loglh;
          notify();
        }
      }
    }
    else
    {
      while ((!interrupt_optimization) &&
        (fabs (cur_loglh - save_loglh) > epsilon && cur_loglh < save_loglh))
      {
          save_loglh = cur_loglh;
          model.optimize(pll_partition, tree_info, tolerance);
          opt_delta = cur_loglh;
          notify();
          cur_loglh = model.get_loglh();
      }
    }

    return cur_loglh;
  }

  const int radius_step = 5;

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

    LOG_DBG << "[dbg] Initial log likelihood: " << loglh << endl;

    tree_info = pllmod_treeinfo_create(pll_utree_graph_clone(pll_tree),
                                       tree.get_n_tips(),
                                       1,
                                       1);

     int retval = pllmod_treeinfo_init_partition(tree_info,
                                                 0,
                                                 pll_partition,
                                                 0xFFFF,
                                                 PLL_GAMMA_RATES_MEAN,
                                                 model.get_alpha(),
                                                 model.get_params_indices(),
                                                 model.get_symmetries());

     if (!retval)
     {
       assert(pll_errno);
       assert(0);
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

        LOG_DBG << "[dbg] SPR params:"
            << " round=" << spr_round_id
            << " rad=[" << radius_min << "," << radius_max << "]"
            << " bl=[" << MT_MIN_BRANCH_LENGTH << "," << MT_MAX_BRANCH_LENGTH << "]"
            << " ntopos=" << ntopol_keep
            << " smooth=" << smoothings
            << " tol=" << tolerance
            << endl;

        new_loglh = pllmod_algo_spr_round(tree_info,
                                          radius_min,
                                          radius_max,
                                          ntopol_keep,
                                          thorough_insertion,
                                          PLLMOD_OPT_BLO_NEWTON_FAST,
                                          MT_MIN_BRANCH_LENGTH,
                                          MT_MAX_BRANCH_LENGTH,
                                          smoothings,
                                          tolerance,
                                          cutoff_info, /* cutoff */
                                          cutoff_thr);

        LOG_DBG << "[dbg] SPR cycle: " << old_loglh << " -> " << new_loglh << endl;

        new_loglh = optimize_parameters(tree_info, 1.0, 1.0, opt_per_param, new_loglh);

        bool impr = (new_loglh - old_loglh > epsilon);
        if (impr && thorough_insertion)
        {
          radius_min = 1;
          radius_max = radius_step;
          LOG_DBG << "[dbg] Reset radius: " << radius_min << "," << radius_max << endl;
        }
        else if (!impr && DO_THOROUGH_ML_SEARCH)
        {
          if (!thorough_insertion)
          {
            thorough_insertion = true;
            spr_round_id = 0;
            radius_min = 1;
            radius_max = radius_step;

            LOG_DBG << "[dbg] Reset insertion and radius: "
                    << radius_min << "," << radius_max << ": " << new_loglh << endl;
            new_loglh = optimize_parameters(tree_info,
                                            ML_SEARCH_PARAM_EPS,
                                            ML_SEARCH_PARAM_TOL,
                                            opt_per_param,
                                            new_loglh);
            LOG_DBG << "[dbg] Thorough insertion [" << ML_SEARCH_PARAM_EPS
                    << "/" << ML_SEARCH_PARAM_TOL << "]: " << new_loglh << endl;
          }
          else
          {
            radius_min += radius_step;
            radius_max += radius_step;
            LOG_DBG << "[dbg] Update radius: " << radius_min << "," << radius_max << endl;
          }
        }
      } while (radius_min >= 0 && radius_min < radius_limit && fabs(old_loglh - new_loglh) > epsilon );

      LOG_DBG << "[dbg] SPR done" << endl;

      free (cutoff_info);

      pll_tree = tree_info->root;
    }

    /* final thorough parameter optimization */
    model.set_loglh(new_loglh);

    LOG_DBG << "[dbg] final parameter optimization: " << new_loglh << endl;
    loglh = optimize_parameters(tree_info, epsilon, tolerance, opt_per_param, new_loglh);
    LOG_DBG << "[dbg] model done: [" << epsilon
            << "/" << tolerance << "]: " << loglh << endl;

    if (!optimize_topology)
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
