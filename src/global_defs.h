#ifndef GLOBAL_DEFS_H
#define GLOBAL_DEFS_H

#include <string>
#include <vector>
#include <climits>

#ifndef __has_cpp_attribute         // Optional of course.
  #define __has_cpp_attribute(x) 0  // Compatibility with non-clang compilers.
#endif

//#if __has_cpp_attribute(clang::fallthrough)
//#define FALLTHROUGH [[clang::fallthrough]]
//#else
//#define FALLTHROUGH
//#endif

#ifdef __clang__
#define FALLTHROUGH [[clang::fallthrough]]
#else
#define FALLTHROUGH
#endif

#define UNUSED(expr) do { (void)(expr); } while (0)
#define MT_PRECISION_DIGITS 4

#define DEFAULT_GAMMA_RATE_CATS   4
#define DEFAULT_PARAM_EPSILON     0.0001
#define DEFAULT_OPT_EPSILON       0.001
#define DEFAULT_RND_SEED          12345

#define MT_SIZE_UNDEF             UINT_MAX
#define DOUBLE_EPSILON            1e-12

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

/* general errors */
#define MT_ERROR_IO              900
#define MT_ERROR_IO_FORMAT       910
#define MT_ERROR_UNIMPLEMENTED 10000
#define MT_ERROR_LIBPLL        10100
#define MT_ERROR_ALIGNMENT     10200
#define MT_ERROR_TREE          10300
#define MT_ERROR_MODELS        10400
#define MT_ERROR_INSTANCE      10500
#define MT_ERROR_OPTIMIZE      10600

/* fine grain errors */
#define MT_ERROR_ALIGNMENT_DUPLICATED  10201
#define MT_ERROR_ALIGNMENT_MISSING     10202
#define MT_ERROR_TREE_MISSING          10301

typedef enum {
    dt_dna,
    dt_protein
} data_type;

typedef enum
{
    tree_mp,
    tree_ml_jc_fixed,
    tree_ml_gtr_fixed,
    tree_ml,
    tree_user_fixed
} tree_type;

typedef enum {
    ss_undef,
    ss_3,
    ss_5,
    ss_7,
    ss_11,
    ss_203
} dna_subst_schemes;

typedef struct
{
    int start;
    int end;
    int stride;
} partition_region_t;

typedef struct
{
    std::vector<partition_region_t> regions;
    data_type datatype;
    std::string partition_name;
} partition_t;

typedef std::vector<partition_t> partitioning_scheme_t;

typedef struct {
    data_type datatype;
    std::string msa_filename;
    std::string tree_filename;
    std::string partitions_filename;
    std::string output_filename;
    tree_type starting_tree;
    std::vector<mt_index_t> candidate_models;
    int model_params;
    mt_size_t n_catg;
    std::vector<partition_t> * partitions_desc;

    mt_size_t n_taxa;
    mt_size_t n_sites;

    double epsilon_param;
    double epsilon_opt;

    dna_subst_schemes subst_schemes;
    unsigned int rnd_seed;
} mt_options;

#endif // GLOBAL_DEFS_H
