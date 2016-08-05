#include "parameter_pinv.h"
#include "partition.h"

#define MIN_PINV 0.02
#define MAX_PINV 0.95

using namespace std;

namespace modeltest
{

ParameterPinv::ParameterPinv( void )
{
  pinv = 0.5;
  min_pinv = MIN_PINV;
  max_pinv = MAX_PINV;
}

ParameterPinv::ParameterPinv( const ParameterPinv & other )
{
  pinv = other.pinv;
}

ParameterPinv::~ParameterPinv( void )
{

}

bool ParameterPinv::initialize(mt_opt_params_t * params,
                               Partition const& partition)
{
  max_pinv = partition.get_empirical_pinv();
  min_pinv = MIN_PINV;
  if (max_pinv > min_pinv)
  {
    pinv = (min_pinv + max_pinv) / 2;
  }
  else
  {
    pinv = max_pinv = MIN_PINV;
  }

  for (mt_index_t i=0; i<params->partition->rate_cats; ++i)
    pll_update_invariant_sites_proportion(params->partition,
                                          params->params_indices[i],
                                          pinv);
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

  pinv = params->partition->prop_invar[0];

  assert(!loglikelihood || (cur_logl - loglikelihood)/loglikelihood < 1e-10);

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
