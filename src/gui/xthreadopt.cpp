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

#include "../gui/xthreadopt.h"
#include "../service/modeltestservice.h"

#include <map>
#include <vector>
#include <iomanip>
#include <iostream>
#include <algorithm>

//using namespace modeltest;
using namespace std;

xThreadOpt::xThreadOpt(partition_id_t & part_id,
                       int n_threads,
                       double epsilon_param,
                       double epsilon_opt):
    scheme(ModelTestService::instance()->get_partitioning_scheme()), n_threads(n_threads),
    epsilon_param(epsilon_param), epsilon_opt(epsilon_opt)
{
    pool = 0;
    interrupt = false;
    assert(ModelTestService::initialized());
    n_models = ModelTestService::instance()->get_number_of_models(part_id);
}

xThreadOpt::xThreadOpt(modeltest::PartitioningScheme & scheme,
           int n_threads,
           double epsilon_param,
           double epsilon_opt) :
    scheme(scheme), n_threads(n_threads),
    epsilon_param(epsilon_param), epsilon_opt(epsilon_opt)
{
    n_models = 0;
    pool = 0;
    interrupt = false;
    assert(ModelTestService::initialized());

    for (mt_index_t i=0; i<scheme.get_size(); ++i)
    {
        n_models += scheme.get_partition(i).get_number_of_models();
    }
}

void xThreadOpt::update(Observable * subject, void * data)
{
    UNUSED(data);
    modeltest::ModelOptimizer * mopt = static_cast<modeltest::ModelOptimizer *>(subject);

    //emit next_model( model, thread_map[my_id] );
    if (interrupt)
    {
        mopt->interrupt();
    }

    emit next_parameter( mopt->get_cur_parameter(), mopt->get_opt_delta(), mopt->get_threadgroup_id() );
}

void xThreadOpt::optimize_single(const partition_id_t &part_id,
                            mt_index_t n_models,
                            modeltest::Model *model,
                            mt_index_t thread_id,
                            double epsilon_param,
                            double epsilon_opt)
{
    // check before optimization
    if (interrupt)
        return;

    if (!model->is_optimized())
    {
      ModelTestService::instance()->optimize_single(part_id,
                                                    model,
                                                    1,
                                                    thread_id,
                                                    epsilon_param,
                                                    epsilon_opt,
                                                    {this});
    }
    // check for models interrupted during optimization
    if (interrupt)
        return;

    emit optimized_single_done(model, n_models);
}

mt_size_t xThreadOpt::get_number_of_models( void ) const
{
    return n_models;
}

void xThreadOpt::run()
{
    mt_index_t cur_model;
    assert(n_models);

    for (mt_index_t cur_part=0; cur_part<scheme.get_size(); ++cur_part)
    {
        modeltest::Partition & partition = scheme.get_partition(cur_part);
        if (n_threads == 1)
        {
            for (cur_model=0; cur_model < partition.get_number_of_models(); cur_model++)
            {
                modeltest::Model *model = partition.get_model(cur_model); //ModelTestService::instance()->get_model(part_id, cur_model);
                emit next_model( model, 0 );
                optimize_single(partition.get_id(), n_models, model, 0, epsilon_param, epsilon_opt);
            }
        }
        else
        {
            pool = new modeltest::ThreadPool(n_threads);
            vector< future<void> > results;

            //cout << "Starting jobs... (output might be unsorted)" << endl;
            for (cur_model=0; cur_model < partition.get_number_of_models(); cur_model++)
            {
                modeltest::Model *model = partition.get_model(cur_model); //ModelTestService::instance()->get_model(part_id, cur_model);

                double eps_par = epsilon_param;
                double eps_opt = epsilon_opt;
                partition_id_t p_id = partition.get_id();
                mt_size_t n_models_par = n_models;
                results.emplace_back(
                  pool->enqueue([cur_model, model, n_models_par, p_id, eps_par, eps_opt, this]
                  {
                      emit next_model( model, modeltest::this_thread_id );
                      optimize_single(p_id, n_models_par, model, modeltest::this_thread_id, epsilon_param, epsilon_opt);
                  })
                );
            }
        }

        delete pool;
        pool = 0;

        emit optimized_partition_done(partition.get_id());
    }

    if (interrupt)
        emit optimization_interrupted();
    else
        emit optimization_done();
}

void xThreadOpt::kill_threads()
{
    interrupt = true;
    if (pool)
        pool->clean();
}
