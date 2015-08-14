#ifndef GLOBAL_DEFS_H
#define GLOBAL_DEFS_H

typedef unsigned int mt_size_t;
typedef mt_size_t mt_index_t;

#ifdef HAVE_MPI
#include <mpi.h>
#define ROOT (!mpi_rank)
#define MINE(x) ((x % mpi_numprocs) == mpi_rank)
extern int mpi_rank;
extern int mpi_numprocs;
#else
#define ROOT (true)
#define MINE(x) (true)
#endif

typedef enum {
    dt_dna,
    dt_protein
} data_type;

#endif // GLOBAL_DEFS_H
