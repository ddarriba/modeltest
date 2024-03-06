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

#ifndef PROTMODEL_H
#define PROTMODEL_H

#include "model.h"

namespace modeltest {

class ProtModel : public Model
{
public:
    ProtModel(mt_index_t matrix_index,
              mt_mask_t model_params,
              const partition_descriptor_t &partition,
              asc_bias_t asc_bias_corr = asc_none,
              const mt_size_t *asc_w = 0);
    virtual ~ProtModel( void );

    virtual data_type_t get_datatype( void ) const
    {
        return dt_protein;
    }

    /* extended */
    virtual mt_size_t get_n_subst_params( void ) const;
    virtual void print(std::ostream  &out = std::cout);
    virtual void print_xml(std::ostream  &out = std::cout);
    virtual void output_log(std::ostream  &out) const;
    virtual void input_log(std::istream  &in);
};

}
#endif // PROTMODEL_H
