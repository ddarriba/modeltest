/*
 * msa.h
 *
 *  Created on: Jun 9, 2015
 *      Author: diego
 */

#ifndef MSA_H_
#define MSA_H_

#include "global_defs.h"
#include <string>

namespace modeltest
{

  class Msa
  {
  public:
    Msa (std::string _msa_filename) :
    msa_filename (_msa_filename), n_taxa (0), n_sites (0)
    {
    }
    virtual ~Msa ();

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
    virtual const char* const* get_headers () const = 0;

    /**
     * @brief Gets the sequence at a certain position
     * @param[in] index The sequence to recover
     * @return the sequence at 'index'
     */
    virtual const char * get_sequence (mt_index_t index) const = 0;

    /**
     * @brief Gets the pattern weights
     * @return the pattern weights
     */
    virtual const unsigned int * get_weights( void ) const = 0;

    /**
     * @brief Reorder the sites according to a partitioning scheme
     * @param[in,out] scheme valid partitioning scheme
     * @return true, if success
     */
    virtual bool reorder_sites(partitioning_scheme_t & scheme) = 0;

    /**
     * @brief Prints the sequence
     */
    virtual void print() const = 0;

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
    mt_size_t n_taxa;
    mt_size_t n_sites;
    mt_size_t n_patterns;
  };

} /* namespace modeltest */

#endif /* MSA_H_ */
