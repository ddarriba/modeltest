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

#include "parameter_pinv.h"
#include "../partition.h"
#include "../thread/parallel_context.h"

#define MIN_PINV 0.02
#define MAX_PINV 0.95

using namespace std;

namespace modeltest
{

ParameterPinv::ParameterPinv( void )
{
  pinv = 0.5;
  min_pinv = MIN_PINV;
  max_pinv = MAX_PINV;
  name = "P-inv";
  char_id = 'i';
}

ParameterPinv::ParameterPinv( const ParameterPinv & other )
{
  pinv = other.pinv;
  max_pinv = other.max_pinv;
  min_pinv = other.min_pinv;
  name = other.name;
  char_id = other.char_id;
}

ParameterPinv::~ParameterPinv( void )
{

}

bool ParameterPinv::initialize(mt_opt_params_t * params,
                               Partition const& partition)
{
  if (ParallelContext::master_thread())
  {
    max_pinv = partition.get_empirical_pinv();
    min_pinv = MIN_PINV;
    if (max_pinv > 2*min_pinv)
    {
      pinv = (min_pinv + max_pinv) / 2;
    }
    else
    {
      pinv = MIN_PINV/2;
      max_pinv = max(MIN_PINV, max_pinv);
      min_pinv = 1E-6;
    }
  }

  ParallelContext::thread_barrier();
  
  for (mt_index_t i=0; i<params->partition->rate_cats; ++i)
    pll_update_invariant_sites_proportion(params->partition,
                                          params->params_indices[i],
                                          pinv);

  return true;
}

double ParameterPinv::optimize(mt_opt_params_t * params,
                               double loglh,
                               double tolerance,
                               bool first_guess)
{
  UNUSED(first_guess);
  double cur_loglh;

  params->tree_info->partitions[0]->prop_invar[0] = params->partition->prop_invar[0];

  ParallelContext::thread_barrier();

  cur_loglh =  -1 * pllmod_algo_opt_onedim_treeinfo(params->tree_info,
                                                    PLLMOD_OPT_PARAM_PINV,
                                                    min_pinv,
                                                    max_pinv,
                                                    tolerance);

  pinv = params->tree_info->partitions[0]->prop_invar[0];

  if (isinf(cur_loglh))
  {
    if (pll_errno == PLLMOD_OPT_ERROR_BRENT_INIT)
    {
      assert(fabs(pinv - params->partition->prop_invar[0]) < 1e-8);
      cur_loglh = loglh;
    }
  }

  assert(!isinf(cur_loglh));
  assert(!loglh || (cur_loglh - loglh)/loglh < 1e-10);

  return cur_loglh;
}

void ParameterPinv::print(std::ostream  &out,
                        bool line_break,
                        int indent_first,
                        int spacing) const
{
  UNUSED(line_break);

  if (indent_first && spacing > 0)
    out << setw(spacing) << " ";
  out << setprecision(MT_PRECISION_DIGITS) << pinv;
}

double ParameterPinv::get_pinv() const
{
  return pinv;
}

void ParameterPinv::set_pinv( double value )
{
  pinv = value;
}

mt_size_t ParameterPinv::get_n_free_parameters( void ) const
{
  return 1;
}

} /* namespace modeltest */
