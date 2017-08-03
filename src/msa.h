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

#ifndef MSA_H_
#define MSA_H_

#include "global_defs.h"

#include <string>

namespace modeltest
{

  typedef struct msa_stats
  {
    unsigned int states;

    unsigned long dup_taxa_pairs_count;
    unsigned long * dup_taxa_pairs;

    unsigned long dup_seqs_pairs_count;
    unsigned long * dup_seqs_pairs;

    double gap_prop;
    unsigned long gap_seqs_count;
    unsigned long * gap_seqs;
    unsigned long gap_cols_count;
    unsigned long * gap_cols;

    double inv_prop;
    unsigned long inv_cols_count;
    unsigned long * inv_cols;

    double * freqs;
    double * subst_rates;
  } msa_stats_t;

  class Msa
  {
  public:
    Msa (std::string _msa_filename,
         msa_format_t _msa_format,
         partitioning_scheme_t & scheme) :
      msa_filename (_msa_filename),
      msa_format(_msa_format),
      n_taxa (0),
      n_sites (0),
      scheme (scheme)
    {
    }
    virtual ~Msa ();

    static msa_format_t guess_msa_format(std::string const& msa_filename);

    /**
     * @brief Gets the header of a certain sequence
     * @param[in] index The index of the sequence
     * @return the header at 'index'
     */
    virtual const char * get_header (mt_index_t index) const = 0;

    /**
     * @brief Gets the list of headers
     * @return an array of headers
     */
    virtual char ** get_headers ( void ) const = 0;

    /**
     * @brief Gets the sequence at a certain position
     * @param[in] index The sequence to recover
     * @return the sequence at 'index'
     */
    virtual const char * get_sequence (mt_index_t index) const = 0;

    virtual char ** get_sequences ( void ) const = 0;

    /**
     * @brief Gets the pattern weights
     * @return the pattern weights
     */
    virtual const unsigned int * get_weights( void ) const = 0;

    /**
     * @brief Reorder the sites according to a partitioning scheme
     * @param[in,out] scheme valid partitioning scheme
     * @param[in] compress_patterns compress identical site patterns
     * @return true, if success
     */
    virtual bool reorder_sites(partitioning_scheme_t & scheme,
                               bool compress_patterns = true) = 0;

    virtual msa_stats_t const& get_stats( mt_index_t partition ) const = 0;

    /**
     * @brief Prints the sequence
     */
    virtual void print() const = 0;

    partitioning_scheme_t const* get_scheme( void ) const
    {
      return & scheme;
    }

    mt_size_t get_n_sequences (void) const
    {
      return n_taxa;
    }

    mt_size_t get_n_sites (void) const
    {
      return n_sites;
    }

    mt_size_t get_n_patterns(void) const
    {
      return n_patterns;
    }

  protected:
    const std::string msa_filename;
    msa_format_t msa_format;
    mt_size_t n_taxa;
    mt_size_t n_sites;
    mt_size_t n_patterns;
    partitioning_scheme_t scheme;
    std::vector<msa_stats_t> stats;
  };

} /* namespace modeltest */

#endif /* MSA_H_ */
