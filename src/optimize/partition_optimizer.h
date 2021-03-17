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

#ifndef PARTITION_OPTIMIZER_H_
#define PARTITION_OPTIMIZER_H_

#include "../msapll.h"
#include "../plldefs.h"
#include "../treepll.h"
#include "../partition.h"
#include "../thread/observer.h"
#include "../thread/threadpool.h"

namespace modeltest
{

  typedef enum
  {
    partition_optimize_all,
    partition_optimize_greedy
  } part_opt_t;

  typedef struct
  {
    Model * model;

    mt_index_t model_index;
    mt_size_t n_models;

    time_t start_time;
    time_t end_time;
  } opt_info_t;

  class PartitionOptimizer : public Observable
  {
  public:
    PartitionOptimizer(Partition &partition,
                       MsaPll & msa,
                       TreePll & tree,
                       part_opt_t opt_type,
                       bool optimize_topology,
                       bool keep_model_parameters,
                       int gamma_rates,
                       double epsilon_param,
                       double epsilon_opt);
    ~PartitionOptimizer();
    bool evaluate( mt_size_t n_procs = 1 );
  private:
    bool evaluate_greedy( mt_size_t n_procs );

    bool evaluate_all_models( std::vector<Model *> const& models,
                              mt_size_t n_procs );

    bool evaluate_single_model(Model & model,
                               mt_index_t thread_number);

    void * model_scheduler( std::vector<Model *> const& models );

    Partition & partition;  //! partition instance
    MsaPll & msa;           //! msa instance
    TreePll & tree;         //! tree instance
    part_opt_t opt_type;
    bool optimize_topology;
    bool keep_model_parameters;
    int gamma_rates;        //! Gamma rates mode (mean/median)
    double epsilon_param;
    double epsilon_opt;
  };
}
#endif
