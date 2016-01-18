#include "xthreadopt.h"

#include "thread/threadpool.h"
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
    mtest(mtest), part_id(part_id), n_threads(n_threads),
    epsilon_param(epsilon_param), epsilon_opt(epsilon_opt)
{

}

void xThreadOpt::optimize_single(modeltest::ModelTest *mtest,
                            const partition_id_t &part_id,
                            mt_index_t n_models,
                            modeltest::Model *model,
                            mt_index_t thread_id,
                            double epsilon_param,
                            double epsilon_opt)
{
        mtest->evaluate_single_model(model, part_id, thread_id, epsilon_param, epsilon_opt);
        emit optimized_single_done(model, n_models);
}

void xThreadOpt::run()
{
    mt_index_t cur_model;
    mt_size_t n_models = mtest->get_models(part_id).size();
    assert(n_models);

    if (n_threads == 1)
    {
        for (cur_model=0; cur_model < mtest->get_models(part_id).size(); cur_model++)
        {
            modeltest::Model *model = mtest->get_models(part_id)[cur_model];
            optimize_single(mtest, part_id, n_models, model, 0, epsilon_param, epsilon_opt);
            //mtest->evaluate_single_model(model, part_id, 0, epsilon_param, epsilon_opt);
        }
    }
    else
    {
        modeltest::ThreadPool pool(n_threads);
        vector< future<void> > results;
        map<thread::id, mt_index_t> thread_map = pool.worker_ids;

        cout << "Starting jobs... (output might be unsorted)" << endl;
        for (cur_model=0; cur_model < mtest->get_models(part_id).size(); cur_model++)
        {
            modeltest::Model *model = mtest->get_models(part_id)[cur_model];

            double eps_par = epsilon_param;
            double eps_opt = epsilon_opt;
            partition_id_t p_id = part_id;
            results.emplace_back(
              pool.enqueue([cur_model, model, n_models, p_id, eps_par, eps_opt, this, &thread_map] {
                  thread::id my_id(__gthread_self());
                  optimize_single(mtest, part_id, n_models, model, thread_map[my_id], epsilon_param, epsilon_opt);
//                thread::id my_id(__gthread_self());
//                int res =  mtest->evaluate_single_model(model, part_id, 0, epsilon_part, epsilon_opt);
//                return res;
              })
            );
        }
    }

    emit optimization_done(part_id);
}
