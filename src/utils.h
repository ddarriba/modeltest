#ifndef UTILS_H
#define UTILS_H

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
};

}

#endif // UTILS_H
