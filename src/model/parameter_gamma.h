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

#ifndef PARAMETER_GAMMA_H
#define PARAMETER_GAMMA_H

#include "parameter_ratecats.h"

namespace modeltest
{

class ParameterGamma : public ParameterRateCats
{
public:
  ParameterGamma(mt_size_t n_cats, double alpha = 1.0);
  ParameterGamma( const ParameterGamma & other );
  ~ParameterGamma( void );
  virtual bool initialize(mt_opt_params_t * params,
                          Partition const& partition);
  virtual double optimize(mt_opt_params_t * params,
                          double loglh,
                          double tolerance = DEFAULT_PARAM_EPSILON,
                          bool first_guess = false);
  virtual void print(std::ostream  &out = std::cout,
                     bool line_break = false,
                     int indent_first = false,
                     int spacing = 0) const;
  virtual double get_alpha( void ) const;
  virtual void set_alpha( double alpha );

  virtual mt_size_t get_n_free_parameters( void ) const;
private:
  double alpha;
};

} /* namespace modeltest */

#endif
