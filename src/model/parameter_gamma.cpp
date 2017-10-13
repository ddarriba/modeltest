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

#include "parameter_gamma.h"

#define MIN_ALPHA 0.02
#define MAX_ALPHA 100

using namespace std;

namespace modeltest
{

ParameterGamma::ParameterGamma(mt_size_t n_cats, double alpha)
  : ParameterRateCats(n_cats), alpha(alpha)
{
  alpha = 1.0;
  name = "Alpha";
}

ParameterGamma::ParameterGamma( const ParameterGamma & other )
  : ParameterRateCats(other.n_cats)
{
  alpha = other.alpha;
  name = other.name;
}

ParameterGamma::~ParameterGamma( void )
{

}

bool ParameterGamma::initialize(mt_opt_params_t * params,
                                Partition const& partition)
{
  UNUSED(partition);

  assert(n_cats == params->partition->rate_cats);
  if (!pll_compute_gamma_cats (alpha,
                          n_cats,
                          rates,
                          PLL_GAMMA_RATES_MEAN))
    assert(0);
  pll_set_category_rates(params->partition,
                         rates);
  return true;
}

double ParameterGamma::optimize(mt_opt_params_t * params,
                                double loglh,
                                double tolerance,
                                bool first_guess)
{
  double cur_loglh;
  //double alpha_prev;
  UNUSED(first_guess);

  if (n_cats == 1)
    return loglh;

  //alpha_prev = alpha;
  cur_loglh = -1 * pllmod_algo_opt_alpha(params->partition,
                                    params->tree,
                                    params->params_indices,
                                    MIN_ALPHA,
                                    MAX_ALPHA,
                                    &alpha,
                                    tolerance);

  assert(!loglh || (cur_loglh - loglh)/loglh <= 1e-10);
  extract_rates_and_weights(params);

  return cur_loglh;
}

void ParameterGamma::print(std::ostream  &out) const
{
  UNUSED(out);
}

double ParameterGamma::get_alpha( void ) const
{
  return alpha;
}

void ParameterGamma::set_alpha( double _alpha )
{
  alpha = _alpha;
}

mt_size_t ParameterGamma::get_n_free_parameters( void ) const
{
  return (n_cats != 1)?1:0;
}

} /* namespace modeltest */
