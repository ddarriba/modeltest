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

#include "optimize/partition_optimizer.h"
#include "optimize/model_optimizer_pll.h"

using namespace std;

namespace modeltest
{

  PartitionOptimizer::PartitionOptimizer(Partition &partition,
                                         MsaPll & msa,
                                         TreePll & tree,
                                         mt_size_t n_categories,
                                         part_opt_t opt_type,
                                         bool optimize_topology,
                                         double epsilon_param,
                                         double epsilon_opt) :
    partition(partition),
    msa(msa),
    tree(tree),
    n_categories(n_categories),
    opt_type(opt_type),
    optimize_topology(optimize_topology),
    epsilon_param(epsilon_param),
    epsilon_opt(epsilon_opt)
  {

  }

  PartitionOptimizer::~PartitionOptimizer()
  {

  }

  bool PartitionOptimizer::evaluate()
  {
    mt_size_t n_models = partition.get_number_of_models();
    bool exec_ok = true;
    for (mt_index_t cur_model=0; cur_model < n_models; cur_model++)
    {
        Model *model = partition.get_model(cur_model);
        exec_ok &= evaluate_single_model(*model, 0);
        notify((void *) model);
    }

    return exec_ok;
  }

  bool PartitionOptimizer::evaluate_single_model(Model & model,
                                                 mt_index_t thread_number)
  {
      ModelOptimizer * mopt = new ModelOptimizerPll(msa, tree, model,
                                                    partition,
                                                    optimize_topology,
                                                    n_categories,
                                                    thread_number);
      assert(mopt);

      bool result = mopt->run(epsilon_param, epsilon_opt);
      delete mopt;

      return result;
  }
}
