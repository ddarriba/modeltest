#ifndef PARAMETER_SUBSTRATES_H
#define PARAMETER_SUBSTRATES_H

#include "abstract_parameter.h"
#include <vector>

namespace modeltest
{

class ParameterSubstRates : public AbstractParameter
{
public:
  virtual bool initialize(mt_opt_params_t * params,
                          Partition const& partition);

  const double * get_subst_rates( void ) const;

protected:
  double *subst_rates;
  mt_size_t n_subst_free_params;
  mt_size_t n_subst_params;
};

class ParameterSubstRatesOpt : public ParameterSubstRates
{
public:
  ParameterSubstRatesOpt(std::vector<int> symmetries);
  ParameterSubstRatesOpt(const ParameterSubstRatesOpt & other);
  ParameterSubstRatesOpt(const int * symmetries, mt_size_t n_subst_params);
  ~ParameterSubstRatesOpt(void);
  virtual bool initialize(mt_opt_params_t * params,
                          Partition const& partition);
  virtual double optimize(mt_opt_params_t * params,
                          double loglikelihood,
                          double tolerance = DEFAULT_PARAM_EPSILON,
                          bool first_guess = false);
  virtual void print(std::ostream  &out = std::cout) const;
  virtual mt_size_t get_n_free_parameters( void ) const;

protected:
  std::vector<int> symmetries;
};

} /* namespace modeltest */

#endif
