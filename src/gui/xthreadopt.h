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
               int n_threads);
signals:
    void optimization_done( partition_id_t );
protected:
    void run();
private:
    modeltest::ModelTest * mtest;
    partition_id_t part_id;
    int n_threads;
};
#endif // XTHREADOPT_H
