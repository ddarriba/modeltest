/*
 * model_optimizer.h
 *
 *  Created on: Jun 10, 2015
 *      Author: diego
 */

#ifndef MODEL_OPTIMIZER_H_
#define MODEL_OPTIMIZER_H_

#define UNUSED(expr) do { (void)(expr); } while (0)

#include "model.h"

namespace modeltest
{

  class ModelOptimizer
  {
  public:
    ModelOptimizer (Model *model) : model(model)
    {
        optimized = false;
    }
    virtual ~ModelOptimizer ()
    {
    }

    /**
     * @brief Optimizes one single parameter
     * @param[in] which_parameter   parameter to optimize
     * @param[in] tolerance         tolerance for parameter optimization
     * @return the resulting log-Likelihood of the model
     */
    virtual double opt_single_parameter(int which_parameter,
                                        double tolerance = 0.0001) = 0;

    /**
     * @brief Optimizes all parameters for the model
     * @param[in] epsilon    the tolerance of the global optimization
     * @param[in] tolerance  tolerance for parameter optimization
     * @return true, if the optimization is OK
     */
    virtual bool run(double epsilon   = 0.01,
                     double tolerance = 0.0001) = 0;

    /**
     * @brief Gets the optimization status of the model
     * @return true, if the model is optimized
     */
    int is_optimized() const { return optimized; }

  protected:
    bool optimized; //! optimization state
    Model *model;   //! the model to optimize
  };

} /* namespace modeltest */

#endif /* MODEL_OPTIMIZER_H_ */
