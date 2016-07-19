#include "parameter_pinv.h"

#define MIN_PINV 0.02
#define MAX_PINV 0.95

using namespace std;

namespace modeltest
{

static double target_func(void *p, double x)
{
  mt_opt_params_t * params = (mt_opt_params_t *) p;
  pll_partition_t * partition = params->partition;

  /* update proportion of invariant sites */
  for (mt_index_t i=0; i<partition->rate_cats; ++i)
  {
    pll_update_invariant_sites_proportion(partition,
                                          params->params_indices[i],
                                          x);
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

ParameterPinv::ParameterPinv( void )
{
}

ParameterPinv::~ParameterPinv( void )
{

}

bool ParameterPinv::initialize(const partition_descriptor_t & partition_desc)
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
  double f2x;
  double xres;

  xres = pll_minimize_brent(MIN_PINV, pinv, MAX_PINV,
                            tolerance,
                            &cur_logl,
                            &f2x,
                            (void *) params,
                            &target_func);

  /* update pinv */
  target_func(params, xres);

  return cur_logl;
}

void ParameterPinv::print(std::ostream  &out)
{

}

} /* namespace modeltest */
