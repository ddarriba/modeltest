#include "xthreadopt.h"
#include "service/modeltestservice.h"

#include <map>
#include <vector>
#include <iomanip>
#include <iostream>
#include <algorithm>

//using namespace modeltest;
using namespace std;

xThreadOpt::xThreadOpt(modeltest::ModelTest * mtest,
                       partition_id_t & part_id,
                       int n_threads,
                       double epsilon_param,
                       double epsilon_opt) :
    part_id(part_id), n_threads(n_threads),
    epsilon_param(epsilon_param), epsilon_opt(epsilon_opt)
{
    pool = 0;
    interrupt = false;
    assert(ModelTestService::initialized());
}

void xThreadOpt::update(Observable * subject)
{
    modeltest::ModelOptimizer * mopt = static_cast<modeltest::ModelOptimizer *>(subject);

    //emit next_model( model, thread_map[my_id] );
    if (interrupt)
    {
        mopt->interrupt();
    }

    emit next_parameter( mopt->get_cur_parameter(), mopt->get_opt_delta(), mopt->get_thread_number() );
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

    ModelTestService::instance()->optimize_single(part_id,
                                                  n_models,
                                                  model,
                                                  thread_id,
                                                  epsilon_param,
                                                  epsilon_opt,
                                                  {this});
//    modeltest::ModelOptimizer * mopt = mtest->get_model_optimizer(model,
//                                                                  part_id,
//                                                                  thread_id);
//    mopt->attach(this);
//    mopt->run(epsilon_param, epsilon_opt);
//    delete mopt;

    // check for models interrupted during optimization
    if (interrupt)
        return;

    emit optimized_single_done(model, n_models);
}

void xThreadOpt::run()
{
    mt_index_t cur_model;
    mt_size_t n_models = ModelTestService::instance()->get_number_of_models(part_id);
    assert(n_models);

    if (n_threads == 1)
    {
        for (cur_model=0; cur_model < n_models; cur_model++)
        {
            modeltest::Model *model = ModelTestService::instance()->get_model(part_id, cur_model);
            emit next_model( model, 0 );
            optimize_single(part_id, n_models, model, 0, epsilon_param, epsilon_opt);
        }
    }
    else
    {
        pool = new modeltest::ThreadPool(n_threads);
        vector< future<void> > results;
        map<thread::id, mt_index_t> thread_map = pool->worker_ids;

        cout << "Starting jobs... (output might be unsorted)" << endl;
        for (cur_model=0; cur_model < n_models; cur_model++)
        {
            modeltest::Model *model = ModelTestService::instance()->get_model(part_id, cur_model);

            double eps_par = epsilon_param;
            double eps_opt = epsilon_opt;
            partition_id_t p_id = part_id;
            results.emplace_back(
              pool->enqueue([cur_model, model, n_models, p_id, eps_par, eps_opt, this, &thread_map] {
                  thread::id my_id(__gthread_self());
                  emit next_model( model, thread_map[my_id] );
                  optimize_single(part_id, n_models, model, thread_map[my_id], epsilon_param, epsilon_opt);
              })
            );
        }
    }

    delete pool;
    pool = 0;

    if (interrupt)
        emit optimization_interrupted(part_id);
    else
        emit optimization_done(part_id);
}

void xThreadOpt::kill_threads()
{
    interrupt = true;
    if (pool)
        pool->clean();
}
