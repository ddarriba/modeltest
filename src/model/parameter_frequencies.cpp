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
#include "../thread/parallel_context.h"

#include <iostream>
#include <iomanip>

#define MIN_FREQ_RATE 0.001
#define MAX_FREQ_RATE 100
#define LBFGSB_FACTOR 1e9

using namespace std;

namespace modeltest
{

static const double equal_dna_freqs[4] = { 0.25, 0.25, 0.25, 0.25 };

ParameterFrequencies::ParameterFrequencies( mt_size_t states, mt_size_t freq_set_count ) :
  states(states),
  freq_set_count(freq_set_count)
{
  assert(freq_set_count > 0);
  char_id = 'f';
}

ParameterFrequencies::~ParameterFrequencies( void )
{
}

void ParameterFrequencies::print(std::ostream  &out,
                                bool line_break,
                                int indent_first,
                                int spacing) const
{
  if (indent_first && spacing > 0)
    out << setw(spacing) << " ";
  for (mt_index_t j=0; j<freq_set_count; ++j)
    {
    const double * freqs = get_frequencies(j);

      if (j > 0)
      {
        if (line_break)
          out << endl;
        if (spacing > 0)
          out << setw(spacing) << " ";
      }
      for (mt_index_t i=0; i<states; ++i)
      {
//        if (i == 10 && line_break)
//        {
//          out << endl;
//          if (spacing > 0)
//            out << setw(spacing) << " ";
//        }
//        else
          if (i > 0)
          out << " ";
        out << setprecision(MT_PRECISION_DIGITS) << freqs[i];
      }
    }
}

/******************************************************************************/

ParameterFrequenciesOpt::ParameterFrequenciesOpt( mt_size_t states )
  : ParameterFrequencies(states)
{
  frequencies = new double*[freq_set_count];

  for (mt_index_t j = 0; j < (mt_size_t) freq_set_count; ++j)
  {
    frequencies[j] = new double[states];
    for (mt_index_t i = 0; i < states; ++i)
    {
      frequencies[j][i] = 1.0 / states;
    }
  }

  name = "OptFrequencies";
}

ParameterFrequenciesOpt::ParameterFrequenciesOpt( ParameterFrequenciesOpt const& other )
  : ParameterFrequencies(other.states, other.freq_set_count)
{
  for (mt_index_t j=0; j<freq_set_count; ++j)
    memcpy(frequencies[j],
           other.frequencies[j],
           sizeof(double) * states);
}

ParameterFrequenciesOpt::~ParameterFrequenciesOpt( void )
{
  for (mt_index_t j=0; j<(mt_size_t)freq_set_count; ++j)
    delete[] frequencies[j];
  delete[] frequencies;
}

bool ParameterFrequenciesOpt::initialize(mt_opt_params_t * params,
                                         Partition const& partition)
{
  for (mt_index_t j=0; j<freq_set_count; ++j)
  {
    memcpy(frequencies[j],
           &partition.get_empirical_frequencies()[0],
           sizeof(double) * states);

    /* if there are missing states, set initial freqs to equal */
    for (mt_index_t i=0; i<states; ++i)
    {
      if (frequencies[j][i] <= 1e-7)
      {
        for (mt_index_t k=0; k<states; ++k)
          frequencies[j][k] = 1.0/states;
        break;
      }
    }

    pll_set_frequencies(params->partition, j, frequencies[j]);
  }

  return true;
}

double ParameterFrequenciesOpt::optimize(mt_opt_params_t * params,
                                double loglh,
                                double tolerance,
                                bool first_guess)
{
  UNUSED(first_guess);
  double cur_loglh;

  /* optimize only a single set of frequencies */
  assert(freq_set_count == 1);

  cur_loglh = -1 * pllmod_algo_opt_frequencies_treeinfo(params->tree_info,
                                                        0,
                                                        MIN_FREQ_RATE,
                                                        MAX_FREQ_RATE,
                                                        LBFGSB_FACTOR,
                                                        tolerance);

  if (loglh && (cur_loglh - loglh)/loglh > 1e-10)
  {
    /* revert */
    memcpy(params->partition->frequencies[0],
           frequencies[0],
           sizeof(double) * states);

    pll_set_frequencies(params->partition, 0, frequencies[0]);

    cur_loglh = pllmod_treeinfo_compute_loglh(params->tree_info, 0);
  }

  assert(!loglh || (cur_loglh - loglh)/loglh < 1e-10);

  memcpy(frequencies[0],
         params->partition->frequencies[0],
         sizeof(double) * states);

  return cur_loglh;
}

mt_size_t ParameterFrequenciesOpt::get_n_free_parameters( void ) const
{
  return freq_set_count * (states - 1);
}

const double * ParameterFrequenciesOpt::get_frequencies( mt_index_t freq_set_index ) const
{
  return frequencies[freq_set_index];
}

void ParameterFrequenciesOpt::set_frequencies(const double value[], mt_index_t freq_set_index)
{
  assert(frequencies);
  memcpy(frequencies[freq_set_index], value, states * sizeof(double));
}

void ParameterFrequenciesOpt::set_frequencies(const vector<double> & value, mt_index_t freq_set_index)
{
  assert(frequencies);
  memcpy(frequencies[freq_set_index], &(value[0]), states * sizeof(double));
}

/******************************************************************************/

ParameterFrequenciesFixed::ParameterFrequenciesFixed( mt_size_t states,
                                                      freqs_type_t freqs_type,
                                                      mt_size_t freq_set_count)
  : ParameterFrequencies(states, freq_set_count),
    freqs_type(freqs_type)
{
  fixed_frequencies = new const double*[freq_set_count]();

  if (freqs_type == freqs_equal)
  {
    assert (states == 4);
    for (mt_index_t j=0; j<freq_set_count; ++j)
      fixed_frequencies[j] = equal_dna_freqs;
  }
  name = "FixedFrequencies";
}

ParameterFrequenciesFixed::ParameterFrequenciesFixed( ParameterFrequenciesFixed const& other )
  : ParameterFrequencies(other.states, other.freq_set_count)
{
  freqs_type = other.freqs_type;
  fixed_frequencies = new const double*[freq_set_count];
  for (mt_index_t j=0; j<freq_set_count; ++j)
    fixed_frequencies[j] = other.fixed_frequencies[j];
}

ParameterFrequenciesFixed::~ParameterFrequenciesFixed( void )
{
  delete[] fixed_frequencies;
}

bool ParameterFrequenciesFixed::initialize(mt_opt_params_t * params,
                                           Partition const& partition)
{
  for (mt_index_t j=0; j<freq_set_count; ++j)
  {
    if (freqs_type == freqs_empirical)
    {
      fixed_frequencies[j] = &partition.get_empirical_frequencies()[0];
    }

    pll_set_frequencies(params->partition, j, fixed_frequencies[j]);
  }

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
  return freqs_type == freqs_empirical?(freq_set_count * (states-1)):0;
}

const double * ParameterFrequenciesFixed::get_frequencies( mt_index_t freq_set_index ) const
{
  return fixed_frequencies[freq_set_index];
}

void ParameterFrequenciesFixed::set_frequencies(const double value[], mt_index_t freq_set_index)
{
  assert(fixed_frequencies);
  fixed_frequencies[freq_set_index] = value;
}

void ParameterFrequenciesFixed::set_frequencies(const vector<double> & value, mt_index_t freq_set_index)
{
  assert(fixed_frequencies);
  fixed_frequencies[freq_set_index] = &(value[0]);
}

} /* namespace modeltest */
