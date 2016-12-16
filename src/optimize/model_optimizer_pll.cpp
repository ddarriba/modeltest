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

#include "utils.h"
#include "optimize/model_optimizer_pll.h"
#include "genesis/logging.h"

#include <cassert>
#include <iostream>

#define CHECK_LOCAL_CONVERGENCE 0

#define DO_THOROUGH_ML_SEARCH true
#define ML_SEARCH_PARAM_EPS   0.1
#define ML_SEARCH_PARAM_TOL   0.1

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

  /* for ascertainment bias correction, we have to set the new partition
     carefully. For example, applying the correction in the last section
     only */
  assert(!(partition->attributes & PLL_ATTRIB_AB_FLAG));

  sites = (partition->sites / count);
  offset = (partition->sites % count);
  start = sites * id + ((id<offset)?id:offset);
  sites += (id < offset)?1:0;

  new_partition = (pll_partition_t *) malloc(sizeof(pll_partition_t));
  new_partition->tips          = partition->tips;
  new_partition->clv_buffers   = partition->clv_buffers;
  new_partition->states        = partition->states;
  new_partition->sites         = sites;
  new_partition->rate_matrices = partition->rate_matrices;
  new_partition->prob_matrices = partition->prob_matrices;
  new_partition->rate_cats     = partition->rate_cats;
  new_partition->scale_buffers = partition->scale_buffers;
  new_partition->attributes    = partition->attributes;
  new_partition->tipmap       = partition->tipmap;

    /* vectorization options */
  new_partition->alignment     = partition->alignment;
  new_partition->states_padded = partition->states_padded;

  new_partition->clv      = (double **) modeltest::Utils::c_allocate (
                                      partition->tips + partition->clv_buffers,
                                      sizeof(double *));
  new_partition->tipchars = (unsigned char **) modeltest::Utils::c_allocate (
                                             partition->tips,
                                             sizeof(unsigned char *));
  if (!new_partition->clv)
  {
    dealloc_partition_local (new_partition);
    return PLL_FAILURE;
  }
  for (i = 0; i < new_partition->tips + new_partition->clv_buffers; ++i)
    new_partition->clv[i] = partition->clv[i]
        + (start * partition->states_padded * partition->rate_cats);

  for (i = 0; i < new_partition->tips; ++i)
      new_partition->tipchars[i] = partition->tipchars[i] + start;

    new_partition->scale_buffer =
                   (unsigned int **) modeltest::Utils::c_allocate (
                                                new_partition->scale_buffers,
                                                sizeof(unsigned int *));
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
                                      mt_size_t _n_cat_g,
                                      mt_index_t _thread_number)
    : ModelOptimizer(_msa, _model, _partition,
                     _optimize_topology, _thread_number),
      tree(_tree)
{
  thread_job = 0;
  global_loglh = 0;

  mt_size_t n_tips = tree.get_n_tips ();
  mt_size_t n_patterns = partition.get_n_patterns();

  pll_partition = model.build_partition(n_tips, n_patterns, _n_cat_g);

  if (!pll_partition)
  {
    LOG_ERR << "Error [PLL:" << pll_errno << "]: " << pll_errmsg << endl;
    assert(pll_partition);
  }

  pll_tree = tree.get_pll_tree(_thread_number);
  if (pllmod_utree_is_tip(pll_tree))
    pll_tree = pll_tree->back;

  model.set_n_categories(pll_partition->rate_cats);

  /* find sequences and link them with the corresponding taxa */
  for (mt_index_t i = 0; i < n_tips; ++i)
  {
      const char *c_seq = partition.get_sequence(i);
      const unsigned int *states_map =
        (model.get_datatype() == dt_dna)?
          (model.is_gap_aware()?extended_dna_map:pll_map_nt)
            :pll_map_aa;

      if (!pll_set_tip_states (pll_partition,
                               i,
                               states_map,
                               c_seq))
      {
          /* data type and tip states should be validated beforehand */
          LOG_ERR << "Error: Sequence does not match the datatype" << endl;
          assert(0);
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

  bool ModelOptimizerPll::build_parameters(pll_utree_t * pll_tree)
  {
    UNUSED(pll_tree);

    // TODO: Refactor in paramter_substrates and parameter_frequencies
    /* set initial model parameters */
    if (model.is_mixture())
    {
        assert (pll_partition->rate_cats == N_MIXTURE_CATS);
        for (mt_index_t i = 0; i < N_MIXTURE_CATS; i++)
        {
            pll_set_frequencies (pll_partition, i, model.get_mixture_frequencies(i));
            pll_set_subst_params (pll_partition, i, model.get_mixture_subst_rates(i));
        }
    }
    else
    {
       pll_set_subst_params (pll_partition, 0, model.get_subst_rates());
    }

    return true;
  }

  bool ModelOptimizerPll::run(double epsilon,
                              double tolerance,
                              mt_size_t _num_threads)
  {
    /*test*/
    //num_threads = 2;

#ifdef VERBOSE //TODO: Verbosity medium
    LOG_DBG << "[DBG] Optimizing model " << model.get_name() <<  " with " << _num_threads << " threads" << endl;
#endif
    time_t start_time = time(NULL);
    mt_size_t n_branches = tree.get_n_branches();

    pthread_t * threads = NULL;
    pll_partition_t ** partition_local = NULL;
    double * result_buf = NULL;
    pthread_barrier_t barrier_buf;

    if (!model.optimize_init(pll_partition,
                             NULL,
                             partition))
    {
      return false;
    }

#ifdef VERBOSE //TODO: Verbosity high
    LOG_DBG << "[DBG] Initial Frequencies:   ";
    for (mt_index_t i=0; i<pll_partition->states; i++)
      LOG_DBG << pll_partition->frequencies[0][i] << " ";
    LOG_DBG << endl;
#endif

    /* /PTHREADS */
    if (_num_threads > 1)
    {
      LOG_INFO << "Multithread optimization is temporary unavailable" << endl;
      return false;

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
        thread_data[i].vroot = tree.get_pll_tree();
        thread_data[i].barrier_buf = &barrier_buf;
        thread_data[i].trap = 1;
        thread_data[i].result_buf = result_buf;

        /* thread 0 is reserved for master process */
        if (i)
        {
            thread_wrap * w = new thread_wrap(&(thread_data[i]), this);
            if(pthread_create(&(threads[i]), NULL, thread_worker, w)) {
              LOG_ERR << "Error: Cannot create thread " << i << endl;
              return false;
            }
        }
      }
    }
    /* /PTHREADS */
    LOG_DBG << "[dbg] Building parameters and computing initial lk score" << endl;
    build_parameters(pll_tree);
    pllmod_utree_compute_lk(pll_partition, pll_tree, model.get_params_indices(), 1, 1);

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
      model.set_loglh(cur_loglh);
      model.set_exec_time(end_time - start_time);

      model.evaluate_criteria(n_branches, msa.get_n_sites());
      if (optimize_topology)
        model.set_tree(pll_tree);
      else
        model.set_tree((pll_utree_t *) tree.extract_tree(thread_number));

      return true;
    }
  }

  double ModelOptimizerPll::optimize_parameters( pll_utree_t * pll_tree,
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
    save_loglh = cur_loglh - 1;

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
                                          pll_tree,
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
          model.optimize(pll_partition, pll_tree, tolerance);
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
    double bl_min = 1e-6,
           bl_max = 1e2;

    int thorough_insertion = false;
    int radius_min = 1;
    int radius_max = 5;
    int ntopol_keep = 5;
    int spr_round_id = 0;
    int smoothings = 32;
    int radius_limit = 1;
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

    if (optimize_topology)
    {
      assert(!pll_tree->data);
      tree_info = pllmod_treeinfo_create(pll_utree_clone(pll_tree),
                                         tree.get_n_tips(),
                                         1,
                                         1);
      radius_limit = (tree_info->tip_count>25)?22:(tree_info->tip_count-3);

      pllmod_treeinfo_destroy_partition(tree_info, 0);
      pllmod_treeinfo_init_partition(tree_info,         /* treeinfo */
                                     0,                 /* partition index */
                                     pll_partition,     /* partition */
                                     0,                 /* params to optimize */
                                     1.0,               /* alpha */
                                     {0},               /* param_indices */
                                     0);                /* subst matrix symmetries */

      new_loglh = optimize_parameters(tree_info->root, 1.0, 1.0, opt_per_param, new_loglh);

      do
      {
        ++spr_round_id;
        old_loglh = new_loglh;

        new_loglh = pllmod_algo_spr_round(tree_info,
                                          radius_min,
                                          radius_max,
                                          ntopol_keep,
                                          thorough_insertion,
                                          bl_min,
                                          bl_max,
                                          smoothings,
                                          tolerance,
                                          NULL, /* cutoff */
                                          0);

        LOG_DBG << "[dbg] SPR cycle: " << old_loglh << " -> " << new_loglh << endl;

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
            new_loglh = optimize_parameters(tree_info->root,
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

      pll_tree = tree_info->root;
      pllmod_treeinfo_destroy(tree_info);
    }

    /* final thorough parameter optimization */
    model.set_loglh(new_loglh);

    tolerance = 1e-4;
    LOG_DBG << "[dbg] final parameter optimization: " << new_loglh << endl;
    loglh = optimize_parameters(pll_tree, epsilon, tolerance, opt_per_param, new_loglh);
    LOG_DBG << "[dbg] model done: [" << epsilon
            << "/" << tolerance << "]: " << loglh << endl;
    return loglh;
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
    // thread_data_t * local_thread_data;
    // unsigned int i;
    // unsigned int id, mat_count, mat_offset, mat_start;
    // unsigned int * local_matrix_indices;
    // double * local_branch_lengths;
    //
    // local_thread_data = (thread_data_t *) void_data;
    // id = (unsigned int) local_thread_data->thread_id;
    // mat_count  = (unsigned int) (local_thread_data->matrix_count / local_thread_data->num_threads);
    // mat_offset = (unsigned int) (local_thread_data->matrix_count % local_thread_data->num_threads);
    // mat_start = id * mat_count + ((id<mat_offset)?id:mat_offset);
    // mat_count += (id < mat_offset)?1:0;
    // local_matrix_indices = local_thread_data->matrix_indices + mat_start;
    // local_branch_lengths = local_thread_data->branch_lengths + mat_start;
    //
    // do
    // {
    //   /* wait */
    //   switch (thread_job)
    //     {
    //     case JOB_FINALIZE:
    //       {
    //         /* finalize */
    //         local_thread_data->trap = 0;
    //         break;
    //       }
    //     case JOB_UPDATE_MATRICES:
    //     {
    //       barrier (local_thread_data);
    //
    //         /* check eigen */
    //         if (!local_thread_data->partition->eigen_decomp_valid[0])
    //         {
    //           barrier (local_thread_data);
    //           if (!id)
    //             pll_update_eigen (local_thread_data->partition, 0);
    //           barrier (local_thread_data);
    //           if (!id)
    //             local_thread_data->partition->eigen_decomp_valid[0] = 1;
    //         }
    //
    //         barrier (local_thread_data);
    //
    //         //TODO: TAKE INDICES FROM model.get_params_indices()
    //         pll_update_prob_matrices (local_thread_data->partition,
    //                                   0,
    //                                   local_matrix_indices,
    //                                   local_branch_lengths,
    //                                   mat_count);
    //
    //         if (!id)
    //           thread_job = JOB_WAIT;
    //         barrier (local_thread_data);
    //         break;
    //       }
    //     case JOB_UPDATE_PARTIALS:
    //       {
    //         barrier (local_thread_data);
    //
    //         pll_update_partials (local_thread_data->partition,
    //                              local_thread_data->operations,
    //                              local_thread_data->ops_count);
    //         if (!id)
    //           thread_job = JOB_WAIT;
    //         barrier (local_thread_data);
    //         break;
    //       }
    //     case JOB_REDUCE_LK_EDGE:
    //       {
    //         if (!id)
    //           global_loglh = 0;
    //         barrier (local_thread_data);
    //
    //         local_thread_data->result_buf[id] =
    //             pll_compute_edge_loglikelihood (
    //               local_thread_data->partition,
    //               local_thread_data->vroot->clv_index,
    //               local_thread_data->vroot->scaler_index,
    //               local_thread_data->vroot->back->clv_index,
    //               local_thread_data->vroot->back->scaler_index,
    //               local_thread_data->vroot->pmatrix_index,
    //               model.get_params_indices(),
    //               NULL);
    //
    //         /* reduce */
    //         barrier (local_thread_data);
    //         if (!id)
    //           for (i=0; i<local_thread_data->num_threads; i++)
    //             global_loglh += local_thread_data->result_buf[i];
    //         barrier (local_thread_data);
    //
    //         if (!id)
    //           thread_job = JOB_WAIT;
    //         barrier (local_thread_data);
    //         break;
    //       }
    //     case JOB_FULL_LK:
    //       {
    //         /* execute */
    //         if (!id)
    //           global_loglh = 0;
    //         barrier (local_thread_data);
    //
    //         pll_update_partials (local_thread_data->partition, local_thread_data->operations,
    //                              local_thread_data->ops_count);
    //
    //         local_thread_data->result_buf[id] =
    //             pll_compute_edge_loglikelihood (
    //               local_thread_data->partition,
    //               local_thread_data->vroot->clv_index,
    //               local_thread_data->vroot->scaler_index,
    //               local_thread_data->vroot->back->clv_index,
    //               local_thread_data->vroot->back->scaler_index,
    //               local_thread_data->vroot->pmatrix_index,
    //               model.get_params_indices(),
    //               NULL);
    //
    //         /* reduce */
    //         barrier (local_thread_data);
    //         if (!id)
    //           for (i = 0; i < local_thread_data->num_threads; i++)
    //             global_loglh += local_thread_data->result_buf[i];
    //         barrier (local_thread_data);
    //
    //         if (!id)
    //           thread_job = JOB_WAIT;
    //         barrier (local_thread_data);
    //         break;
    //       }
    //     }
    // } while (local_thread_data->trap);

    return 0;
  }

} /* namespace modeltest */

#undef JOB_WAIT
#undef JOB_UPDATE_MATRICES
#undef JOB_FULL_LK
#undef JOB_UPDATE_PARTIALS
#undef JOB_REDUCE_LK_EDGE
#undef JOB_FINALIZE
