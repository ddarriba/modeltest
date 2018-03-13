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

#ifndef PARAMETER_SUBSTRATES_H
#define PARAMETER_SUBSTRATES_H

#include "abstract_parameter.h"

#include <vector>

namespace modeltest
{

class ParameterSubstRates : public AbstractParameter
{
public:

  ParameterSubstRates(mt_size_t n_subst_params, mt_size_t rate_set_count);

  virtual bool initialize(mt_opt_params_t * params,
                          Partition const& partition);

  virtual mt_size_t get_n_subst_params( void ) const;
  virtual mt_size_t get_n_free_parameters( void ) const;

  virtual const double * get_subst_rates( int rate_set_index = 0 ) const = 0;
  virtual void set_subst_rates(const double * values, int rate_set_index = 0) = 0;

  virtual void print(std::ostream  &out = std::cout,
                     bool line_break = false,
                     int indent_first = false,
                     int spacing = 0) const;
protected:
  mt_size_t n_subst_free_params;
  mt_size_t n_subst_params;
  mt_size_t rate_set_count;
};

class ParameterSubstRatesFixed : public ParameterSubstRates
{
public:
  ParameterSubstRatesFixed(const double * subst_rates, mt_size_t n_subst_rates);
  ParameterSubstRatesFixed(mt_size_t n_subst_rates, int rate_set_count);
  ParameterSubstRatesFixed(const ParameterSubstRatesFixed & other);
  ~ParameterSubstRatesFixed(void);
  virtual bool initialize(mt_opt_params_t * params,
                          Partition const& partition);
  virtual double optimize(mt_opt_params_t * params,
                          double loglh,
                          double tolerance = DEFAULT_PARAM_EPSILON,
                          bool first_guess = false);

  virtual const double * get_subst_rates( int rate_set_index = 0 ) const;
  virtual void set_subst_rates(const double * values, int rate_set_index = 0);

protected:
  const double ** const_subst_rates;
};

class ParameterSubstRatesOpt : public ParameterSubstRates
{
public:
  ParameterSubstRatesOpt(std::vector<int> symmetries);
  ParameterSubstRatesOpt(const ParameterSubstRatesOpt & other);
  ParameterSubstRatesOpt(const int * symmetries, mt_size_t n_subst_params);
  ~ParameterSubstRatesOpt(void);
  virtual bool initialize(mt_opt_params_t * params,
                          Partition const& partition);
  virtual double optimize(mt_opt_params_t * params,
                          double loglh,
                          double tolerance = DEFAULT_PARAM_EPSILON,
                          bool first_guess = false);

  virtual const double * get_subst_rates( int rate_set_index = 0 ) const;
  virtual void set_subst_rates(const double * values, int rate_set_index = 0);

protected:
  std::vector<int> symmetries;
  double ** subst_rates;
};

} /* namespace modeltest */

#endif
