#include "parameter_ratecats.h"

#define MIN_RATE 0.02
#define MAX_RATE 2

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

ParameterRateCats::ParameterRateCats(mt_size_t n_cats)
  : n_cats(n_cats)
{
}

ParameterRateCats::ParameterRateCats( const ParameterRateCats & other )
{
  n_cats = other.n_cats;
}

ParameterRateCats::~ParameterRateCats( void )
{

}

bool ParameterRateCats::initialize(Partition const& partition)
{
  return true;
}

double ParameterRateCats::optimize(mt_opt_params_t * params,
                                double loglikelihood,
                                double tolerance,
                                bool first_guess)
{
  UNUSED(first_guess);
  // double cur_log;
  // double f2x;
  // double xres;

  // if (n_cats == 1)
    return loglikelihood;

  // xres = pll_minimize_brent(MIN_ALPHA, alpha, MAX_ALPHA,
  //                               tolerance,
  //                               &cur_logl,
  //                               &f2x,
  //                               (void *) params,
  //                               &target_func);
  //
  // /* update alpha */
  // target_func(params, xres);
  // alpha = xres;

  // return cur_logl;
}

void ParameterRateCats::print(std::ostream  &out) const
{

}

mt_size_t ParameterRateCats::get_n_free_parameters( void ) const
{
  return (2*n_cats - 1);
}

} /* namespace modeltest */
