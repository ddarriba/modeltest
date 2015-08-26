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

#define VERBOSITY_DEFAULT         0
#define VERBOSITY_LOW             1
#define VERBOSITY_MID             2
#define VERBOSITY_HIGH            3

namespace modeltest
{
  extern unsigned int mt_errno;
  extern char mt_errmsg[200];
}

typedef unsigned int mt_size_t;
typedef mt_size_t mt_index_t;
typedef mt_size_t mt_mask_t;

typedef std::vector<mt_index_t> partition_id_t;

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
#define MT_ERROR_PARTITIONS    10400
#define MT_ERROR_MODELS        10500
#define MT_ERROR_INSTANCE      10600
#define MT_ERROR_OPTIMIZE      10700

/* fine grain errors */
#define MT_ERROR_ALIGNMENT_DUPLICATED  10201
#define MT_ERROR_ALIGNMENT_MISSING     10202
#define MT_ERROR_TREE_MISSING          10301
#define MT_ERROR_PARTITIONS_OUTBOUNDS  10401
#define MT_ERROR_PARTITIONS_OVERLAP    10402

#define MT_WARN_PARTITIONS_UNASIGNED   10410

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
    mt_index_t start;
    mt_index_t end;
    mt_index_t stride;
} partition_region_t;

typedef struct
{
    std::vector<partition_region_t> regions;
    data_type datatype;
    std::string partition_name;
} partition_t;

typedef std::vector<partition_t> partitioning_scheme_t;

typedef struct {
    std::string msa_filename;                   //! Input MSA filename
    std::string tree_filename;                  //! User tree filename
    std::string partitions_filename;            //! Partitions filename
    std::string output_filename;                //! Output filename
    tree_type starting_tree;                    //! Starting tree type
    std::vector<mt_index_t> nt_candidate_models;   //! Candidate models for DNA
    std::vector<mt_index_t> aa_candidate_models;   //! Candidate models for AA
    mt_mask_t model_params;                     //! Model parameters to opt
    mt_size_t n_catg;                           //! Number of gamma rate cats
    std::vector<partition_t> * partitions_desc; //! Original partitioning
    std::vector<partition_t> * partitions_eff;  //! Effective partitioning

    mt_size_t n_taxa;        //! Number of taxa
    mt_size_t n_sites;       //! Number of sites

    double epsilon_param;    //! Parameter optimization epsilon
    double epsilon_opt;      //! Global optimization epsilon

    dna_subst_schemes subst_schemes;        //! DNA substitution schemes
    unsigned int rnd_seed;                  //! RNG seed
    int verbose;                            //! Verbosity level
} mt_options;

#endif // GLOBAL_DEFS_H
