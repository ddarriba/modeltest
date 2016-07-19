#ifndef PARAMETER_RATES_H
#define PARAMETER_RATES_H

#include "abstract_parameter.h"

namespace modeltest
{

class ParameterGamma : public AbstractParameter
{
public:
  ParameterGamma(mt_size_t n_cats);
  ~ParameterGamma( void );
  virtual bool initialize(const partition_descriptor_t & partition_desc);
  virtual double optimize(mt_opt_params_t * params,
                          double loglikelihood,
                          double tolerance = DEFAULT_PARAM_EPSILON,
                          bool first_guess = false);
  virtual void print(std::ostream  &out = std::cout);
private:
  mt_size_t n_cats;
  double alpha;
};

} /* namespace modeltest */

#endif
