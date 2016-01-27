/*
 * model_optimizer_pll.h
 *
 *  Created on: Jun 10, 2015
 *      Author: diego
 */

#ifndef MODEL_OPTIMIZER_PLL_H_
#define MODEL_OPTIMIZER_PLL_H_

#include "model_optimizer.h"
#include "plldefs.h"
#include "treepll.h"
#include "msapll.h"

#include <pthread.h>
#include <vector>

typedef struct
{
    long thread_id;
    long num_threads;
    pll_partition_t * partition;
    unsigned int * matrix_indices;
    unsigned int matrix_count;
    pll_operation_t * operations;
    unsigned int ops_count;
    double * branch_lengths;
    pll_utree_t * vroot;
    pthread_barrier_t * barrier_buf;
    double * result_buf;
    int trap;
} thread_data_t;

namespace modeltest
{

  class ModelOptimizerPll : public ModelOptimizer
  {
  public:
    ModelOptimizerPll (MsaPll *_msa,
                       TreePll *_tree,
                       Model *_model,
                       const partition_t & partition,
                       mt_size_t _n_cat_g = DEFAULT_GAMMA_RATE_CATS,
                       mt_index_t _thread_number = 0);
    virtual ~ModelOptimizerPll ();

    virtual double opt_single_parameter(mt_index_t which_parameter,
                                        double tolerance = DEFAULT_PARAM_EPSILON,
                                        bool first_guess = false);

    virtual bool run(double epsilon   = 0.01,
                     double tolerance = 0.0001,
                     mt_size_t num_threads = 1);

    /* pthreads */
    void * worker(void * void_data);

  private:

    bool build_parameters();
    MsaPll *msa;     //! the MSA instance
    TreePll *tree;   //! the tree instance

    pll_optimize_options_t * params; //! optimization parameters
    pll_partition_t * pll_partition; //! partition
    pll_operation_t * operations;    //! array of operation for CLVs
    mt_size_t ops_count;

    double * branch_lengths;    //! array of branch lengths
    mt_index_t * matrix_indices;       //! array of matrix indices
    mt_size_t matrix_count;

    /* pthreads */
    void start_job_sync(int JOB, thread_data_t * td);

    //TODO: num_threads is the argument for run(...)
    //      thread_data should be a vector, and use "length()" instead of reading
    //      num_threads from here!
    mt_size_t num_threads;
    thread_data_t * thread_data = NULL;
    volatile int thread_job;
    volatile double global_lnl;
  };

} /* namespace modeltest */

#endif /* MODEL_OPTIMIZER_PLL_H_ */
