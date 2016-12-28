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

#ifndef MSAPLL_H_
#define MSAPLL_H_

#include "msa.h"
#include "plldefs.h"

namespace modeltest
{

  class MsaPll : public Msa
  {
  public:
    MsaPll (std::string msa_filename, mt_size_t n_taxa);        /* fasta   */
    MsaPll (std::string msa_filename, pll_msa_t * msa_data);    /* phylip  */
    MsaPll (std::string msa_filename, msa_format_t msa_format); /* generic */
    virtual ~MsaPll ();

    virtual const char * get_header (mt_index_t index) const;
    virtual char ** get_headers() const;
    virtual const char * get_sequence (mt_index_t index) const;
    virtual char ** get_sequences ( void ) const;
    virtual const unsigned int * get_weights( void ) const;
    virtual bool reorder_sites(partitioning_scheme_t & scheme,
                               bool compress_patterns = true);

    virtual void print() const;

    static bool test(std::string const& msa_filename,
               mt_size_t *n_tips,
               mt_size_t *n_sites,
               mt_size_t *n_patterns = 0,
               msa_format_t *msa_format = 0,
               data_type_t *datatype = 0);
  private:
    std::vector <pll_partition_t *> pll_partitions;
    char **sequences;
    char **tipnames;
    unsigned int * weights;
  };

} /* namespace modeltest */

#endif /* MSAPLL_H_ */
