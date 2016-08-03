#include "parameter_pinv.h"

#define MIN_PINV 0.02
#define MAX_PINV 0.95

using namespace std;

namespace modeltest
{

ParameterPinv::ParameterPinv( void )
{
  pinv = 0.5;
}

ParameterPinv::ParameterPinv( const ParameterPinv & other )
{
  pinv = other.pinv;
}

ParameterPinv::~ParameterPinv( void )
{

}

bool ParameterPinv::initialize(Partition const& partition)
{
  return true;
}

double ParameterPinv::optimize(mt_opt_params_t * params,
                               double loglikelihood,
                               double tolerance,
                               bool first_guess)
{
  UNUSED(first_guess);
  double cur_logl;

  cur_logl = pllmod_algo_opt_pinv (params->partition,
                                   params->tree,
                                   params->params_indices,
                                   MIN_PINV,
                                   MAX_PINV,
                                   tolerance);

  return cur_logl;
}

void ParameterPinv::print(std::ostream  &out) const
{

}

double ParameterPinv::get_pinv() const
{
  return pinv;
}

void ParameterPinv::set_pinv( double value )
{
  pinv = value;
}

mt_size_t ParameterPinv::get_n_free_parameters( void ) const
{
  return 1;
}

} /* namespace modeltest */
