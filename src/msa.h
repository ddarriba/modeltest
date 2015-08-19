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
    msa_filename (_msa_filename), n_sequences (0), n_sites (0)
    {
    }
    virtual ~Msa ();

    virtual const char * get_header (mt_index_t index) const = 0;
    virtual const char * get_sequence (mt_index_t index) const = 0;

    mt_size_t get_n_sequences (void) const
    {
      return n_sequences;
    }
    mt_size_t get_n_sites (void) const
    {
      return n_sites;
    }

  protected:
    const std::string msa_filename;
    mt_size_t n_sequences;
    mt_size_t n_sites;
  };

} /* namespace modeltest */

#endif /* MSA_H_ */
