#include "parameter_ratecats.h"

#define MIN_RATE 0.02
#define MAX_RATE 2

/* epsilon for Weights/Rates optimization */
#define WR_EPSILON 0.9

using namespace std;

namespace modeltest
{

ParameterRateCats::ParameterRateCats(mt_size_t n_cats)
  : n_cats(n_cats)
{
  weights = new double[n_cats];
  rates   = new double[n_cats];

  pll_compute_gamma_cats (1.0,
                          n_cats,
                          rates);
  for (mt_index_t i=0; i<n_cats; ++i)
    weights[i] = 1.0/n_cats;
}

ParameterRateCats::ParameterRateCats( const ParameterRateCats & other )
{
  n_cats = other.n_cats;
  weights = new double[n_cats];
  rates   = new double[n_cats];
}

ParameterRateCats::~ParameterRateCats( void )
{
  delete[] weights;
  delete[] rates;
}

bool ParameterRateCats::initialize(mt_opt_params_t * params,
                                   Partition const& partition)
{
  assert(n_cats == params->partition->rate_cats);
  pll_set_category_rates(params->partition,
                         rates);
  pll_set_category_weights(params->partition,
                           weights);
  return true;
}

double * ParameterRateCats::get_weights( void ) const
{
  return weights;
}

double * ParameterRateCats::get_rates( void ) const
{
  return rates;
}

void ParameterRateCats::set_n_categories( mt_size_t n_categories )
{
  if (n_cats != n_categories)
  {
    n_cats = n_categories;
    delete[] weights;
    delete[] rates;
    weights = new double[n_cats];
    rates   = new double[n_cats];
  }
}

void ParameterRateCats::extract_rates_and_weights(mt_opt_params_t * params)
{
  memcpy(weights,
         params->partition->rate_weights,
         sizeof(double) * n_cats);
  memcpy(rates,
         params->partition->rates,
         sizeof(double) * n_cats);
}

double ParameterRateCats::optimize(mt_opt_params_t * params,
                                double loglikelihood,
                                double tolerance,
                                bool first_guess)
{
  UNUSED(first_guess);
  double cur_logl;

  cur_logl = pllmod_algo_opt_rates_weights(params->partition,
                                           params->tree,
                                           params->params_indices,
                                           0.01, 10,
                                           tolerance);

  assert(!loglikelihood || (cur_logl - loglikelihood)/loglikelihood < 1e-10);
  extract_rates_and_weights(params);

  return cur_logl;

}

void ParameterRateCats::print(std::ostream  &out) const
{
  UNUSED(out);
}

mt_size_t ParameterRateCats::get_n_free_parameters( void ) const
{
  return (2*n_cats - 1);
}

} /* namespace modeltest */
