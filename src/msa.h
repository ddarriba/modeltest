/*
 * msa.h
 *
 *  Created on: Jun 9, 2015
 *      Author: diego
 */

#ifndef MSA_H_
#define MSA_H_

#include <string>

namespace modeltest
{

  class Msa
  {
  public:
    Msa (std::string msa_filename) :
	msa_filename (msa_filename), n_sequences (0), n_sites (0)
    {
    }
    virtual ~Msa ()
    {
    }

    virtual const char * get_header (int index) = 0;
    virtual const char * get_sequence (int index) = 0;

    int get_n_sequences (void) const
    {
      return n_sequences;
    }
    int get_n_sites (void) const
    {
      return n_sites;
    }

  protected:
    const std::string msa_filename;
    int n_sequences;
    int n_sites;
  };

} /* namespace modeltest */

#endif /* MSA_H_ */
