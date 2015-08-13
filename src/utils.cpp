#include "utils.h"

#include <algorithm>
#include <cstdarg>
#include <cstdio>

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

void * Utils::allocate(mt_size_t n, mt_size_t size) {
    void * ptr;
    ptr = malloc(n * size);
    if (!ptr) {
        exit_with_error("Error allocating memory");
    }
    return ptr;
}

void * Utils::c_allocate(mt_size_t n, mt_size_t size) {
    void * ptr;
    ptr = calloc(n, size);
    if (!ptr) {
        exit_with_error("Error allocating memory");
    }
    return ptr;
}

void Utils::exit_with_error(const char * message, ...) {
    va_list arg;
    if (ROOT) {
        va_start(arg, message);

        fprintf(stderr, "Error: ");
        vfprintf(stderr, message, arg);
        fprintf(stderr, "\n");

        va_end(arg);
    }
#ifdef HAVE_MPI
    MPI_Finalize();
#endif
    exit(EXIT_FAILURE);
}

} /* namespace */
