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

#ifndef ABSTRACT_PARAMETER_H
#define ABSTRACT_PARAMETER_H

#include "../plldefs.h"
#include "../global_defs.h"

#include <iostream>

namespace modeltest
{

class Partition;

typedef struct
{
    pll_partition_t * partition;
    pllmod_treeinfo_t * tree_info;
    unsigned int * params_indices;
    int gamma_rates_mode;
} mt_opt_params_t;

class AbstractParameter
{
public:
  virtual ~AbstractParameter() {}
  virtual bool initialize(mt_opt_params_t * params,
                          Partition const& partition) = 0;
  virtual double optimize(mt_opt_params_t * params,
                          double loglh,
                          double tolerance = DEFAULT_PARAM_EPSILON,
                          bool first_guess = false) = 0;

  virtual void print(std::ostream  &out = std::cout,
                     bool line_break = false,
                     int indent_first = false,
                     int spacing = 0) const = 0;
  virtual mt_size_t get_n_free_parameters( void ) const = 0;

  const std::string & get_name( void ) const;
  char get_char_id( void ) const;
protected:
  char char_id;
  static double cb_compute_negative_loglikelihood(void *p);
  std::string name;
};

} /* namespace modeltest */
#endif
