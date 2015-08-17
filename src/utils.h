#ifndef UTILS_H
#define UTILS_H

#include "global_defs.h"
#include <string>

namespace modeltest {

#ifdef WIN32
struct MatchPathSeparator
{
    bool operator()( char ch ) const
    {
        return ch == '\\' || ch == '/';
    }
};
#else
struct MatchPathSeparator
{
    bool operator()( char ch ) const
    {
        return ch == '/';
    }
};
#endif

class Utils
{
public:
    Utils();
    static std::string getBaseName(std::string const& filename);
    static size_t mem_size(unsigned int n_taxa,
                           unsigned int n_sites,
                           unsigned int n_categories,
                           unsigned int n_states);

    static void * allocate(mt_size_t n, mt_size_t size);
    static void * c_allocate(mt_size_t n, mt_size_t size);
    static void exit_with_error(const char * message, ...) __attribute__ ((noreturn));
};

}

#endif // UTILS_H
