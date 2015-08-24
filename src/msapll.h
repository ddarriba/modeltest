/*
 * msapll.h
 *
 *  Created on: Jun 9, 2015
 *      Author: diego
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
    MsaPll (std::string msa_filename);
    virtual ~MsaPll ();

    virtual const char * get_header (mt_index_t index) const;
    virtual const char * get_sequence (mt_index_t index) const;
    virtual bool reorder_sites(partitioning_scheme_t & scheme);
    virtual void print() const;

    static bool test(std::string const& msa_filename,
               mt_size_t *n_tips,
               mt_size_t *n_sites);
  private:
    char **sequences;
    char **tipnames;
  };

} /* namespace modeltest */

#endif /* MSAPLL_H_ */
