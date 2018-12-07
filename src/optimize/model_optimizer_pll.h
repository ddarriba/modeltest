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

#ifndef MODEL_OPTIMIZER_PLL_H_
#define MODEL_OPTIMIZER_PLL_H_

#include "model_optimizer.h"
#include "../global_defs.h"
#include "../msapll.h"
#include "../plldefs.h"
#include "../treepll.h"

#include <vector>

namespace modeltest
{

  class ModelOptimizerPll : public ModelOptimizer
  {
  public:
    ModelOptimizerPll (MsaPll & _msa,
                       TreePll & _tree,
                       Model & _model,
                       Partition &partition,
                       bool _optimize_topology,
                       bool _keep_model_parameters,
                       int gamma_rates = PLL_GAMMA_RATES_MEAN,
                       mt_index_t _thread_number = 0);
    virtual ~ModelOptimizerPll ();

    virtual bool run(double epsilon   = 0.01,
                     double tolerance = 0.0001,
                     mt_size_t num_threads = 1);

    /* pthreads */
    void * worker(void * void_data);

  private:

    double optimize_model( double epsilon,
                           double tolerance,
                           bool opt_per_param );

    double optimize_parameters( pllmod_treeinfo_t * pll_tree,
                                double epsilon,
                                double tolerance,
                                bool opt_per_param,
                                double start_loglh );

    TreePll & tree;   //! the tree instance

    pll_partition_t * pll_partition; //! partition
    pll_unode_t * pll_tree;

  };

} /* namespace modeltest */

#endif /* MODEL_OPTIMIZER_PLL_H_ */
