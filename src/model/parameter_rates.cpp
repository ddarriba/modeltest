#include "parameter_rates.h"

#define MIN_RATE 0.02
#define MAX_RATE 100

using namespace std;

namespace modeltest
{

static double target_func(void *p, double *x);

struct custom_params {
  mt_opt_params_t * common_params;
  int n_subst_free_params;
  const int * symmetries;
};

bool ParameterRates::initialize(Partition const& partition)
{
  return true;
}

/******************************************************************************/
/* OPTIMIZABLE RATES (defined symmetries) */
/******************************************************************************/

ParameterRatesOpt::ParameterRatesOpt(vector<int> symmetries)
  : ParameterRates(), symmetries(symmetries)
{
  n_subst_params = symmetries.size();
  for (mt_index_t i=0; i<n_subst_params; ++i)
  {
    if (symmetries[i] > (int)n_subst_free_params)
      n_subst_free_params = (int)symmetries[i];
  }

  x = new double[n_subst_free_params];
  lower = new double[n_subst_free_params];
  upper = new double[n_subst_free_params];
  bound_type = new int[n_subst_free_params];

  for (mt_index_t i=0; i<n_subst_free_params; ++i)
  {
    lower[i] = MIN_RATE;
    upper[i] = MAX_RATE;
    bound_type[i] = PLL_LBFGSB_BOUND_BOTH;
  }
}

ParameterRatesOpt::ParameterRatesOpt(const int * a_symmetries)
  : ParameterRates()
{
  n_subst_params = 6;
  symmetries.resize(n_subst_params);
  n_subst_free_params = 0;
  for (mt_index_t i=0; i<n_subst_params; ++i)
  {
    symmetries[i] = a_symmetries[i];
    if (symmetries[i] > (int) n_subst_free_params)
      n_subst_free_params = symmetries[i];
  }

  x = new double[n_subst_free_params];
  lower = new double[n_subst_free_params];
  upper = new double[n_subst_free_params];
  bound_type = new int[n_subst_free_params];

  for (mt_index_t i=0; i<n_subst_free_params; ++i)
  {
    lower[i] = MIN_RATE;
    upper[i] = MAX_RATE;
    bound_type[i] = PLL_LBFGSB_BOUND_BOTH;
  }
}

ParameterRatesOpt::ParameterRatesOpt( const ParameterRatesOpt & other )
  : ParameterRates()
{
  symmetries = other.symmetries;
  n_subst_params = other.n_subst_params;
  n_subst_free_params = other.n_subst_free_params;

  x = new double[n_subst_free_params];
  lower = new double[n_subst_free_params];
  upper = new double[n_subst_free_params];
  bound_type = new int[n_subst_free_params];
  memcpy(x, other.x, sizeof(double) * n_subst_free_params);
  memcpy(lower, other.lower, sizeof(double) * n_subst_free_params);
  memcpy(upper, other.upper, sizeof(double) * n_subst_free_params);
  memcpy(bound_type, other.bound_type, sizeof(int) * n_subst_free_params);
}

ParameterRatesOpt::~ParameterRatesOpt( void )
{
  delete[] x;
  delete[] lower;
  delete[] upper;
  delete[] bound_type;
}

double ParameterRatesOpt::optimize(mt_opt_params_t * params,
                                double loglikelihood,
                                double tolerance,
                                bool first_guess)
{
  UNUSED(first_guess);
  double cur_logl;

  mt_size_t n_subst_params = 6;

  if (!n_subst_free_params)
    return loglikelihood;

  /* assign values to the substitution rates */
  for (mt_index_t i = 0; i < n_subst_free_params; i++)
  {
    for (mt_index_t j = 0; j < n_subst_params; j++)
      if (symmetries[j] == (int)i)
      {
        x[i] = params->partition->subst_params[0][j];
        break;
      }
  }

  struct custom_params opt_params;
  opt_params.common_params = params;
  opt_params.symmetries = &symmetries[0];
  opt_params.n_subst_free_params = n_subst_free_params;

  cur_logl = pll_minimize_lbfgsb(x, lower, upper, bound_type,
                                 n_subst_free_params,
                                 1e9, tolerance,
                                 (void *) &opt_params,
                                 &target_func);

  /* update rates */
  target_func((void *)&opt_params, x);

  return cur_logl;
}

void ParameterRatesOpt::print(std::ostream  &out) const
{

}

mt_size_t ParameterRatesOpt::get_n_free_parameters( void ) const
{
  return n_subst_free_params;
}

/******************************************************************************/
/* Static declarations */
/******************************************************************************/

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

} /* namespace modeltest */
