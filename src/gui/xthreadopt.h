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
