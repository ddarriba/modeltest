#include "parameter_substrates.h"

#define MIN_RATE 0.02
#define MAX_RATE 100

using namespace std;

namespace modeltest
{

bool ParameterSubstRates::initialize(Partition const& partition)
{
  return true;
}

const double * ParameterSubstRates::get_subst_rates( void ) const
{
  return subst_rates;
}

/******************************************************************************/
/* OPTIMIZABLE RATES (defined symmetries) */
/******************************************************************************/

ParameterSubstRatesOpt::ParameterSubstRatesOpt(vector<int> symmetries)
  : ParameterSubstRates(),
    symmetries(symmetries)
{
  n_subst_params = symmetries.size();
  subst_rates = new double[n_subst_params];
  for (mt_index_t i=0; i<n_subst_params; ++i)
  {
    subst_rates[i] = 1.0;
    if (symmetries[i] > (int)n_subst_free_params)
      n_subst_free_params = (int)symmetries[i];
  }
}

ParameterSubstRatesOpt::ParameterSubstRatesOpt(const int * a_symmetries,
                                     mt_size_t _n_subst_params)
  : ParameterSubstRates()
{
  n_subst_params = _n_subst_params;
  symmetries.resize(n_subst_params);
  n_subst_free_params = 0;
  for (mt_index_t i=0; i<n_subst_params; ++i)
  {
    symmetries[i] = a_symmetries[i];
    if (symmetries[i] > (int) n_subst_free_params)
      n_subst_free_params = symmetries[i];
  }
  subst_rates = new double[n_subst_params];
  for (mt_index_t i=0; i<n_subst_params; ++i)
    subst_rates[i] = 1.0;
}

ParameterSubstRatesOpt::ParameterSubstRatesOpt( const ParameterSubstRatesOpt & other )
  : ParameterSubstRates()
{
  symmetries = other.symmetries;
  n_subst_params = other.n_subst_params;
  n_subst_free_params = other.n_subst_free_params;
  subst_rates = new double[n_subst_params];
  memcpy(subst_rates, other.subst_rates, sizeof(double)*n_subst_params);
}

ParameterSubstRatesOpt::~ParameterSubstRatesOpt( void )
{
}

double ParameterSubstRatesOpt::optimize(mt_opt_params_t * params,
                                double loglikelihood,
                                double tolerance,
                                bool first_guess)
{
  UNUSED(first_guess);
  double cur_logl;

  cur_logl = pllmod_algo_opt_subst_rates (params->partition,
                                          params->tree,
                                          0,
                                          params->params_indices,
                                          (int *)&symmetries[0],
                                          MIN_RATE,
                                          MAX_RATE,
                                          tolerance);

  memcpy(subst_rates, params->partition->subst_params[0], n_subst_params * sizeof(double));
  return cur_logl;
}

void ParameterSubstRatesOpt::print(std::ostream  &out) const
{

}

mt_size_t ParameterSubstRatesOpt::get_n_free_parameters( void ) const
{
  return n_subst_free_params;
}

} /* namespace modeltest */
