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

#include "partition.h"
#include "parameter_branches.h"

#define MIN_BL 1e-4
#define MAX_BL 5
#define SMOOTHINGS 4

using namespace std;

namespace modeltest
{

ParameterBranches::ParameterBranches( const ParameterBranches & other )
{
  n_branches = other.n_branches;
}

ParameterBranches::ParameterBranches( void )
{
  n_branches = 0;
}

ParameterBranches::~ParameterBranches( void )
{
}

bool ParameterBranches::initialize(mt_opt_params_t * params,
                                   Partition const& partition )
{
  UNUSED(params);
  UNUSED(partition);
  n_branches = partition.get_n_sequences()*2 - 3;
  return true;
}

double ParameterBranches::optimize(mt_opt_params_t * params,
                                double loglh,
                                double tolerance,
                                bool first_guess)
{
  UNUSED(first_guess);
  double cur_loglh;

#ifdef DEBUG
  /* verify that matrices and partials where up to date */
  double test_loglh = pllmod_utree_compute_lk(params->partition,
                                              params->tree,
                                              params->params_indices,
                                              1,
                                              1);
  assert(fabs(test_loglh - loglh) < 1e-6);
#endif

  cur_loglh = pllmod_opt_optimize_branch_lengths_iterative (
             params->partition,
             params->tree,
             params->params_indices,
             MIN_BL, MAX_BL,
             tolerance, SMOOTHINGS, true);

  assert(!loglh || (cur_loglh - loglh)/loglh < 1e-10);

  return cur_loglh;
}

void ParameterBranches::print(std::ostream  &out) const
{
  UNUSED(out);
}

mt_size_t ParameterBranches::get_n_free_parameters( void ) const
{
  return n_branches;
}

} /* namespace modeltest */
