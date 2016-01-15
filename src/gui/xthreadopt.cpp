#include "xthreadopt.h"

#include <iostream>

xThreadOpt::xThreadOpt(modeltest::ModelTest * mtest,
                       partition_id_t & part_id,
                       int n_threads) :
    mtest(mtest), part_id(part_id), n_threads(n_threads)
{

}

void xThreadOpt::run()
{
    mtest->evaluate_models(part_id, n_threads, 0.01, 0.1);

    emit optimization_done(part_id);
}
