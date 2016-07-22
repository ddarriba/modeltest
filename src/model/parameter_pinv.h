#ifndef PARAMETER_PINV_H
#define PARAMETER_PINV_H

#include "abstract_parameter.h"

namespace modeltest
{

class ParameterPinv : public AbstractParameter
{
public:
  ParameterPinv( void );
  ParameterPinv( const ParameterPinv & other );
  ~ParameterPinv( void );
  virtual bool initialize(Partition const& partition);
  virtual double optimize(mt_opt_params_t * params,
                          double loglikelihood,
                          double tolerance = DEFAULT_PARAM_EPSILON,
                          bool first_guess = false);
  virtual void print(std::ostream  &out = std::cout) const;
  double get_pinv() const;
  void set_pinv( double value );
  virtual mt_size_t get_n_free_parameters( void ) const;
  
private:
  double pinv;
};

} /* namespace modeltest */

#endif
