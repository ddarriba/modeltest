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

#ifndef PARAMETER_FREQUENCIES_H
#define PARAMETER_FREQUENCIES_H

#define PARAM_FREQUENCIES_EQUAL      1
#define PARAM_FREQUENCIES_EMPIRICAL  2
#define PARAM_FREQUENCIES_PREDEFINED 3

#include "abstract_parameter.h"
#include "../model_defs.h"

#include <vector>

namespace modeltest
{

class ParameterFrequencies : public AbstractParameter
{
public:
  ParameterFrequencies( mt_size_t states,
                        mt_size_t freq_set_count = 1);
  ~ParameterFrequencies( void );

  /**
   * @brief Gets the model state frequencies
   * @return the state frequencies
   */
  virtual const double * get_frequencies( mt_index_t freq_set_index = 0 ) const = 0;

  /**
   * @brief Sets the model state frequencies
   * @param[in] value the model state frequencies
   */
  virtual void set_frequencies(const double value[], mt_index_t freq_set_index = 0) = 0;
  virtual void set_frequencies(const std::vector<double> & value, mt_index_t freq_set_index = 0) = 0;

  virtual void print(std::ostream  &out = std::cout,
                     bool line_break = false,
                     int indent_first = false,
                     int spacing = 0) const;

protected:
  mt_size_t states;
  mt_size_t freq_set_count;
};

class ParameterFrequenciesOpt : public ParameterFrequencies
{
public:
  ParameterFrequenciesOpt( mt_size_t states );
  ParameterFrequenciesOpt( ParameterFrequenciesOpt const& other );
  ~ParameterFrequenciesOpt( void );
  virtual bool initialize(mt_opt_params_t * params,
                          Partition const& partition);
  virtual double optimize(mt_opt_params_t * params,
                          double loglh,
                          double tolerance = DEFAULT_PARAM_EPSILON,
                          bool first_guess = false);
  virtual mt_size_t get_n_free_parameters( void ) const;

  virtual const double * get_frequencies( mt_index_t freq_set_index = 0 ) const;
  virtual void set_frequencies(const double value[], mt_index_t freq_set_index = 0);
  virtual void set_frequencies(const std::vector<double> & value, mt_index_t freq_set_index = 0);

private:
  double ** frequencies;
};

class ParameterFrequenciesFixed : public ParameterFrequencies
{
public:
  ParameterFrequenciesFixed( mt_size_t states,
                             freqs_type_t freqs_type,
                             mt_size_t freq_set_count = 1);
  ParameterFrequenciesFixed( ParameterFrequenciesFixed const& other );
  ~ParameterFrequenciesFixed( void );
  virtual bool initialize(mt_opt_params_t * params,
                          Partition const& partition);
  virtual double optimize(mt_opt_params_t * params,
                          double loglh,
                          double tolerance = DEFAULT_PARAM_EPSILON,
                          bool first_guess = false);
  virtual mt_size_t get_n_free_parameters( void ) const;

  virtual const double * get_frequencies( mt_index_t freq_set_index = 0 ) const;
  virtual void set_frequencies(const double value[], mt_index_t freq_set_index = 0);
  virtual void set_frequencies(const std::vector<double> & value, mt_index_t freq_set_index = 0);

private:
    const double ** fixed_frequencies;
    freqs_type_t freqs_type;
};

} /* namespace modeltest */

#endif
