#include "parameter_substrates.h"
#include "partition.h"

#define MIN_RATE 0.02
#define MAX_RATE 100

using namespace std;

namespace modeltest
{

bool ParameterSubstRates::initialize(mt_opt_params_t * params,
                                     Partition const& partition)
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
  delete[] subst_rates;
}

bool ParameterSubstRatesOpt::initialize(mt_opt_params_t * params,
                                     Partition const& partition)
{
    const vector<double> empirical_rates = partition.get_empirical_subst_rates();
    assert(empirical_rates.size() == n_subst_params);
    
    for (mt_index_t i=0; i<n_subst_free_params; ++i)
    {
        double sum_rate = 0;
        int count = 0;
        for (mt_index_t j=0; j<n_subst_params; ++j)
        {
            if ((mt_index_t)symmetries[j] == i)
            {
                ++count;
                sum_rate += empirical_rates[j];
            }
        }
        assert(count);
        sum_rate /= count;

        for (mt_index_t j=0; j<n_subst_params; ++j)
            if ((mt_index_t)symmetries[j] == i)
                subst_rates[j] = sum_rate;
    }
    for (mt_index_t j=0; j<n_subst_params; ++j)
        subst_rates[j] /= subst_rates[n_subst_params-1];
    pll_set_subst_params(params->partition, 0, subst_rates);

  return true;
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

  assert(!loglikelihood || (cur_logl - loglikelihood)/loglikelihood < 1e-10);

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
