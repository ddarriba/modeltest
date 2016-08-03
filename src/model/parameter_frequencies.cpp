#include "parameter_frequencies.h"
#include "partition.h"

#include <iostream>
#include <iomanip>

#define MIN_FREQ_RATE 0.01
#define MAX_FREQ_RATE 100

using namespace std;

namespace modeltest
{

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
  frequencies = new double[states];

  for (mt_index_t i=0; i<states; ++i)
  {
    frequencies[i] = 1.0/states;
  }
}

ParameterFrequenciesOpt::ParameterFrequenciesOpt( ParameterFrequenciesOpt const& other )
  : ParameterFrequencies(states)
{
  states = other.states;
  frequencies = new double[states];
  memcpy(frequencies,
         other.frequencies,
         sizeof(double) * states);
}

ParameterFrequenciesOpt::~ParameterFrequenciesOpt( void )
{
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

  cur_logl = pllmod_algo_opt_frequencies(params->partition,
                                         params->tree,
                                         0,
                                         params->params_indices,
                                         tolerance);

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

mt_size_t ParameterFrequenciesFixed::get_n_free_parameters( void ) const
{
  return equal_frequencies?0:(states-1);
}

} /* namespace modeltest */
