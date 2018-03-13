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

#include "abstract_parameter.h"

namespace modeltest
{
  double AbstractParameter::cb_compute_negative_loglikelihood(void *p)
  {
    mt_opt_params_t * params = (mt_opt_params_t *) p;
    double score = -1
          * pll_compute_edge_loglikelihood (
              params->partition,
              params->tree_info->root->clv_index,
              params->tree_info->root->scaler_index,
              params->tree_info->root->back->clv_index,
              params->tree_info->root->back->scaler_index,
              params->tree_info->root->pmatrix_index,
              params->params_indices,
              NULL);

    return score;
  }

  const std::string & AbstractParameter::get_name( void ) const
  {
      return name;
  }

  char AbstractParameter::get_char_id( void ) const
  {
    return char_id;
  }
}
