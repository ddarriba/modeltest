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

#include "parameter_substrates.h"
#include "../partition.h"
#include "../thread/parallel_context.h"

#define MIN_RATE 0.02
#define MAX_RATE 100
#define LBFGSB_FACTOR 1e9

using namespace std;

namespace modeltest
{

ParameterSubstRates::ParameterSubstRates(mt_size_t n_subst_params, mt_size_t rate_set_count)
  : n_subst_params(n_subst_params), rate_set_count(rate_set_count)
{
  n_subst_free_params = 0;
  char_id = 's';
}

bool ParameterSubstRates::initialize(mt_opt_params_t * params,
                                     Partition const& partition)
{
  UNUSED(params);
  UNUSED(partition);

  return true;
}

mt_size_t ParameterSubstRates::get_n_subst_params( void ) const
{
  return n_subst_params;
}

mt_size_t ParameterSubstRates::get_n_free_parameters( void ) const
{
  return n_subst_free_params;
}

void ParameterSubstRates::print(std::ostream  &out,
                                bool line_break,
                                int indent_first,
                                int spacing) const
{
  if (indent_first && spacing > 0)
    out << setw(spacing) << " ";
  for (mt_index_t j=0; j<rate_set_count; ++j)
    {
    const double * rates = get_subst_rates(j);

      if (j > 0)
      {
        if (line_break)
          out << endl << endl;
        if (spacing > 0)
          out << setw(spacing) << " ";
      }
      for (mt_index_t i=0; i<n_subst_params; ++i)
      {
        if (i > 0)
          out << ",";
        out << rates[i];
      }
    }
}

/******************************************************************************/
/* FIXED RATES */
/******************************************************************************/

ParameterSubstRatesFixed::ParameterSubstRatesFixed(const double * subst_rates, mt_size_t n_subst_rates)
  : ParameterSubstRates(n_subst_rates, 1)
{
  const_subst_rates = new const double *[rate_set_count];
  const_subst_rates[0] = subst_rates;

  n_subst_free_params = 0;

  name = "FixedSubstRates";
}

ParameterSubstRatesFixed::ParameterSubstRatesFixed(mt_size_t n_subst_rates, int rate_set_count)
  : ParameterSubstRates(n_subst_rates, rate_set_count)
{
  const_subst_rates = new const double *[rate_set_count];

  n_subst_free_params = 0;

  name = "FixedSubstRates";
}

ParameterSubstRatesFixed::ParameterSubstRatesFixed( const ParameterSubstRatesFixed & other )
  : ParameterSubstRates(other.n_subst_params, other.rate_set_count)
{
  const_subst_rates = new const double *[rate_set_count];
  for (mt_index_t i=0; i<rate_set_count; ++i)
    const_subst_rates[i] = other.const_subst_rates[i];
  n_subst_free_params = other.n_subst_free_params;
  name = other.name;
}

ParameterSubstRatesFixed::~ParameterSubstRatesFixed( void )
{
  delete[] const_subst_rates;
}

bool ParameterSubstRatesFixed::initialize(mt_opt_params_t * params,
                                     Partition const& partition)
{
  UNUSED(partition);

  for (mt_index_t i=0; i<rate_set_count; ++i)
    pll_set_subst_params(params->partition, i, const_subst_rates[i]);

  return true;
}

double ParameterSubstRatesFixed::optimize(mt_opt_params_t * params,
                                double loglh,
                                double tolerance,
                                bool first_guess)
{
  UNUSED(first_guess);
  UNUSED(tolerance);
  UNUSED(params);

  return loglh;
}

const double * ParameterSubstRatesFixed::get_subst_rates( int rate_set_index ) const
{
  return const_subst_rates[rate_set_index];
}

void ParameterSubstRatesFixed::set_subst_rates(const double * values, int rate_set_index)
{
  const_subst_rates[rate_set_index] = values;
}

/******************************************************************************/
/* OPTIMIZABLE RATES (defined symmetries) */
/******************************************************************************/

ParameterSubstRatesOpt::ParameterSubstRatesOpt(vector<int> symmetries)
  : ParameterSubstRates(symmetries.size(), 1),
    symmetries(symmetries)
{
  subst_rates = new double *[1];

  subst_rates[0] = new double[n_subst_params];
  for (mt_index_t i=0; i<n_subst_params; ++i)
  {
    subst_rates[0][i] = 1.0;
    if (symmetries[i] > (int)n_subst_free_params)
      n_subst_free_params = (int)symmetries[i];
  }

  name = "SubstRates";
}

ParameterSubstRatesOpt::ParameterSubstRatesOpt(const int * a_symmetries,
                                               mt_size_t n_subst_params)
  : ParameterSubstRates(n_subst_params, 1)
{
  symmetries.resize(n_subst_params);
  n_subst_free_params = 0;
  for (mt_index_t i=0; i<n_subst_params; ++i)
  {
    symmetries[i] = a_symmetries[i];
    if (symmetries[i] > (int) n_subst_free_params)
      n_subst_free_params = symmetries[i];
  }
  subst_rates = new double *[1];
  subst_rates[0] = new double[n_subst_params];
  for (mt_index_t i=0; i<n_subst_params; ++i)
    subst_rates[0][i] = 1.0;

  name = "SubstRates";
}

ParameterSubstRatesOpt::ParameterSubstRatesOpt( const ParameterSubstRatesOpt & other )
  : ParameterSubstRates(other.n_subst_params, other.rate_set_count)
{
  symmetries = other.symmetries;
  n_subst_params = other.n_subst_params;
  n_subst_free_params = other.n_subst_free_params;
  subst_rates = new double *[rate_set_count];
  for (mt_index_t j=0; j<rate_set_count; ++j)
  {
    subst_rates[j] = new double[n_subst_params];
    memcpy(subst_rates[j], other.subst_rates[j], sizeof(double)*n_subst_params);
  }
  name = other.name;
}

ParameterSubstRatesOpt::~ParameterSubstRatesOpt( void )
{
  for (mt_index_t j=0; j<rate_set_count; ++j)
    delete[] subst_rates[j];
  delete[] subst_rates;
}

bool ParameterSubstRatesOpt::initialize(mt_opt_params_t * params,
                                     Partition const& partition)
{
    const vector<double> empirical_rates = partition.get_empirical_subst_rates();
    assert(empirical_rates.size() == n_subst_params);

    if (ParallelContext::master_thread())
    {
      for (mt_index_t k=0; k<rate_set_count; ++k)
      {
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
                    subst_rates[k][j] = sum_rate;
        }

        for (mt_index_t i=0; i<n_subst_params; ++i)
        {
            subst_rates[k][i] /= subst_rates[k][n_subst_params-1];

            /* validate boundaries */
            if (subst_rates[k][i] < MIN_RATE)
              subst_rates[k][i] = MIN_RATE;
            else if (subst_rates[k][i] > MAX_RATE)
              subst_rates[k][i] = MAX_RATE;
        }
      }
    }
    
  ParallelContext::thread_barrier();

  for (mt_index_t k=0; k<rate_set_count; ++k)
    pll_set_subst_params(params->partition, k, subst_rates[k]);

  return true;
}

double ParameterSubstRatesOpt::optimize(mt_opt_params_t * params,
                                double loglh,
                                double tolerance,
                                bool first_guess)
{
  UNUSED(first_guess);
  double cur_loglh;

  if (!n_subst_free_params)
    return loglh;

  assert(rate_set_count == 1);

  cur_loglh = -1 * pllmod_algo_opt_subst_rates_treeinfo(params->tree_info,
                                                        0,
                                                        MIN_RATE,
                                                        MAX_RATE,
                                                        LBFGSB_FACTOR,
                                                        tolerance);

  if (loglh && (cur_loglh - loglh)/loglh > 1e-10)
  {
    ParallelContext::thread_barrier();

    /* revert */
    memcpy(params->partition->subst_params[0],
           subst_rates[0],
           n_subst_params * sizeof(double));

    pll_set_subst_params(params->partition, 0, subst_rates[0]);

    cur_loglh = pllmod_treeinfo_compute_loglh(params->tree_info, 0);
  }

  assert(!loglh || (cur_loglh - loglh)/loglh < 1e-10);

  memcpy(subst_rates[0],
         params->partition->subst_params[0],
         n_subst_params * sizeof(double));

  return cur_loglh;
}

const double * ParameterSubstRatesOpt::get_subst_rates( int rate_set_index ) const
{
  return subst_rates[rate_set_index];
}

void ParameterSubstRatesOpt::set_subst_rates(const double * values, int rate_set_index)
{
  memcpy(subst_rates[rate_set_index], values, n_subst_params * sizeof(double));
}

} /* namespace modeltest */
