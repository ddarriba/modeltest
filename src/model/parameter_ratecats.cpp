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

#include "model/parameter_ratecats.h"

#define MIN_RATE 0.02
#define MAX_RATE 2
#define LBFGSB_FACTOR 1e9

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
                                double loglh,
                                double tolerance,
                                bool first_guess)
{
  UNUSED(first_guess);
  double cur_loglh;
  double branch_scaler;

  cur_loglh = pllmod_algo_opt_rates_weights(params->partition,
                                            params->tree,
                                            params->params_indices,
                                            0.01, 10,
                                            LBFGSB_FACTOR,
                                            tolerance,
                                            &branch_scaler,
                                            false);

  assert(!loglh || (cur_loglh - loglh)/loglh < 1e-10);
  extract_rates_and_weights(params);

  return cur_loglh;

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
