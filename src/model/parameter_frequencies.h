#ifndef PARAMETER_FREQUENCIES_H
#define PARAMETER_FREQUENCIES_H

#include "abstract_parameter.h"
#include <vector>

namespace modeltest
{

class ParameterFrequencies : public AbstractParameter
{
public:
  ParameterFrequencies( void );
  ~ParameterFrequencies( void );
  virtual bool initialize(const partition_descriptor_t & partition_desc);
  virtual double optimize(mt_opt_params_t * params,
                          double loglikelihood,
                          double tolerance = DEFAULT_PARAM_EPSILON,
                          bool first_guess = false);
  virtual void print(std::ostream  &out = std::cout);
private:
  double *x;
  double *lower, *upper;
};

} /* namespace modeltest */

#endif
