#include "utils.h"

#include <algorithm>

namespace modeltest {

Utils::Utils()
{
}

std::string Utils::getBaseName(std::string const& filename)
{
  return std::string(
    std::find_if( filename.rbegin(), filename.rend(),
    MatchPathSeparator() ).base(),
    filename.end() );
}

size_t Utils::mem_size(unsigned int n_taxa,
                       unsigned int n_sites,
                       unsigned int n_categories,
                       unsigned int n_states)
{
    int n_nodes = n_taxa * 2 - 2;
    size_t mem = (n_nodes * n_sites * n_states * n_categories +
            n_states * n_states * n_categories * n_nodes) * sizeof(double) +
            ((n_nodes + 1)* n_sites) * sizeof(int);

    return mem;
}

}
