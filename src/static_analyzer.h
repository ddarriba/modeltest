#ifndef STATIC_ANALYZER_H_
#define STATIC_ANALYZER_H_

#include "global_defs.h"
#include <string>

namespace modeltest
{

  class StaticAnalyzer
  {
  public:
    static void print_info(const std::string msa_filename, msa_format_t format);
  };

}

#endif
