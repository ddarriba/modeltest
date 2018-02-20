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

#include "parameter_branches.h"
#include "../partition.h"
#include "../genesis/logging.h"

#define SMOOTHINGS 4

using namespace std;

namespace modeltest
{

ParameterBranches::ParameterBranches( const ParameterBranches & other )
{
  n_branches = other.n_branches;
  name = other.name;
}

ParameterBranches::ParameterBranches( void )
{
  n_branches = 0;
  name = "BranchLengths";
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
  branch_lengths.resize(n_branches);
  return true;
}

struct bl_data_t {
    vector<double> & bl_vector;
    size_t bl_pos;
};

static int cb_extract_branches(pll_unode_t * node, void *data)
{
  struct bl_data_t * bl_data = (struct bl_data_t *) data;
  if(bl_data->bl_pos >= bl_data->bl_vector.size())
      return 0;

  bl_data->bl_vector[bl_data->bl_pos] = node->length;
  ++bl_data->bl_pos;

  return 1;
}

static int cb_reset_branches(pll_unode_t * node, void *data)
{
    struct bl_data_t * bl_data = (struct bl_data_t *) data;
    if(bl_data->bl_pos >= bl_data->bl_vector.size())
        return 0;

    node->length = node->back->length = bl_data->bl_vector[bl_data->bl_pos];
    ++bl_data->bl_pos;

    return 1;
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
                                              params->tree_info->root,
                                              params->params_indices,
                                              1,
                                              1);
  assert(!loglh || fabs(test_loglh - loglh) < 1e-6);
#endif

  struct bl_data_t bl_data = {branch_lengths, 0};
  pllmod_utree_traverse_apply(params->tree_info->root,
                              NULL,
                              NULL,
                              &cb_extract_branches,
                              &bl_data);

  cur_loglh = -1 * pllmod_opt_optimize_branch_lengths_iterative (
             params->partition,
             params->tree_info->root,
             params->params_indices,
             MT_MIN_BRANCH_LENGTH, MT_MAX_BRANCH_LENGTH,
             tolerance, SMOOTHINGS, true);

  if (pll_errno)
  {
      bl_data.bl_pos = 0;
      pllmod_utree_traverse_apply(params->tree_info->root,
                                  NULL,
                                  NULL,
                                  &cb_reset_branches,
                                  &bl_data);
      cur_loglh = pllmod_utree_compute_lk(params->partition,
                                          params->tree_info->root,
                                          params->params_indices,
                                          1,
                                          1);

      LOG_DBG << "[dbg] Reset branches :( " << cur_loglh << " ~= " << loglh << endl;
      assert(fabs(cur_loglh - loglh) < 1e-6);
  }
  else
  {
    LOG_DBG << "[dbg] fix branches " << cur_loglh << endl;
  }

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
