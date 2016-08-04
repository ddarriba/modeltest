#ifndef PARAMETER_RATECATS_H
#define PARAMETER_RATECATS_H

#include "abstract_parameter.h"

namespace modeltest
{

/* optimize rate categories and weights */
class ParameterRateCats : public AbstractParameter
{
public:
  ParameterRateCats(mt_size_t n_cats);
  ParameterRateCats( const ParameterRateCats & other );
  ~ParameterRateCats( void );
  virtual bool initialize(Partition const& partition);
  virtual double optimize(mt_opt_params_t * params,
                          double loglikelihood,
                          double tolerance = DEFAULT_PARAM_EPSILON,
                          bool first_guess = false);
  virtual void print(std::ostream  &out = std::cout) const;
  virtual double get_alpha( void ) const { return -1; }
  virtual void set_alpha( double alpha ) { UNUSED(alpha); }

  virtual mt_size_t get_n_free_parameters( void ) const;
protected:
  mt_size_t n_cats;
};

} /* namespace modeltest */

#endif
