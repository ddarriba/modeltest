#ifndef PARAMETER_RATES_H
#define PARAMETER_RATES_H

#include "abstract_parameter.h"

namespace modeltest
{

class ParameterGamma : public AbstractParameter
{
public:
  ParameterGamma(mt_size_t n_cats, double alpha = 0.);
  ParameterGamma( const ParameterGamma & other );
  ~ParameterGamma( void );
  virtual bool initialize(Partition const& partition);
  virtual double optimize(mt_opt_params_t * params,
                          double loglikelihood,
                          double tolerance = DEFAULT_PARAM_EPSILON,
                          bool first_guess = false);
  virtual void print(std::ostream  &out = std::cout) const;
  double get_alpha( void ) const;
  void set_alpha( double alpha );

  virtual mt_size_t get_n_free_parameters( void ) const;
private:
  mt_size_t n_cats;
  double alpha;
};

} /* namespace modeltest */

#endif
