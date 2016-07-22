#include "parameter_frequencies.h"
#include "partition.h"

#include <iostream>
#include <iomanip>

#define MIN_FREQ_RATE 0.01
#define MAX_FREQ_RATE 100

using namespace std;

namespace modeltest
{

static double target_func(void *p, double *x);

struct custom_params {
  mt_opt_params_t * common_params;
  mt_index_t highest_freq_state;
};

ParameterFrequencies::ParameterFrequencies( mt_size_t states ) :
  states(states)
{
  frequencies = 0;
}

ParameterFrequencies::~ParameterFrequencies( void )
{
}

const double * ParameterFrequencies::get_frequencies( void ) const
{
    return frequencies;
}

void ParameterFrequencies::set_frequencies(const double value[])
{
    memcpy(frequencies, value, states * sizeof(double));
}

void ParameterFrequencies::set_frequencies(const vector<double> & value)
{
    memcpy(frequencies, &(value[0]), states * sizeof(double));
}

void ParameterFrequencies::print(std::ostream  &out) const
{
  for (mt_index_t i=0; i<states; i++)
      out << setprecision(MT_PRECISION_DIGITS) << frequencies[i] << " ";
}

/******************************************************************************/

ParameterFrequenciesOpt::ParameterFrequenciesOpt( mt_size_t states )
  : ParameterFrequencies(states)
{
  x = new double[states - 1];
  lower = new double[states - 1];
  upper = new double[states - 1];
  bound_type = new int[states - 1];
  frequencies = new double[states];

  for (mt_index_t i=0; i<states-1; ++i)
  {
    lower[i] = MIN_FREQ_RATE;
    upper[i] = MAX_FREQ_RATE;
    bound_type[i] = PLL_LBFGSB_BOUND_BOTH;
  }

  for (mt_index_t i=0; i<states; ++i)
  {
    frequencies[i] = 1.0/states;
  }
}

ParameterFrequenciesOpt::ParameterFrequenciesOpt( ParameterFrequenciesOpt const& other )
  : ParameterFrequencies(states)
{
  states = other.states;
  x = new double[states - 1];
  lower = new double[states - 1];
  upper = new double[states - 1];
  bound_type = new int[states - 1];
  frequencies = new double[states];
  memcpy(x, other.x, sizeof(double) * (states - 1));
  memcpy(lower, other.lower, sizeof(double) * (states - 1));
  memcpy(upper, other.upper, sizeof(double) * (states - 1));
  memcpy(bound_type, other.bound_type, sizeof(int) * (states - 1));
  memcpy(frequencies,
         other.frequencies,
         sizeof(double) * states);
}

ParameterFrequenciesOpt::~ParameterFrequenciesOpt( void )
{
  delete[] x;
  delete[] lower;
  delete[] upper;
  delete[] bound_type;
  delete[] frequencies;
}

bool ParameterFrequenciesOpt::initialize(Partition const& partition)
{
  memcpy(frequencies,
         &partition.get_empirical_frequencies()[0],
         sizeof(double) * states);
  return true;
}

double ParameterFrequenciesOpt::optimize(mt_opt_params_t * params,
                                double loglikelihood,
                                double tolerance,
                                bool first_guess)
{
  UNUSED(first_guess);
  double cur_logl;

  struct custom_params opt_params;
  opt_params.common_params = params;

  unsigned int states = params->partition->states;
  unsigned int n_freqs_free_params = states - 1;
  unsigned int cur_index;
  mt_index_t params_index = 0;

  double * frequencies =
      params->partition->frequencies[params_index];

  /* find highest frequency */
  opt_params.highest_freq_state = 0;
  for (mt_index_t i = 1; i < states; i++)
          if (frequencies[i] > frequencies[opt_params.highest_freq_state])
            opt_params.highest_freq_state = i;

  cur_index = 0;
  for (mt_index_t i = 0; i < states; i++)
  {
    if (i != opt_params.highest_freq_state)
    {
      x[cur_index] = frequencies[i]
          / frequencies[opt_params.highest_freq_state];
      cur_index++;
    }
  }

  cur_logl = pll_minimize_lbfgsb(x, lower, upper, bound_type,
                                 n_freqs_free_params,
                                 1e9, tolerance,
                                 (void *) &opt_params,
                                 &target_func);

  /* update frequencies */
  target_func((void *)&opt_params, x);

  return cur_logl;
}

mt_size_t ParameterFrequenciesOpt::get_n_free_parameters( void ) const
{
  return states - 1;
}

/******************************************************************************/

ParameterFrequenciesFixed::ParameterFrequenciesFixed( mt_size_t states,
                                                      bool equal_frequencies )
  : ParameterFrequencies(states), equal_frequencies(equal_frequencies)
{
  frequencies = new double[states];
  for (mt_index_t i=0; i<states; ++i)
  {
    frequencies[i] = 1.0/states;
  }
}

ParameterFrequenciesFixed::ParameterFrequenciesFixed( ParameterFrequenciesFixed const& other )
  : ParameterFrequencies(states)
{
  states = other.states;
  equal_frequencies = other.equal_frequencies;
  frequencies = new double[states];
  memcpy(frequencies,
         other.frequencies,
         sizeof(double) * states);
}

ParameterFrequenciesFixed::~ParameterFrequenciesFixed( void )
{
  delete[] frequencies;
}

bool ParameterFrequenciesFixed::initialize(Partition const& partition)
{
  if (!equal_frequencies)
  {
    memcpy(frequencies,
           &partition.get_empirical_frequencies()[0],
           sizeof(double) * states);
  }
  return true;
}

double ParameterFrequenciesFixed::optimize(mt_opt_params_t * params,
                                double loglikelihood,
                                double tolerance,
                                bool first_guess)
{
  /* do not optimize */
  return loglikelihood;
}

/* static functions */

static double target_func(void *p, double *x)
{
  struct custom_params * params = (struct custom_params *) p;
  mt_opt_params_t * mt_params = (mt_opt_params_t *) params->common_params;
  pll_partition_t * partition = mt_params->partition;

  mt_index_t highest_freq_state = params->highest_freq_state;
  mt_size_t states = partition->states;

  mt_index_t i;
  mt_index_t cur_index;

  double sum_ratios = 1.0;

  /* update frequencies */
  double *freqs = new double[states];

  for (i = 0; i < (states - 1); ++i)
  {
    assert(x[i] == x[i]);
    sum_ratios += x[i];
  }
  cur_index = 0;
  for (i = 0; i < states; ++i)
  {
    if (i != highest_freq_state)
    {
      freqs[i] = x[cur_index] / sum_ratios;
      cur_index++;
    }
  }
  freqs[highest_freq_state] = 1.0 / sum_ratios;
  pll_set_frequencies (partition,
                       0,
                       freqs);
  delete[] freqs;

  /* compute negative score */
  double score = -1 *
                 pll_utree_compute_lk(mt_params->partition,
                                      mt_params->tree,
                                      mt_params->params_indices,
                                      1,   /* update pmatrices */
                                      1);  /* update partials */
  return score;
}

mt_size_t ParameterFrequenciesFixed::get_n_free_parameters( void ) const
{
  return equal_frequencies?0:(states-1);
}

} /* namespace modeltest */
