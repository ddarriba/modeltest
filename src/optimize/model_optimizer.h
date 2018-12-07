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

#ifndef MODEL_OPTIMIZER_H_
#define MODEL_OPTIMIZER_H_

#include "../msa.h"
#include "../partition.h"
#include "../model/model.h"
#include "../thread/observer.h"

#define EXCEPTION_INTERNAL_ERROR  301
#define EXCEPTION_DATA_MISMATCH   302

namespace modeltest
{

  typedef enum {
      mt_param_branch_lengths        = 1<<0,
      mt_param_subst_rates           = 1<<1,
      mt_param_frequencies           = 1<<2,
      mt_param_alpha                 = 1<<3,
      mt_param_pinv                  = 1<<4,
      mt_param_mixture_rates_weights = 1<<5
  } mt_parameter_t;

  extern bool on_run;

  class ModelOptimizer : public Observable
  {
  public:
    ModelOptimizer (Msa & _msa,
                    Model & _model,
                    Partition & _partition,
                    bool _optimize_topology,
                    bool _keep_model_parameters,
                    int gamma_rates = PLL_GAMMA_RATES_MEAN,
                    mt_index_t _thread_number = 0)
        : msa(_msa),
          model(_model),
          partition(_partition),
          optimize_topology(_optimize_topology),
          keep_model_parameters(_keep_model_parameters),
          gamma_rates(gamma_rates),
          thread_number(_thread_number)
    {
        interrupt_optimization = false;
        optimized = false;

        cur_parameter = 0;
        opt_delta = 0.0;
        n_patterns = 0;
        n_sites = 0;
    }
    virtual ~ModelOptimizer ();

    /**
     * @brief Optimizes all parameters for the model
     * @param[in] epsilon     the tolerance of the global optimization
     * @param[in] tolerance   tolerance for parameter optimization
     * @param[in] num_threads number of threads for model optimization
     * @return true, if the optimization is OK
     */
    virtual bool run(double epsilon   = DEFAULT_OPT_EPSILON,
                     double tolerance = DEFAULT_PARAM_EPSILON,
                     mt_size_t num_threads = 1) = 0;

    /**
     * @brief Gets the optimization status of the model
     * @return true, if the model is optimized
     */
    int is_optimized() const { return optimized; }

    mt_index_t get_thread_number() const { return thread_number; }
    mt_index_t get_cur_parameter() const { return cur_parameter; }
    double get_opt_delta() const { return opt_delta; }
    void interrupt() { interrupt_optimization = true; }

  protected:
    bool optimized; //! optimization state
    Msa & msa;       //! the multiple sequence alignment
    Model & model;   //! the model to optimize
    Partition & partition;
    bool optimize_topology;
    bool keep_model_parameters;
    int gamma_rates;      //! Gamma rates mode (mean/median)
    mt_size_t n_sites;    //! original number of sites
    mt_size_t n_patterns; //! crunched number of sites

    mt_index_t thread_number;  //! the number of the current thread

    // dynamic optimization status
    bool interrupt_optimization;
    mt_index_t cur_parameter;
    double opt_delta;
  };

} /* namespace modeltest */

#endif /* MODEL_OPTIMIZER_H_ */
