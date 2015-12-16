/*
 * model_optimizer.h
 *
 *  Created on: Jun 10, 2015
 *      Author: diego
 */

#ifndef MODEL_OPTIMIZER_H_
#define MODEL_OPTIMIZER_H_

#include "model.h"

namespace modeltest
{

  extern bool on_run;

  class ModelOptimizer
  {
  public:
    ModelOptimizer (Model *_model,
                    const partition_t & _partition)
        : model(_model),
          partition(_partition)
    {
        optimized = false;
    }
    virtual ~ModelOptimizer ();

    /**
     * @brief Optimizes one single parameter
     * @param[in] which_parameter   parameter to optimize
     * @param[in] tolerance         tolerance for parameter optimization
     * @return the resulting log-Likelihood of the model
     */
    virtual double opt_single_parameter(mt_index_t which_parameter,
                                        double tolerance = DEFAULT_PARAM_EPSILON) = 0;

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

  protected:
    bool optimized; //! optimization state
    Model *model;   //! the model to optimize
    const partition_t partition;
  };

} /* namespace modeltest */

#endif /* MODEL_OPTIMIZER_H_ */
