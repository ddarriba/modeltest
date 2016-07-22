#include "parameter_gamma.h"

#define MIN_ALPHA 0.02
#define MAX_ALPHA 100

using namespace std;

namespace modeltest
{

static double target_func(void *p, double x)
{
  mt_opt_params_t * params = (mt_opt_params_t *) p;
  pll_partition_t * partition = params->partition;

  /* update rate categories */
  if (!pll_compute_gamma_cats (x, partition->rate_cats, partition->rates))
  {
    return PLL_FAILURE;
  }

  /* compute negative score */
  double score = -1 *
                 pll_utree_compute_lk(params->partition,
                                      params->tree,
                                      params->params_indices,
                                      1,   /* update pmatrices */
                                      1);  /* update partials */
  return score;
}

ParameterGamma::ParameterGamma(mt_size_t n_cats, double alpha)
  : n_cats(n_cats), alpha(alpha)
{
}

ParameterGamma::ParameterGamma( const ParameterGamma & other )
{
  n_cats = other.n_cats;
  alpha = other.alpha;
}

ParameterGamma::~ParameterGamma( void )
{

}

bool ParameterGamma::initialize(Partition const& partition)
{

  return true;
}

double ParameterGamma::optimize(mt_opt_params_t * params,
                                double loglikelihood,
                                double tolerance,
                                bool first_guess)
{
  UNUSED(first_guess);
  double cur_logl;
  double f2x;
  double xres;

  if (n_cats == 1)
    return loglikelihood;

// printf("%f\n", loglikelihood);
  xres = pll_minimize_brent(MIN_ALPHA, alpha, MAX_ALPHA,
                                tolerance,
                                &cur_logl,
                                &f2x,
                                (void *) params,
                                &target_func);

  /* update alpha */
  target_func(params, xres);
  alpha = xres;

  return cur_logl;
}

void ParameterGamma::print(std::ostream  &out) const
{

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
