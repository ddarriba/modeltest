#ifndef PARAMETER_GAMMA_H
#define PARAMETER_GAMMA_H

#include "parameter_ratecats.h"

namespace modeltest
{

class ParameterGamma : public ParameterRateCats
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
  virtual double get_alpha( void ) const;
  virtual void set_alpha( double alpha );

  virtual mt_size_t get_n_free_parameters( void ) const;
private:
  double alpha;
};

} /* namespace modeltest */

#endif
