#ifndef GLOBAL_DEFS_H
#define GLOBAL_DEFS_H

#include <string>
#include <vector>
#include <climits>
#include <iomanip>

#ifdef HAVE_CONFIG_H
#include <config.h>
#else
#define PACKAGE "modeltest"
#define VERSION "1.0.1"
#endif

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

#define ERR_MSG_SIZE 200

#define UNUSED(expr) do { (void)(expr); } while (0)
#define MT_PRECISION_DIGITS 4

#define DEFAULT_GAMMA_RATE_CATS   4
#define DEFAULT_PARAM_EPSILON     0.0001
#define DEFAULT_OPT_EPSILON       0.01
#define DEFAULT_RND_SEED          12345

#define MT_SIZE_UNDEF             UINT_MAX
#define DOUBLE_EPSILON            1e-12

#define VERBOSITY_DEFAULT         0
#define VERBOSITY_LOW             1
#define VERBOSITY_MID             2
#define VERBOSITY_HIGH            3

#define MT_MIN_SMOOTH_FREQ        0.02

#define MT_ERROR_LENGTH 400

#define OUTPUT_LOG_SUFFIX         ".log"
#define OUTPUT_TREE_SUFFIX        ".tree"
#define OUTPUT_RESULTS_SUFFIX     ".out"

namespace modeltest
{
  extern int mt_errno;
  extern char mt_errmsg[MT_ERROR_LENGTH];

  extern double alpha_guess;
  extern double pinv_guess;
  extern double alpha_inv_guess;
  extern double pinv_alpha_guess;

  extern int verbosity;
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
#define MT_ERROR_FREQUENCIES   10250
#define MT_ERROR_TREE          10300
#define MT_ERROR_PARTITIONS    10400
#define MT_ERROR_MODELS        10500
#define MT_ERROR_INSTANCE      10600
#define MT_ERROR_OPTIMIZE      10700

/* fine grain errors */
#define MT_ERROR_ALIGNMENT_DUPLICATED      10201
#define MT_ERROR_ALIGNMENT_MISSING         10202
#define MT_ERROR_ALIGNMENT_ILLEGAL         10203
#define MT_ERROR_ALIGNMENT_HEADER          10204
#define MT_ERROR_ALIGNMENT_UNPRINTABLECHAR 10205
#define MT_ERROR_TREE_MISSING              10301
#define MT_ERROR_PARTITIONS_OUTBOUNDS      10401
#define MT_ERROR_PARTITIONS_OVERLAP        10402

#define MT_WARN_PARTITIONS_UNASIGNED   10410

typedef enum {
    dt_dna,
    dt_protein
} data_type_t;

typedef enum
{
    tree_mp,
    tree_ml_jc_fixed,
    tree_ml_gtr_fixed,
    tree_ml,
    tree_user_fixed,
    tree_random
} tree_type_t;

typedef enum {
    ss_undef,
    ss_3,
    ss_5,
    ss_7,
    ss_11,
    ss_203
} dna_subst_schemes_t;

typedef struct
{
    mt_index_t start;
    mt_index_t end;
    mt_index_t stride;
} partition_region_t;

typedef enum
{
    template_none = 0,
    template_raxml,
    template_phyml,
    template_mrbayes,
    template_paup
} template_models_t;

typedef struct
{
    std::vector<partition_region_t> regions;
    data_type_t datatype;
    mt_size_t states;
    std::string partition_name;
    mt_mask_t model_params;
} partition_descriptor_t;

typedef std::vector<partition_descriptor_t> partitioning_scheme_t;

typedef struct {

    /* input data */
    std::string msa_filename;                   //! Input MSA filename
    std::string tree_filename;                  //! User tree filename
    std::string partitions_filename;            //! Partitions filename

    /* output data */
    std::string output_log_file;                //! Output log filename
    std::string output_tree_file;               //! Output tree filename
    std::string output_results_file;            //! Output results filename
    bool redirect_output;             //! Redirect standard output to a file
    bool force_override;              //! Force overriding existing files
    bool output_tree_to_file;         //! Whether the starting tree is printed

    mt_size_t n_taxa;        //! Number of taxa
    mt_size_t n_sites;       //! Number of sites

    /* configuration */
    tree_type_t starting_tree;                    //! Starting tree type
    dna_subst_schemes_t subst_schemes;  //! DNA substitution schemes
    std::vector<mt_index_t> nt_candidate_models;   //! Candidate models for DNA
    std::vector<mt_index_t> aa_candidate_models;   //! Candidate models for AA
    mt_mask_t model_params;                     //! Model parameters to opt
    mt_size_t n_catg;                           //! Number of gamma rate cats
    std::vector<partition_descriptor_t> * partitions_desc; //! Original partitioning
    std::vector<partition_descriptor_t> * partitions_eff;  //! Effective partitioning
    template_models_t template_models; //! template for different tools
    double epsilon_param;    //! Parameter optimization epsilon
    double epsilon_opt;      //! Global optimization epsilon

    bool smooth_freqs;                //! Force frequencies smoothing
    unsigned int rnd_seed;            //! RNG seed
    int verbose;                      //! Verbosity level

    mt_size_t n_threads;              //! Number of threads for optimiz.
} mt_options_t;

#endif // GLOBAL_DEFS_H
