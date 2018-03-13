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

#ifndef DNAMODEL_H
#define DNAMODEL_H

#include "model.h"

namespace modeltest {

class DnaModel : public Model
{
public:
    DnaModel(mt_index_t matrix_index,
             mt_mask_t model_params,
             const partition_descriptor_t &partition,
             asc_bias_t asc_bias_corr = asc_none,
             const mt_size_t *asc_w = 0);
    DnaModel(const Model &other);
    virtual ~DnaModel( void );

    static mt_index_t get_index_for_matrix(const int * matrix);

    virtual void clone(const Model *other);

    virtual data_type_t get_datatype( void ) const
    {
        return dt_dna;
    }

    /**
     * @brief Get the matrix symmetries
     * @return the rate matrix symmetries
     */
    virtual const int * get_symmetries( void ) const;

    /* extended */
    virtual pll_partition_t * build_partition( mt_size_t n_tips,
                                               mt_size_t n_sites);
    virtual void print(std::ostream  &out = std::cout);
    virtual void print_xml(std::ostream  &out = std::cout);
    virtual void output_log(std::ostream  &out) const;
    virtual void input_log(std::istream  &in);
    virtual int output_bin(std::string const& bin_filename) const;
    virtual int input_bin(std::string const& bin_filename);

private:
  int *matrix_symmetries; //! The DNA matrix symmetries
};

}
#endif // DNAMODEL_H
