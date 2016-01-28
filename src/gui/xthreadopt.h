#ifndef XTHREADOPT_H
#define XTHREADOPT_H

#include "partitioning_scheme.h"

#include "thread/threadpool.h"
#include "thread/observer.h"

#include <qthread.h>

class xThreadOpt : public QThread, public Observer
{
    Q_OBJECT
public:
    xThreadOpt(partition_id_t & part_id,
               int n_threads,
               double epsilon_param,
               double epsilon_opt);
    xThreadOpt(modeltest::PartitioningScheme & scheme,
               int n_threads,
               double epsilon_param,
               double epsilon_opt);
    virtual void update(Observable * subject);
    mt_size_t get_number_of_models( void ) const;

public slots:
    void kill_threads();

signals:
    void optimization_done( );
    void optimization_interrupted( );
    void optimized_partition_done( partition_id_t );
    void optimized_single_done( modeltest::Model *, unsigned int );
    void next_model( modeltest::Model *, unsigned int );
    void next_parameter( unsigned int, double, unsigned int );
protected:
    void run();

private:
    void optimize_single(const partition_id_t &part_id,
                                mt_index_t n_models,
                                modeltest::Model *model,
                                mt_index_t thread_id,
                                double epsilon_param,
                                double epsilon_opt);

    modeltest::ThreadPool * pool;
    modeltest::PartitioningScheme & scheme;
    int n_threads;
    mt_size_t n_models;

    double epsilon_param;
    double epsilon_opt;
    bool interrupt;
};
#endif // XTHREADOPT_H
