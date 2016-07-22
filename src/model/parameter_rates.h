#ifndef PARAMETER_GAMMA_H
#define PARAMETER_GAMMA_H

#include "abstract_parameter.h"
#include <vector>

namespace modeltest
{

class ParameterRates : public AbstractParameter
{
public:
  virtual bool initialize(Partition const& partition);
protected:
  mt_size_t n_subst_free_params;
  mt_size_t n_subst_params;
};

class ParameterRatesOpt : public ParameterRates
{
public:
  ParameterRatesOpt(std::vector<int> symmetries);
  ParameterRatesOpt(const ParameterRatesOpt & other);
  ParameterRatesOpt(const int * symmetries);
  ~ParameterRatesOpt(void);
  virtual double optimize(mt_opt_params_t * params,
                          double loglikelihood,
                          double tolerance = DEFAULT_PARAM_EPSILON,
                          bool first_guess = false);
  virtual void print(std::ostream  &out = std::cout) const;
  virtual mt_size_t get_n_free_parameters( void ) const;
protected:
  std::vector<int> symmetries;
  double *x;
  double *lower, *upper;
  int *bound_type;
};

} /* namespace modeltest */

#endif
