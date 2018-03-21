/*
  Copyright (C) 2016 Diego Darriba

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU Affero General Public License as
  published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Affero General Public License for more details.

  You should have received a copy of the GNU Affero General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  Contact: Diego Darriba <Diego.Darriba@h-its.org>,
  Heidelberg Institute for Theoretical Studies,
  Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

#ifndef GLOBAL_DEFS_H
#define GLOBAL_DEFS_H

#include <string>
#include <vector>
#include <climits>
#include <iomanip>
#include <mutex>

#ifdef HAVE_CONFIG_H
#include <config.h>
#else
#define PACKAGE "modeltest"
#define VERSION "x.y.z"
#endif

#ifdef BUILD_MPI
#define MPI_ENABLED 1
#define BARRIER MPI_Barrier(MPI_COMM_WORLD)
#else
#define MPI_ENABLED 0
#define BARRIER
#endif

#define ENABLE_DT_OUTPUT 0

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
#define DEFAULT_PARAM_EPSILON     0.01
#define DEFAULT_OPT_EPSILON       0.01
#define DEFAULT_RND_SEED          12345

#define MT_SIZE_UNDEF             UINT_MAX
#define DOUBLE_EPSILON            1e-12

#define VERBOSITY_DEFAULT         0
#define VERBOSITY_LOW             1
#define VERBOSITY_MID             2
#define VERBOSITY_HIGH            3

#define MT_MIN_SMOOTH_FREQ        0.02

#define MT_MAX_STATES             20

#define MT_ERROR_LENGTH           400

#define OUTPUT_LOG_SUFFIX         ".log"
#define OUTPUT_TREE_SUFFIX        ".tree"
#define OUTPUT_RESULTS_SUFFIX     ".out"
#define OUTPUT_MODELS_SUFFIX      ".models"
#define OUTPUT_TOPOS_SUFFIX       ".topos"
#define OUTPUT_RAXML_PART_SUFFIX  ".part"
#define CHECKPOINT_SUFFIX         ".ckp"

namespace modeltest
{
  extern int mt_errno;
  extern char mt_errmsg[MT_ERROR_LENGTH];

  extern double alpha_guess;
  extern double pinv_guess;
  extern double alpha_inv_guess;
  extern double pinv_alpha_guess;

  extern std::mutex model_mutex;

  extern int verbosity;
  extern time_t global_ini_time;

  extern bool disable_repeats;    //! Disable subtree repeats
}

typedef unsigned int mt_size_t;
typedef mt_size_t mt_index_t;
typedef mt_size_t mt_mask_t;

typedef std::vector<mt_index_t> partition_id_t;

extern int mpi_rank;
extern int mpi_numprocs;

extern mt_size_t num_cores;  //! Number of physical cores

extern bool have_avx;
extern bool have_sse3;

#if(MPI_ENABLED)
#include <mpi.h>
#define ROOT (!mpi_rank)
#define MINE(x) ((x % mpi_numprocs) == mpi_rank)
extern MPI_Comm master_mpi_comm;
#else
#define ROOT (true)
#define MINE(x) (true)
#endif

/* general errors */
#define MT_ERROR_SYSTEM          800
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
#define MT_ERROR_NUMBER        10800

/* fine grain errors */
#define MT_ERROR_ALIGNMENT_DUPLICATED      10201
#define MT_ERROR_ALIGNMENT_MISSING         10202
#define MT_ERROR_ALIGNMENT_ILLEGAL         10203
#define MT_ERROR_ALIGNMENT_HEADER          10204
#define MT_ERROR_ALIGNMENT_UNPRINTABLECHAR 10205
#define MT_ERROR_TREE_MISSING              10301
#define MT_ERROR_PARTITIONS_OUTBOUNDS      10401
#define MT_ERROR_PARTITIONS_OVERLAP        10402

#define MT_ERROR_NUMBER_INT                10801
#define MT_ERROR_NUMBER_FLOAT              10802

#define MT_WARN_PARTITIONS_UNASIGNED   10410

/* 14 bits for partition descriptor: 16384 */
#define NBIT_PARTITION_INDEX 14
#define MAX_PARTITION_INDEX (1<<NBIT_PARTITION_INDEX)

const char dna_chars[20] = {'A','C','G','T'};
const char aa_chars[20] = {'A','R','N','D','C',
                           'E','Q','G','H','I',
                           'L','K','M','F','P',
                           'S','T','W','Y','V'};

typedef enum {
    dt_dna,
    dt_protein
} data_type_t;

typedef enum {
    mf_undefined,
    mf_fasta,
    mf_phylip
} msa_format_t;

typedef enum {
    asc_none,
    asc_lewis,
    asc_felsenstein,
    asc_stamatakis
} asc_bias_t;

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
    std::vector<partition_region_t> regions; //! description of regions
    data_type_t datatype;                    //! data type
    bool gap_aware;                          //! treat gaps as new state
    mt_size_t states;                        //! number of states
    mt_size_t site_patterns;                 //! number of sites
    std::string partition_name;              //! name of the partition
    mt_mask_t model_params;                  //! model parameters
    mt_size_t n_categories;                  //! number of categories
    asc_bias_t asc_bias_corr;                //! ascertainment bias correction
    mt_size_t *asc_weights;                  //! state weights
    tree_type_t starting_tree;               //! starting tree type
    mt_index_t unique_id;                    //! id for checkpointing
} partition_descriptor_t;

typedef std::vector<partition_descriptor_t> partitioning_scheme_t;

typedef struct {

    /* input data */
    std::string msa_filename;         //! Input MSA filename
    msa_format_t msa_format;          //! Input MSA format
    std::string tree_filename;        //! User tree filename
    std::string partitions_filename;  //! Partitions filename

    /* output data */
    std::string output_log_file;      //! Output log filename
    std::string output_tree_file;     //! Output tree filename
    std::string output_results_file;  //! Output results filename
    std::string output_models_file;   //! Output models filename
    std::string output_topos_file;    //! Output topologies filename
    std::string output_raxml_part_file;  //! Output RAxML-NG partitions filename
    bool redirect_output;             //! Redirect standard output to a file
    bool force_override;              //! Force overriding existing files
    bool output_tree_to_file;         //! Whether the starting tree is printed

    std::string checkpoint_file;      //! Checkpoint filename
    bool write_checkpoint;

    bool compress_patterns;  //! Compress site patterns
    mt_size_t n_taxa;        //! Number of taxa
    mt_size_t n_sites;       //! Number of sites
    mt_size_t n_patterns;    //! Number of unique patterns
    mt_size_t max_memb;      //! Required memory estimation

    /* configuration */
    tree_type_t starting_tree;                    //! Starting tree type
    dna_subst_schemes_t subst_schemes;            //! DNA substitution schemes
    bool rate_clustering;                         //! DNA rate clustering
    std::vector<mt_index_t> nt_candidate_models;  //! Candidate models for DNA
    std::vector<mt_index_t> aa_candidate_models;  //! Candidate models for AA
    mt_mask_t model_params;                       //! Model parameters to opt
    mt_size_t n_catg;                             //! Number of gamma rate cats
    asc_bias_t asc_bias_corr;                     //! ascertainment bias correction
    mt_size_t asc_weights[MT_MAX_STATES];         //! dummy weights
    std::vector<partition_descriptor_t> * partitions_desc; //! Original partitioning
    std::vector<partition_descriptor_t> const* partitions_eff;  //! Effective partitioning
    template_models_t template_models; //! template for different tools
    double epsilon_param;    //! Parameter optimization epsilon
    double epsilon_opt;      //! Global optimization epsilon

    bool smooth_freqs;                //! Force frequencies smoothing
    unsigned int rnd_seed;            //! RNG seed
    int verbose;                      //! Verbosity level

    mt_size_t n_threads;              //! Number of threads for optimiz.
} mt_options_t;

#endif // GLOBAL_DEFS_H
