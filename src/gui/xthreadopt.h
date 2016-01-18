#ifndef XTHREADOPT_H
#define XTHREADOPT_H

#include <qthread.h>
#include "modeltest.h"

class xThreadOpt : public QThread
{
    Q_OBJECT
public:
    xThreadOpt(modeltest::ModelTest * mtest,
               partition_id_t & part_id,
               int n_threads,
               double epsilon_param,
               double epsilon_opt);
signals:
    void optimization_done( partition_id_t );
    void optimized_single_done( modeltest::Model *, unsigned int );
protected:
    void run();
private:
    void optimize_single(modeltest::ModelTest *mtest,
                                const partition_id_t &part_id,
                                mt_index_t n_models,
                                modeltest::Model *model,
                                mt_index_t thread_id,
                                double epsilon_param,
                                double epsilon_opt);

    modeltest::ModelTest * mtest;
    partition_id_t part_id;
    int n_threads;

    double epsilon_param;
    double epsilon_opt;
};
#endif // XTHREADOPT_H
