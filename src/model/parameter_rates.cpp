#include "parameter_rates.h"

#define MIN_RATE 0.02
#define MAX_RATE 100

using namespace std;

namespace modeltest
{

struct custom_params {
  mt_opt_params_t * common_params;
  int n_subst_free_params;
  const int * symmetries;
};

static double target_func(void *p, double *x)
{
  struct custom_params * params = (struct custom_params *) p;
  mt_opt_params_t * mt_params = (mt_opt_params_t *) params->common_params;
  pll_partition_t * partition = mt_params->partition;
  const int * symmetries = params->symmetries;
  int n_subst_free_params = params->n_subst_free_params;
  mt_size_t states = partition->states;
  int n_subst_params = (int)(states * (states-1) / 2);

  /* update substitution rates */
  double * subst_rates = new double[n_subst_params];
  if (n_subst_free_params < n_subst_params)
  {
    assert(symmetries);

    int i, j, k;
    int n_subst_free_params = 0;

    /* assign values to the substitution rates */
    k = 0;
    for (i = 0; i <= n_subst_free_params; i++)
    {
      double next_value = (i == symmetries[n_subst_params - 1]) ? 1.0 : x[k++];
      for (j = 0; j < n_subst_params; j++)
        if (symmetries[j] == i)
        {
          subst_rates[j] = next_value;
        }
    }
  }
  else
  {
    memcpy (subst_rates, x, ((size_t)n_subst_params - 1) * sizeof(double));
    subst_rates[n_subst_params - 1] = 1.0;
  }

  pll_set_subst_params (partition,
                        0,
                        subst_rates);
  delete[] subst_rates;

  /* compute negative score */
  double score = -1 *
                 pll_utree_compute_lk(mt_params->partition,
                                      mt_params->tree,
                                      mt_params->params_indices,
                                      1,   /* update pmatrices */
                                      1);  /* update partials */
  return score;
}

ParameterRates::ParameterRates(vector<int> symmetries)
  : symmetries(symmetries)
{
}

ParameterRates::ParameterRates(const int * a_symmetries)
{
  symmetries.resize(6);
  for (int i=0; i<6; ++i)
    symmetries[i] = a_symmetries[i];
}

ParameterRates::~ParameterRates( void )
{

}

bool ParameterRates::initialize(const partition_descriptor_t & partition_desc)
{

  return true;
}

double ParameterRates::optimize(mt_opt_params_t * params,
                                double loglikelihood,
                                double tolerance,
                                bool first_guess)
{
  UNUSED(first_guess);
  double cur_logl;

  if (!symmetries.size())
    return loglikelihood;

  int bound_type[6] = {PLL_LBFGSB_BOUND_BOTH,PLL_LBFGSB_BOUND_BOTH,PLL_LBFGSB_BOUND_BOTH,
                       PLL_LBFGSB_BOUND_BOTH,PLL_LBFGSB_BOUND_BOTH,PLL_LBFGSB_BOUND_BOTH};

  struct custom_params opt_params;
  opt_params.common_params = params;
  opt_params.symmetries = &symmetries[0];
  opt_params.n_subst_free_params = n_subst_free_params;

  cur_logl = pll_minimize_lbfgsb(x, lower, upper, bound_type,
                                 n_subst_free_params,
                                 1e7, tolerance,
                                 (void *) &opt_params,
                                 &target_func);

  /* update rates */
  target_func((void *)params, x);

  return cur_logl;
}

void ParameterRates::print(std::ostream  &out)
{

}

} /* namespace modeltest */
