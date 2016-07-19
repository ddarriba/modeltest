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

ParameterGamma::ParameterGamma(mt_size_t n_cats)
  : n_cats(n_cats)
{
  alpha = 0.5;
}

ParameterGamma::~ParameterGamma( void )
{

}

bool ParameterGamma::initialize(const partition_descriptor_t & partition_desc)
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
    
  xres = pll_minimize_brent(MIN_ALPHA, alpha, MAX_ALPHA,
                                tolerance,
                                &cur_logl,
                                &f2x,
                                (void *) params,
                                &target_func);

  /* update alpha */
  target_func(params, xres);

  return cur_logl;
}

void ParameterGamma::print(std::ostream  &out)
{

}

} /* namespace modeltest */
