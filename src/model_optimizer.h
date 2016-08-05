/*
 * model_optimizer.h
 *
 *  Created on: Jun 10, 2015
 *      Author: diego
 */

#ifndef MODEL_OPTIMIZER_H_
#define MODEL_OPTIMIZER_H_

#include "model/model.h"
#include "msa.h"
#include "partition.h"
#include "thread/observer.h"

namespace modeltest
{

  typedef enum {
      mt_param_branch_lengths        = 1<<0,
      mt_param_subst_rates           = 1<<1,
      mt_param_frequencies           = 1<<2,
      mt_param_alpha                 = 1<<3,
      mt_param_pinv                  = 1<<4,
      mt_param_mixture_rates_weights = 1<<5
  } mt_parameter_t;

  extern bool on_run;

  class ModelOptimizer : public Observable
  {
  public:
    ModelOptimizer (Msa & _msa,
                    Model & _model,
                    Partition & _partition,
                    mt_index_t _thread_number = 0)
        : msa(_msa),
          model(_model),
          partition(_partition),
          thread_number(_thread_number)
    {
        interrupt_optimization = false;
        optimized = false;
    }
    virtual ~ModelOptimizer ();

    /**
     * @brief Optimizes all parameters for the model
     * @param[in] epsilon     the tolerance of the global optimization
     * @param[in] tolerance   tolerance for parameter optimization
     * @param[in] num_threads number of threads for model optimization
     * @return true, if the optimization is OK
     */
    virtual bool run(double epsilon   = DEFAULT_OPT_EPSILON,
                     double tolerance = DEFAULT_PARAM_EPSILON,
                     mt_size_t num_threads = 1) = 0;

    /**
     * @brief Gets the optimization status of the model
     * @return true, if the model is optimized
     */
    int is_optimized() const { return optimized; }

    mt_index_t get_thread_number() const { return thread_number; }
    mt_index_t get_cur_parameter() const { return cur_parameter; }
    double get_opt_delta() const { return opt_delta; }
    void interrupt() { interrupt_optimization = true; }

  protected:
    bool optimized; //! optimization state
    Msa & msa;       //! the multiple sequence alignment
    Model & model;   //! the model to optimize
    Partition & partition;
    mt_size_t n_sites;    //! original number of sites
    mt_size_t n_patterns; //! crunched number of sites

    mt_index_t thread_number;  //! the number of the current thread

    // dynamic optimization status
    bool interrupt_optimization;
    mt_index_t cur_parameter;
    double opt_delta;
  };

} /* namespace modeltest */

#endif /* MODEL_OPTIMIZER_H_ */
