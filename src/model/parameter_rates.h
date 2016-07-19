#ifndef PARAMETER_GAMMA_H
#define PARAMETER_GAMMA_H

#include "abstract_parameter.h"
#include <vector>

namespace modeltest
{

class ParameterRates : public AbstractParameter
{
public:
  ParameterRates(std::vector<int> symmetries);
  ParameterRates(const int * symmetries);
  ~ParameterRates( void );
  virtual bool initialize(const partition_descriptor_t & partition_desc);
  virtual double optimize(mt_opt_params_t * params,
                          double loglikelihood,
                          double tolerance = DEFAULT_PARAM_EPSILON,
                          bool first_guess = false);
  virtual void print(std::ostream  &out = std::cout);
private:
  mt_size_t n_subst_free_params;
  std::vector<int> symmetries;
  double *x;
  double *lower, *upper;
};

} /* namespace modeltest */

#endif
