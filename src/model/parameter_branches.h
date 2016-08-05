#ifndef PARAMETER_BRANCHES_H
#define PARAMETER_BRANCHES_H

#include "abstract_parameter.h"

namespace modeltest
{

class ParameterBranches : public AbstractParameter
{
public:
  ParameterBranches( void  );
  ParameterBranches( const ParameterBranches & other );
  ~ParameterBranches( void );
  virtual bool initialize(mt_opt_params_t * params,
                          Partition const& partition);
  virtual double optimize(mt_opt_params_t * params,
                          double loglikelihood,
                          double tolerance = DEFAULT_PARAM_EPSILON,
                          bool first_guess = false);
  virtual void print(std::ostream  &out = std::cout) const;
  virtual mt_size_t get_n_free_parameters( void ) const;
private:
  mt_size_t n_branches;
};

} /* namespace modeltest */

#endif
