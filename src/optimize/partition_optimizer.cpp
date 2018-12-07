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

#include "partition_optimizer.h"
#include "model_optimizer_pll.h"
#include "../genesis/logging.h"

#include <fstream>

using namespace std;

namespace modeltest
{

  PartitionOptimizer::PartitionOptimizer(Partition &partition,
                                         MsaPll & msa,
                                         TreePll & tree,
                                         part_opt_t opt_type,
                                         bool optimize_topology,
                                         bool keep_model_parameters,
                                         int gamma_rates,
                                         double epsilon_param,
                                         double epsilon_opt) :
    partition(partition),
    msa(msa),
    tree(tree),
    opt_type(opt_type),
    optimize_topology(optimize_topology),
    keep_model_parameters(keep_model_parameters),
    gamma_rates(gamma_rates),
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

    MT_INFO << "Step 1/6" << endl;
    /* 1. optimize starting models */
    if (!evaluate_all_models( candidate_models, n_procs ))
      return false;

    for (int k=N_DNA_SUBST_RATES-1; k>0; --k)
    {
      MT_INFO << "Step " << N_DNA_SUBST_RATES - k + 1 << "/6" << endl;
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

      if (!evaluate_all_models( candidate_models, n_procs ))
        return false;
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

  #if(MPI_ENABLED)
  typedef struct mpi_comm_data
  {
    int model_index;
    time_t t_start;
    time_t t_end;
    double alpha;
    double pinv;
    double loglh;
    union {
        struct {
          double rates[N_DNA_SUBST_RATES];
          double freqs[N_DNA_STATES];
        } dna_data;
        //TODO: PROTGTR Rates
    };
    double rate_categories[MT_MAX_CATEGORIES];
    double rate_weights[MT_MAX_CATEGORIES];
  } mpi_comm_data_t;

  void * PartitionOptimizer::model_scheduler( vector<Model *> const& models ) {
    mt_size_t n_jobs = models.size();
    vector<int> job_map;
    MPI_Request * request = new MPI_Request[mpi_numprocs];
    MPI_Status * status = new MPI_Status[mpi_numprocs];
    mpi_comm_data_t rec_data;

    job_map.resize(n_jobs);
    for (mt_index_t i=1 ; i<n_jobs; ++i) job_map[i] = -1;

    /* schedule jobs sequentially */
    int opt_index = 0;
    for (mt_index_t i=0 ; i<(n_jobs + mpi_numprocs - 1); ++i)
    {
      int next_index = (i<n_jobs)?i:-1;

      /* receive request notification from processes
       * either the previous optimized model or an empty block
       */
      MPI_Recv(&rec_data, sizeof(mpi_comm_data_t), MPI_BYTE, MPI_ANY_SOURCE, 201, master_mpi_comm, &status[0]);

      if (rec_data.model_index >= 0)
      {
        /* update local model information */
        pll_unode_t * serialized_tree;
        pll_utree_t * expanded_tree;

        /* update model */
        Model *model = models[rec_data.model_index];
        if (model->is_G()) model->set_alpha(rec_data.alpha);
        if (model->is_I()) model->set_prop_inv(rec_data.pinv);
        if (model->is_R())
        {
          model->set_mixture_rates(rec_data.rate_categories);
          model->set_mixture_weights(rec_data.rate_weights);
        }
        model->set_loglh(rec_data.loglh);

        if (optimize_topology)
        {
          serialized_tree = (pll_unode_t *) malloc((2*tree.get_n_tips() - 2) * sizeof(pll_unode_t));

          /* receive serialized tree */
          MPI_Recv(serialized_tree, sizeof(pll_unode_t) * (2*tree.get_n_tips() - 2), MPI_BYTE,MPI_ANY_SOURCE, 301, master_mpi_comm, &status[0]);

          expanded_tree = pllmod_utree_expand(serialized_tree, tree.get_n_tips());
          tree.update_names(expanded_tree);
          model->set_tree(expanded_tree);

          free(serialized_tree);
        }

        if (partition.get_datatype() == dt_dna)
        {
          model->set_frequencies(rec_data.dna_data.freqs);
          model->set_subst_rates(rec_data.dna_data.rates);
        }
        else
        {
          if (model->is_F())
          {
            model->set_frequencies(&partition.get_empirical_frequencies()[0]);
          }
        }

        model->evaluate_criteria(tree.get_n_branches(), msa.get_n_sites());

        opt_info_t exec_info;
        exec_info.n_models = n_jobs;
        exec_info.model_index = ++opt_index;
        exec_info.start_time = rec_data.t_start;
        exec_info.model = model;
        exec_info.end_time = rec_data.t_end;

        /* notify to observers the updated model */
        notify((void *) &exec_info);
      }

      /* send reply back to sender with the next model index */
      MPI_Isend(&next_index, 1, MPI_INT, status[0].MPI_SOURCE, 202,
                master_mpi_comm, &request[status[0].MPI_SOURCE]);
      if (next_index >= 0)
        job_map[i] = status[0].MPI_SOURCE;
    }

    delete[] request;
    delete[] status;

    return 0;
  }
  #else
  void * PartitionOptimizer::model_scheduler( vector<Model *> const& models ) {
    UNUSED(models);
    assert(0);
    return 0;
  }
  #endif

  bool PartitionOptimizer::evaluate_all_models( vector<Model *> const& models,
                                                mt_size_t n_procs )
  {
    bool exec_ok = true;
    mt_size_t n_models = models.size();

    BARRIER;
#if(MPI_ENABLED)
    mpi_comm_data_t snd_data;
#endif

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

            int res = evaluate_single_model(*model, thread_map[std::this_thread::get_id()]);

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
#if(MPI_ENABLED)
  int next_model = 0;
  if (ROOT)
  {
    model_scheduler(models);
    next_model = -1;
  }

  pll_unode_t * serialized_tree = 0;
  snd_data.model_index = -1;
  while (next_model != -1)
  {
    /* request model */
    MPI_Status status;
    snd_data.t_start = exec_info.start_time;
    snd_data.t_end = exec_info.end_time;
    MPI_Send(&snd_data, sizeof(mpi_comm_data_t), MPI_BYTE, 0, 201, master_mpi_comm);

    if (optimize_topology && snd_data.model_index >= 0)
    {
      assert(serialized_tree);
      MPI_Send(serialized_tree, sizeof(pll_unode_t) * (2*tree.get_n_tips() - 2), MPI_BYTE, 0, 301, master_mpi_comm);
      free(serialized_tree);
    }
    MPI_Recv(&next_model, 1, MPI_INT, 0, 202, master_mpi_comm, &status);

    if (next_model < 0) break;
    exec_info.model_index = next_model;

    Model *model = models[exec_info.model_index];
#else
      for (Model *model : models)
      {
        ++exec_info.model_index;
#endif
        exec_info.start_time = time(NULL);
        exec_ok &= evaluate_single_model(*model, 0);
        if (mt_errno)
          break;

        exec_info.model = model;

        exec_info.end_time = time(NULL);
        notify((void *) &exec_info);

#if(MPI_ENABLED)
        snd_data.model_index = next_model;
        snd_data.loglh = model->get_loglh();

        if (optimize_topology)
          serialized_tree = pllmod_utree_serialize(model->get_tree()->nodes[0]->back,
                                                   tree.get_n_tips());

        if (model->is_G()) snd_data.alpha = model->get_alpha();
        if (model->is_I()) snd_data.pinv = model->get_prop_inv();
        if (model->is_R())
        {
          memcpy(snd_data.rate_categories, model->get_mixture_rates(), model->get_n_categories() * sizeof(double));
          memcpy(snd_data.rate_weights, model->get_mixture_weights(), model->get_n_categories() * sizeof(double));
        }
        if (partition.get_datatype() == dt_dna)
        {
          memcpy(snd_data.dna_data.freqs, model->get_frequencies(), N_DNA_STATES * sizeof(double));
          memcpy(snd_data.dna_data.rates, model->get_subst_rates(), N_DNA_SUBST_RATES * sizeof(double));
        }
#endif
      }
    }

BARRIER;

    return exec_ok;
  }

  bool PartitionOptimizer::evaluate_single_model(Model & model,
                                                 mt_index_t thread_number)
  {
    bool result;
    mt_errno = 0;

    if (model.is_optimized())
    {
      result = true;
    }
    else
    {
      try
      {
        ModelOptimizer * mopt = new ModelOptimizerPll(msa, tree, model,
                                                      partition,
                                                      optimize_topology,
                                                      keep_model_parameters,
                                                      gamma_rates,
                                                      thread_number);
        assert(mopt);

        result = mopt->run(epsilon_opt, epsilon_param);

        delete mopt;
      }
      catch (int e)
      {
        assert(mt_errno);
        result = false;
      }
    }

    return result;
  }
}
