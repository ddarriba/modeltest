#include "parameter_gamma.h"

#define MIN_ALPHA 0.02
#define MAX_ALPHA 100

using namespace std;

namespace modeltest
{

ParameterGamma::ParameterGamma(mt_size_t n_cats, double alpha)
  : ParameterRateCats(n_cats), alpha(alpha)
{
  alpha = 1.0;
}

ParameterGamma::ParameterGamma( const ParameterGamma & other )
  : ParameterRateCats(other.n_cats)
{
  alpha = other.alpha;
}

ParameterGamma::~ParameterGamma( void )
{

}

bool ParameterGamma::initialize(mt_opt_params_t * params,
                                Partition const& partition)
{
  assert(n_cats == params->partition->rate_cats);
  pll_compute_gamma_cats (alpha,
                          n_cats,
                          rates);
  pll_set_category_rates(params->partition,
                         rates);
  return true;
}

double ParameterGamma::optimize(mt_opt_params_t * params,
                                double loglikelihood,
                                double tolerance,
                                bool first_guess)
{
  double cur_logl, alpha_prev;
  UNUSED(first_guess);

  if (n_cats == 1)
    return loglikelihood;

  alpha_prev = alpha;
  cur_logl = pllmod_algo_opt_alpha(params->partition,
                                   params->tree,
                                   params->params_indices,
                                   MIN_ALPHA,
                                   MAX_ALPHA,
                                   &alpha,
                                   tolerance);

  assert(!loglikelihood || (cur_logl - loglikelihood)/loglikelihood <= 1e-10);
  extract_rates_and_weights(params);

  return cur_logl;
}

void ParameterGamma::print(std::ostream  &out) const
{
  UNUSED(out);
}

double ParameterGamma::get_alpha( void ) const
{
  return alpha;
}

void ParameterGamma::set_alpha( double _alpha )
{
  alpha = _alpha;
}

mt_size_t ParameterGamma::get_n_free_parameters( void ) const
{
  return (n_cats > 1)?1:0;
}

} /* namespace modeltest */
