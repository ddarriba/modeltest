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

    virtual const char * get_header (int index);
    virtual const char * get_sequence (int index);

    static bool test(std::string const& msa_filename,
               int *n_tips,
               int *n_sites);

  private:
    char **sequences;
    char **tipnames;
  };

} /* namespace modeltest */

#endif /* MSAPLL_H_ */
