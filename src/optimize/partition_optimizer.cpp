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

  bool PartitionOptimizer::evaluate(mt_size_t n_procs)
  {
    bool exec_ok = false;

    switch (opt_type)
    {
      case partition_optimize_all:
        exec_ok = evaluate_all_models( partition.get_models(), n_procs );
        break;
      case partition_optimize_greedy:
        exec_ok = evaluate_greedy( n_procs );
        break;
    }
    return exec_ok;
  }

  bool PartitionOptimizer::evaluate_greedy( mt_size_t n_procs )
  {
    //opt_info_t exec_info;
    bool exec_ok = true;
    if (partition.get_datatype() != dt_dna)
      return false;

    vector<Model *> candidate_models = partition.get_models();

    cout << "Step 1/6" << endl;
    /* 1. optimize starting models */
    evaluate_all_models( candidate_models, n_procs );

    for (int k=N_DNA_SUBST_RATES-1; k>0; --k)
    {
      cout << "Step " << N_DNA_SUBST_RATES - k + 1 << "/6" << endl;
      /* 2. select best-fit model */
      //TODO: So far, choose first
      Model * best_model = candidate_models[0];
      for (Model * model : candidate_models)
      {
        if (model->get_bic() < best_model->get_bic())
          best_model = model;
      }

      /* 3. merge rates */
      candidate_models = partition.update_model_set(*static_cast<DnaModel *>(best_model));

      evaluate_all_models( candidate_models, n_procs );
    }
    // exec_info.start_time = time(NULL);
    // exec_info.n_models = 0;
    // start_model_params = MOD_PARAM_ESTIMATED_FREQ | MOD_PARAM_GAMMA;
    // start_model = partition.get_model_by_matrix(DNA_GTR_INDEX, start_model_params);
    // exec_ok &= evaluate_single_model(*start_model, 0);
    // exec_info.model_index = 0;
    // exec_info.model = start_model;
    // exec_info.end_time = time(NULL);
    // notify((void *) &exec_info);

    // update_model_set(*static_cast<DnaModel *>(c_models[0]));

    return exec_ok;
  }

  bool PartitionOptimizer::evaluate_all_models( vector<Model *> const& models,
                                                mt_size_t n_procs )
  {
    bool exec_ok = true;
    mt_size_t n_models = models.size();

    if (n_procs > 1)
    {
      /* execute on thread pool */
      ThreadPool pool(n_procs);
      vector< future<int> > results;
      map<thread::id, mt_index_t> thread_map = pool.worker_ids;

      mt_index_t cur_model = 0;
      for (Model *model : models)
      {
        ++cur_model;

        results.emplace_back(
          pool.enqueue([cur_model, model, n_models,
                         this, &thread_map] {
            opt_info_t exec_info;
            exec_info.start_time = time(NULL);
            exec_info.n_models = n_models;
            thread::id my_id(__gthread_self());
            int res = evaluate_single_model(*model, thread_map[my_id]);

            exec_info.model_index = cur_model + 1;
            exec_info.model = model;

            exec_info.end_time = time(NULL);
            notify((void *) &exec_info);
            return res;
          })
        );
      }

      for (future<int> & f : results)
      {
        f.wait();
        exec_ok &= f.get();
      }
    }
    else
    {
      /* sequential execution */
      opt_info_t exec_info;
      exec_info.n_models = n_models;
      exec_info.model_index = 0;
      for (Model *model : models)
      {
        exec_info.start_time = time(NULL);

        exec_ok &= evaluate_single_model(*model, 0);
        ++exec_info.model_index;
        exec_info.model = model;

        exec_info.end_time = time(NULL);
        notify((void *) &exec_info);
      }
    }

    return exec_ok;
  }

  bool PartitionOptimizer::evaluate_single_model(Model & model,
                                                 mt_index_t thread_number)
  {
    bool result;

    ModelOptimizer * mopt = new ModelOptimizerPll(msa, tree, model,
                                                  partition,
                                                  optimize_topology,
                                                  n_categories,
                                                  thread_number);
    assert(mopt);
    result = mopt->run(epsilon_param, epsilon_opt);

    delete mopt;

    return result;
  }
}
