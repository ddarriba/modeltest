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

#include <vector>

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
                                        double tolerance
                                               = DEFAULT_PARAM_EPSILON);

    virtual bool run(double epsilon   = 0.01,
                     double tolerance = 0.0001);

  private:

    bool build_parameters();
    MsaPll *msa;     //! the MSA instance
    TreePll *tree;   //! the tree instance

    pll_optimize_options_t * params; //! optimization parameters
    pll_partition_t * pll_partition; //! partition
    pll_operation_t * operations;    //! array of operation for CLVs

    double * branch_lengths;    //! array of branch lengths
    mt_index_t * matrix_indices;       //! array of matrix indices

    mt_index_t thread_number;  //! the number of the current thread
  };

} /* namespace modeltest */

#endif /* MODEL_OPTIMIZER_PLL_H_ */
