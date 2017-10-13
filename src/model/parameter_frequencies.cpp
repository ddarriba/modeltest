/*
  Copyright (C) 2016 Diego Darriba

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU Affero General Public License as
  published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Affero General Public License for more details.

  You should have received a copy of the GNU Affero General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  Contact: Diego Darriba <Diego.Darriba@h-its.org>,
  Heidelberg Institute for Theoretical Studies,
  Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

#include "parameter_frequencies.h"
#include "../partition.h"

#include <iostream>
#include <iomanip>

#define MIN_FREQ_RATE 0.01
#define MAX_FREQ_RATE 100
#define LBFGSB_FACTOR 1e9

using namespace std;

namespace modeltest
{

ParameterFrequencies::ParameterFrequencies( mt_size_t states ) :
  states(states)
{
  frequencies = 0;
  name = "Frequencies";
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
  : ParameterFrequencies(other.states)
{
  frequencies = new double[states];
  memcpy(frequencies,
         other.frequencies,
         sizeof(double) * states);
}

ParameterFrequenciesOpt::~ParameterFrequenciesOpt( void )
{
  delete[] frequencies;
}

bool ParameterFrequenciesOpt::initialize(mt_opt_params_t * params,
                                         Partition const& partition)
{
  memcpy(frequencies,
         &partition.get_empirical_frequencies()[0],
         sizeof(double) * states);

  pll_set_frequencies(params->partition, 0, frequencies);

  return true;
}

double ParameterFrequenciesOpt::optimize(mt_opt_params_t * params,
                                double loglh,
                                double tolerance,
                                bool first_guess)
{
  UNUSED(first_guess);
  double cur_loglh;

  cur_loglh = -1 * pllmod_algo_opt_frequencies(params->partition,
                                         params->tree,
                                         0,
                                         params->params_indices,
                                         LBFGSB_FACTOR,
                                         tolerance);

  assert(!loglh || (cur_loglh - loglh)/loglh < 1e-10);

  memcpy(frequencies,
         params->partition->frequencies[0],
         sizeof(double) * states);

  return cur_loglh;
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
  : ParameterFrequencies(other.states)
{
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

bool ParameterFrequenciesFixed::initialize(mt_opt_params_t * params,
                                           Partition const& partition)
{
  if (!equal_frequencies)
  {
    memcpy(frequencies,
           &partition.get_empirical_frequencies()[0],
           sizeof(double) * states);
  }

  pll_set_frequencies(params->partition, 0, frequencies);

  return true;
}

double ParameterFrequenciesFixed::optimize(mt_opt_params_t * params,
                                           double loglh,
                                           double tolerance,
                                           bool first_guess)
{
  UNUSED(params);
  UNUSED(tolerance);
  UNUSED(first_guess);

  /* do not optimize */
  return loglh;
}

mt_size_t ParameterFrequenciesFixed::get_n_free_parameters( void ) const
{
  return equal_frequencies?0:(states-1);
}

} /* namespace modeltest */
