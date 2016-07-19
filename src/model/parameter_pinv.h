#ifndef PARAMETER_PINV_H
#define PARAMETER_PINV_H

#include "abstract_parameter.h"

namespace modeltest
{

class ParameterPinv : public AbstractParameter
{
public:
  ParameterPinv( void );
  ~ParameterPinv( void );
  virtual bool initialize(const partition_descriptor_t & partition_desc);
  virtual double optimize(mt_opt_params_t * params,
                          double loglikelihood,
                          double tolerance = DEFAULT_PARAM_EPSILON,
                          bool first_guess = false);
  virtual void print(std::ostream  &out = std::cout);
private:
  double pinv;
};

} /* namespace modeltest */

#endif
