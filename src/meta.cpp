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

#include "meta.h"
#include "utils.h"
#include "modeltest.h"
#include "model_defs.h"
#include "static_analyzer.h"

#include "genesis/logging.h"

#include <getopt.h>
#include <sstream>
#include <algorithm>

#define MAX_OPT_LENGTH 40
#define SHORT_OPT_LENGTH 6
#define COMPL_OPT_LENGTH MAX_OPT_LENGTH-SHORT_OPT_LENGTH

using namespace std;

bool Meta::parse_arguments(int argc, char *argv[], mt_options_t & exec_opt, mt_size_t *n_procs)
{
    bool input_file_ok = false;
    bool output_files_ok = false;
    bool exist_dna_models = false;
    bool exist_protein_models = false;

    int exclusion_modelset = 0;

    template_models_t template_models = template_none;
    dna_subst_schemes_t dna_ss = ss_undef;
    string user_candidate_models = "";
    string output_basename = "";

    /* defaults */
    dna_subst_schemes_t default_ss = ss_11;

    /* set default options */
    data_type_t arg_datatype = dt_dna;
    mt_size_t freqs_mask = 0;
    bool gap_aware = false;
    exec_opt.n_catg           = DEFAULT_GAMMA_RATE_CATS;
    exec_opt.gamma_rates_mode = DEFAULT_GAMMA_RATE_MODE;
    exec_opt.epsilon_param   = DEFAULT_PARAM_EPSILON;
    exec_opt.epsilon_opt     = DEFAULT_OPT_EPSILON;
    exec_opt.rnd_seed        = DEFAULT_RND_SEED;
    exec_opt.model_params    = 0;
    exec_opt.compress_patterns = true;
    exec_opt.keep_model_parameters = false;
    exec_opt.smooth_freqs    = false;
    exec_opt.rate_clustering = false;
    exec_opt.subst_schemes   = ss_undef;
    exec_opt.partitions_desc = NULL;
    exec_opt.partitions_eff  = NULL;
    exec_opt.verbose         = VERBOSITY_DEFAULT;
    exec_opt.n_threads       = 1;
    exec_opt.starting_tree   = tree_mp;
    exec_opt.force_override  = false;
    exec_opt.asc_bias_corr   = asc_none;
    exec_opt.write_checkpoint = true;
    memset(exec_opt.asc_weights, 0, sizeof(mt_size_t) * MT_MAX_STATES);

    static struct option long_options[] =
    {
        { "asc-bias", required_argument,     0, 'a' },
        { "categories", required_argument,   0, 'c' },
        { "datatype", required_argument,     0, 'd' },
        { "gap-aware", no_argument,          0, 13 },
        { "input", required_argument,        0, 'i' },
        { "model-freqs", required_argument,  0, 'f' },
        { "gamma-rates", required_argument,  0, 'g' },
        { "no-compress", no_argument,        0, 'H' },
        { "model-het", required_argument,    0, 'h' },
        { "keep-params", no_argument,        0, 'k' },
        { "models", required_argument,       0, 'm' },
        { "output", required_argument,       0, 'o' },
        { "threads", required_argument,      0, 'p' },
        { "partitions", required_argument,   0, 'q' },
        { "rngseed", required_argument,      0, 'r' },
        { "schemes", required_argument,      0, 's' },
        { "template", required_argument,     0, 'T' },
        { "tree", required_argument,         0, 't' },
        { "utree", required_argument,        0, 'u' },
        { "version", no_argument,            0, 'v' },
        { "verbose", no_argument,            0, 'V' },
        { "help", no_argument,               0,  0 },
        { "usage", no_argument,              0,  1 },
        { "psearch", required_argument,      0,  2 },
        { "smooth-frequencies", no_argument, 0,  3 },
        { "disable-repeats", no_argument,    0,  4 },
        { "disable-checkpoint", no_argument, 0,  5 },
        { "eps", required_argument,          0, 10 },
        { "tol", required_argument,          0, 11 },
        { "msa-info", no_argument,           0, 12 },
        { "force", no_argument,              0, 20 },
        { "quiet", no_argument,              0, 21 },
        { "ultraverbose", no_argument,       0, 22 },
        { 0, 0, 0, 0 }
    };

    int opt = 0, long_index = 0;
    bool params_ok = true;
    while ((opt = getopt_long(argc, argv, "a:c:d:f:g:h:Hki:m:o:p:q:r:s:t:T:u:vV", long_options,
                              &long_index)) != -1) {
        switch (opt) {
        case 0:
            print_usage(cout);
            cout << endl;
            print_help(cout);
            modeltest::mt_errno = MT_ERROR_IGNORE;
            return false;
        case 1:
            print_usage(cout);
            modeltest::mt_errno = MT_ERROR_IGNORE;
            return false;
        case 2:
            /* partitioning search */
            //TODO
            if (!strcmp(optarg, "kn"))
            {
                LOG_ERR << "Search K=N" << endl;
            }
            else if (!strcmp(optarg, "hcluster"))
            {
                LOG_ERR << "Search hierarchical clustering" << endl;
            }
            else if (!strcmp(optarg, "greedy"))
            {
                LOG_ERR << "Search greedy" << endl;
            }
            else if (!strcmp(optarg, "kn"))
            {
                LOG_ERR << "Search K=N" << endl;
            }
            else
            {
                LOG_ERR <<  PACKAGE << ": Invalid scheme search algorithm: " << optarg << endl;
                params_ok = false;
            }
            assert(0);
            break;
        case 3:
            /* force frequencies smoothing */
            exec_opt.smooth_freqs = true;
            break;
        case 4:
            /* disable subtree repeats */
            modeltest::disable_repeats = true;
            break;
        case 5:
            /* disable checkpoint */
            exec_opt.write_checkpoint = false;
            break;
        case 10:
            exec_opt.epsilon_opt = atof(optarg);
            if (exec_opt.epsilon_opt <= 0.0)
            {
                LOG_ERR << PACKAGE << ": Invalid optimization epsilon: " << exec_opt.epsilon_opt << endl;
                params_ok = false;
            }
            break;
        case 11:
            exec_opt.epsilon_param = atof(optarg);
            if (exec_opt.epsilon_param <= 0.0)
            {
                LOG_ERR << PACKAGE << ": Invalid parameter tolerance: " << exec_opt.epsilon_param << endl;
                params_ok = false;
            }
            break;
        case 12:
            /* --msa-info */
            if (!input_file_ok)
            {
                LOG_ERR << PACKAGE << ": MSA (-i) must be set before this argument." << endl;
            }
            else
            {
              modeltest::StaticAnalyzer::print_info(exec_opt.msa_filename,  mf_fasta);
            }
            return false;
        case 13:
            /* --gap-aware */
            gap_aware = true;
            break;
        case 20:
            exec_opt.force_override = true;
            break;
        case 21:
            exec_opt.verbose = VERBOSITY_LOW;
            break;
        case 22:
            exec_opt.verbose = VERBOSITY_ULTRA;
            break;
        case 'a':
            /* ascertainment bias correction */
            {
              char * asc_bias_type = strtok(optarg, ":");
              char * asc_bias_values = strtok(NULL, "\0");
              if (!strcmp(asc_bias_type, "lewis"))
              {
                  exec_opt.asc_bias_corr = asc_lewis;
              }
              else if (!strcmp(asc_bias_type, "felsenstein"))
              {
                  exec_opt.asc_bias_corr = asc_felsenstein;
                  /* parse dummy weight */
                  if (asc_bias_values && strlen(asc_bias_values))
                  {
                    mt_size_t w = modeltest::Utils::parse_size(asc_bias_values);
                    if (modeltest::mt_errno)
                    {
                      LOG_ERR << PACKAGE <<
                        ": Invalid weight for Felsenstein asc bias correction: "
                        << asc_bias_values << endl;
                      params_ok = false;
                    }
                    else
                    {
                      exec_opt.asc_weights[0] = w;
                    }

                    //CHECK
                    for (int i=1; i<MT_MAX_STATES; ++i)
                      exec_opt.asc_weights[i] = 0;
                  }
                  else
                  {
                    LOG_ERR << PACKAGE <<
                      ": Felsenstein asc bias correction require the weight of dummy sites." << endl;
                    params_ok = false;
                  }
              }
              else if (!strcmp(asc_bias_type, "stamatakis"))
              {
                  exec_opt.asc_bias_corr = asc_stamatakis;
                  int current_state = 0;
                  char * asc_state_w = strtok(asc_bias_values, ",");
                  while (asc_state_w != NULL)
                  {
                    mt_size_t w = modeltest::Utils::parse_size(asc_state_w);
                    if (modeltest::mt_errno)
                    {
                      LOG_ERR << PACKAGE <<
                        ": Invalid weight for Stamatakis asc bias correction: state "
                        << current_state+1 << endl;
                      params_ok = false;
                    }
                    else
                      exec_opt.asc_weights[current_state] = w;
                    ++current_state;
                    asc_state_w = strtok(NULL, ",");
                  }
              }
              else
              {
                  LOG_ERR <<  PACKAGE << ": Invalid ascertainment bias correction algorithm: " << asc_bias_type << endl;
                  LOG_ERR <<  setw(strlen(PACKAGE) + 2) << setfill(' ') << " " << "Should be one of {lewis, felsenstein, stamatakis}" << endl;
                  params_ok = false;
              }
            }
            break;
        case 'c':
            exec_opt.n_catg = (mt_size_t) atoi(optarg);
            if (exec_opt.n_catg <= 0 || exec_opt.n_catg > MT_MAX_CATEGORIES)
            {
                LOG_ERR << PACKAGE << ": Invalid number of categories: " << exec_opt.n_catg << endl;
                LOG_ERR <<  setw(strlen(PACKAGE) + 2) << setfill(' ') << " " << "Should be a positive integer number between 1 and " << MT_MAX_CATEGORIES << endl;
                params_ok = false;
            }
            break;
        case 'd':
            if (!strcmp(optarg, "nt"))
            {
                arg_datatype = dt_dna;
            }
            else if (!strcmp(optarg, "aa"))
            {
                arg_datatype = dt_protein;
            }
            else
            {
                LOG_ERR <<  PACKAGE << ": Invalid datatype " << optarg << endl;
                LOG_ERR <<  setw(strlen(PACKAGE) + 2) << setfill(' ') << " " << "Should be one of {nt, aa} " << endl;
                params_ok = false;
            }
            break;
        case 'f':
            for (mt_index_t i=0; i<strlen(optarg); i++)
            {
                switch(optarg[i])
                {
                case 'f':
                case 'F':
                    /* equal freqs (DNA) / empirical freqs (AA) */
                    freqs_mask |= MOD_PARAM_FIXED_FREQ;
                    break;
                case 'e':
                case 'E':
                    /* ML freqs (DNA) / model defined freqs (AA) */
                    freqs_mask |= MOD_PARAM_ESTIMATED_FREQ;
                    break;
                default:
                    LOG_ERR <<  PACKAGE << ": Unrecognised rate heterogeneity parameter " << optarg[i] << endl;
                    LOG_ERR <<  setw(strlen(PACKAGE) + 2) << setfill(' ') << " " << "Should be a non-empty combination of {f,e}" << endl;
                    params_ok = false;
                }
            }
            break;
        case 'g':
            switch(optarg[0])
            {
            case 'a':
            case 'A':
                exec_opt.gamma_rates_mode = PLL_GAMMA_RATES_MEAN;
                break;
            case 'm':
            case 'M':
                exec_opt.gamma_rates_mode = PLL_GAMMA_RATES_MEDIAN;
                break;
            default:
                LOG_ERR <<  PACKAGE << ": Unrecognised gamma rates mode " << optarg << endl;
                LOG_ERR <<  setw(strlen(PACKAGE) + 2) << setfill(' ') << " " << "Should be either 'a' (average) or 'm' (median)" << endl;
                params_ok = false;
            }
            break;
        case 'h':
            for (mt_index_t i=0; i<strlen(optarg); i++)
            {
                switch(optarg[i])
                {
                case 'u':
                case 'U':
                    exec_opt.model_params  |= MOD_PARAM_NO_RATE_VAR;
                    break;
                case 'i':
                case 'I':
                    exec_opt.model_params  |= MOD_PARAM_INV;
                    break;
                case 'g':
                case 'G':
                    exec_opt.model_params  |= MOD_PARAM_GAMMA;
                    break;
                case 'f':
                case 'F':
                    exec_opt.model_params  |= MOD_PARAM_INV_GAMMA;
                    break;
                case 'r':
                case 'R':
                    exec_opt.model_params  |= MOD_PARAM_FREE_RATES;
                    break;
                default:
                    LOG_ERR <<  PACKAGE << ": Unrecognised rate heterogeneity parameter " << optarg[i] << endl;
                    LOG_ERR <<  setw(strlen(PACKAGE) + 2) << setfill(' ') << " " << "Should be a non-empty combination {u,i,g,f}" << endl;
                    params_ok = false;
                }
            }
            break;
        case 'H':
            exec_opt.compress_patterns = false;
            break;
        case 'k':
            exec_opt.keep_model_parameters = true;
            break;
        case 'i':
            exec_opt.msa_filename = optarg;
            input_file_ok = strcmp(optarg, "");
            break;
        case 'm':
        {
            user_candidate_models = optarg;
            exclusion_modelset |= 1;
            break;
        }
        case 'o':
            output_basename = optarg;
            break;
        case 'p':
            *n_procs = (mt_size_t) atoi(optarg);
            exec_opt.n_threads = *n_procs;
            if (*n_procs <= 0)
            {
                LOG_ERR << PACKAGE << ": Invalid number of parallel processes: " << optarg << endl;
                LOG_ERR <<  setw(strlen(PACKAGE) + 2) << setfill(' ') << " " << "Should a positive integer number" << endl;
                params_ok = false;
            }
            break;
        case 'q':
            exec_opt.partitions_filename = optarg;
            break;
        case 'r':
            exec_opt.rnd_seed = (unsigned int) atoi(optarg);
            break;
        case 's':
            if (!strcmp(optarg, "3"))
            {
                dna_ss = ss_3;
            }
            else if (!strcmp(optarg, "5"))
            {
                dna_ss = ss_5;
            }
            else if (!strcmp(optarg, "7"))
            {
                dna_ss = ss_7;
            }
            else if (!strcmp(optarg, "11"))
            {
                dna_ss = ss_11;
            }
            else if (!strcmp(optarg, "203"))
            {
                dna_ss = ss_203;
            }
            else
            {
                LOG_ERR << PACKAGE << ": Invalid number of substitution schemes " << optarg << endl;
                LOG_ERR <<  setw(strlen(PACKAGE) + 2) << setfill(' ') << " " << "Should be one of {3,5,7,11,203}" << endl;
                params_ok = false;
            }
            exclusion_modelset |= 2;
            break;
        case 't':
            if (!strcmp(optarg, "user"))
            {
                exec_opt.starting_tree = tree_user_fixed;
            }
            else if (!strcmp(optarg, "mp"))
            {
                exec_opt.starting_tree = tree_mp;
            }
            else if (!strcmp(optarg, "ml"))
            {
                exec_opt.starting_tree = tree_ml;
            }
            else if (!strcmp(optarg, "fixed-ml-gtr"))
            {
                exec_opt.starting_tree = tree_ml_gtr_fixed;
            }
            else if (!strcmp(optarg, "fixed-ml-jc"))
            {
                exec_opt.starting_tree = tree_ml_jc_fixed;
            }
            else if (!strcmp(optarg, "random"))
            {
                exec_opt.starting_tree = tree_random;
            }
            else
            {
                LOG_ERR << PACKAGE << ": Invalid starting topology " << optarg << endl;
                LOG_ERR <<  setw(strlen(PACKAGE) + 2) << setfill(' ') << " " << "Should be one of {ml,mp,fixed-ml-gtr,fixed-ml-jc,random,user}" << endl;
                params_ok = false;
            }
            break;
        case 'T':
            /* load template */
            if (!strcmp(optarg, "raxml"))
            {
                template_models = template_raxml;
            }
            else if (!strcmp(optarg, "phyml"))
            {
                template_models = template_phyml;
            }
            else if (!strcmp(optarg, "mrbayes"))
            {
                template_models = template_mrbayes;
            }
            else if (!strcmp(optarg, "paup"))
            {
                template_models = template_paup;
            }
            else
            {
                LOG_ERR <<  PACKAGE << ": Invalid template: " << optarg << endl;
                LOG_ERR <<  setw(strlen(PACKAGE) + 2) << setfill(' ') << " " << "Should be one of {raxml,phyml,mrbayes,paup}" << endl;
                params_ok = false;
            }
            exclusion_modelset |= 4;
            break;
        case 'u':
            exec_opt.tree_filename = optarg;
            break;
        case 'v':
            print_version();
            modeltest::mt_errno = MT_ERROR_IGNORE;
            return false;
        case 'V':
            exec_opt.verbose = VERBOSITY_HIGH;
            break;
        default:
            return false;
        }
    }

    switch (exec_opt.verbose)
    {
      case VERBOSITY_LOW:
        genesis::utils::Logging::max_level (genesis::utils::Logging::kWarning);
        break;
      case VERBOSITY_DEFAULT:
      case VERBOSITY_MID:
        genesis::utils::Logging::max_level (genesis::utils::Logging::kProgress);
        break;
      case VERBOSITY_HIGH:
        genesis::utils::Logging::max_level (genesis::utils::Logging::kDebug);
        break;
      case VERBOSITY_ULTRA:
        genesis::utils::Logging::max_level (genesis::utils::Logging::kDebug4);
        break;
      default:
        assert(0);
    }

    if (__builtin_popcount(exclusion_modelset) > 1)
    {
      LOG_ERR << PACKAGE << ": Options 'm' (--models), 's' (--schemes), and 'T' (--template) are mutually exclusive" << endl;
      return false;
    }

    srand(exec_opt.rnd_seed);

    /* test MSA */
    if (!modeltest::MsaPll::test(exec_opt.msa_filename,
                                      &exec_opt.n_taxa,
                                      &exec_opt.n_sites,
                                      0,//exec_opt.compress_patterns?(&exec_opt.n_patterns):0,
                                      &exec_opt.msa_format))
    {
        LOG_ERR << PACKAGE << ": Cannot parse the msa: " << exec_opt.msa_filename << endl;
        LOG_ERR <<  setw(strlen(PACKAGE) + 2) << setfill(' ') << " " << "[" << modeltest::mt_errno << "]: " << modeltest::mt_errmsg << endl;
        params_ok = false;
    }

    //if (!exec_opt.compress_patterns)
    exec_opt.n_patterns = exec_opt.n_sites;

    /* if there are no model specifications, include all */
    mt_mask_t all_params = MOD_PARAM_NO_RATE_VAR |
                           MOD_PARAM_INV |
                           MOD_PARAM_GAMMA |
                           MOD_PARAM_INV_GAMMA |
                           MOD_PARAM_FREE_RATES;
    if (!(exec_opt.model_params & all_params))
        exec_opt.model_params |= DEFAULT_PARAMS;

    if (freqs_mask)
    {
      if (freqs_mask & MOD_PARAM_ESTIMATED_FREQ)
      {
        if (arg_datatype == dt_dna)
          exec_opt.model_params  |= MOD_PARAM_ESTIMATED_FREQ;
        else if (arg_datatype == dt_protein)
          exec_opt.model_params  |= MOD_PARAM_EMPIRICAL_FREQ;
      }
      if (freqs_mask & MOD_PARAM_FIXED_FREQ)
      {
        exec_opt.model_params  |= MOD_PARAM_FIXED_FREQ;
      }
    }
    else
    {
      /* if there are no frequencies specifications, include all */
      if (arg_datatype == dt_dna)
        exec_opt.model_params |= (MOD_PARAM_FIXED_FREQ | MOD_PARAM_ESTIMATED_FREQ);
      else if (arg_datatype == dt_protein)
        exec_opt.model_params |= (MOD_PARAM_FIXED_FREQ | MOD_PARAM_EMPIRICAL_FREQ);
    }

    /* validate ascertainment bias correction */
    if (exec_opt.asc_bias_corr != asc_none)
    {
      if (exec_opt.model_params & MOD_MASK_RATE_PARAMS)
      {
        if (exec_opt.model_params & (MOD_PARAM_INV | MOD_PARAM_INV_GAMMA))
        {
          LOG_ERR << PACKAGE << ": Ascertainment bias correction is not compatible with +I/+I+G models" << endl;
          params_ok = false;
        }
      }
      else
      {
        exec_opt.model_params |= MOD_PARAM_NO_RATE_VAR | MOD_PARAM_GAMMA;
      }
    }

    /* validate input file */
    if (input_file_ok)
    {
      if (!modeltest::Utils::file_exists(exec_opt.msa_filename))
      {
        LOG_ERR << PACKAGE << ": msa file does not exist: " << exec_opt.msa_filename << endl;
        params_ok = false;
      }
    }
    else
    {
      LOG_ERR << PACKAGE << ": You must specify an alignment file (-i)" << endl;
      params_ok = false;
    }

    if (exec_opt.tree_filename.compare(""))
    {
      if (!modeltest::Utils::file_exists(exec_opt.tree_filename))
      {
        LOG_ERR << PACKAGE << ": tree file does not exist: " << exec_opt.tree_filename << endl;
        params_ok = false;
      }
      else if (exec_opt.starting_tree == tree_mp ||
               exec_opt.starting_tree == tree_random)
      {
        LOG_ERR << PACKAGE << ": Warning: "
          "User defined tree (-u) is used only with ml, fixed-ml or fixed-user starting trees" <<
          endl;
        LOG_ERR << PACKAGE << ":          "
            "Starting tree will be set to 'user'" << endl;
        exec_opt.starting_tree = tree_user_fixed;
      }
    }
    else if (exec_opt.starting_tree == tree_user_fixed)
    {
      LOG_ERR << PACKAGE <<
        ": Fixed-user starting tree (-t user) requires a tree file (-u)" <<
        endl;
      params_ok = false;
    }

    /* validate partitions file */
    if (exec_opt.partitions_filename.compare(""))
    {
      if (!modeltest::Utils::file_exists(exec_opt.partitions_filename))
      {
        LOG_ERR << PACKAGE << ": partitions file does not exist: " <<
                exec_opt.partitions_filename << endl;
        params_ok = false;
      }
      else
      {
        exec_opt.partitions_desc =
                modeltest::Utils::parse_partitions_file(
                    exec_opt.partitions_filename);
        if (!exec_opt.partitions_desc)
        {
          switch (modeltest::mt_errno)
          {
          case MT_ERROR_IO:
              LOG_ERR << PACKAGE << ": Cannot read partitions file: "
                   << exec_opt.partitions_filename << endl;
              break;
          case MT_ERROR_IO_FORMAT:
              LOG_ERR << PACKAGE << ": Cannot parse partitions: "
                   << exec_opt.partitions_filename << endl;
              LOG_ERR <<  setw(strlen(PACKAGE) + 2) << setfill(' ')
                   << " [" << modeltest::mt_errno << "] "
                   << modeltest::mt_errmsg << endl;
              break;
          case MT_ERROR_PARTITIONS_OVERFLOW:
              LOG_ERR << PACKAGE << ": Partitions overflow: "
                   << exec_opt.partitions_filename << endl;
              LOG_ERR <<  setw(strlen(PACKAGE) + 2) << setfill(' ')
                   << " [" << modeltest::mt_errno << "] "
                   << modeltest::mt_errmsg << endl;
              break;
          default:
              assert(0);
          }
          params_ok = false;
        }
      }

      if (params_ok)
      {
        if (!modeltest::ModelTest::test_partitions(*exec_opt.partitions_desc,
                                                   exec_opt.n_sites))
        {
          LOG_ERR << PACKAGE << ": Error in partitions file: "
               << exec_opt.partitions_filename << endl;
          LOG_ERR <<  setw(strlen(PACKAGE) + 2) << setfill(' ')
               << " [" << modeltest::mt_errno << "] "
               << modeltest::mt_errmsg << endl;
          params_ok = false;
        }
        else
        {
            if (modeltest::mt_errno)
            {
                LOG_ERR << PACKAGE << ": Warning: "
                     << modeltest::mt_errmsg << endl;
                modeltest::mt_errno = 0;
            }
        }

        assert(exec_opt.partitions_desc);
        if (exec_opt.partitions_desc->size() > 1 &&
            !(exec_opt.asc_bias_corr == asc_none ||
              exec_opt.asc_bias_corr == asc_lewis))
        {
          LOG_ERR << PACKAGE <<
               ": Only None or Lewis ascertainment bias correction is allowed for partitioned data sets so far."
               << endl;
          params_ok = false;
        }

        for (partition_descriptor_t & partition : (*exec_opt.partitions_desc))
        {
          assert(partition.unique_id > 0);
          partition.model_params = exec_opt.model_params;
          partition.states = (partition.datatype == dt_dna?N_DNA_STATES:
                                                           N_PROT_STATES);
          exist_dna_models     |= (partition.datatype == dt_dna);
          exist_protein_models |= (partition.datatype == dt_protein);
          partition.n_categories = exec_opt.n_catg;
          partition.gap_aware = gap_aware;
          partition.asc_bias_corr = exec_opt.asc_bias_corr;
          partition.asc_weights = exec_opt.asc_weights;
          partition.starting_tree = exec_opt.starting_tree;
        }
      }
    }
    else
    {
      /* create single partition / single region */
      exec_opt.partitions_desc = new vector<partition_descriptor_t>();
      partition_region_t region;
      partition_descriptor_t partition;
      region.start = 1;
      region.end = exec_opt.n_sites;
      region.stride = 1;
      partition.datatype = arg_datatype;
      partition.states = (partition.datatype == dt_dna?N_DNA_STATES:
                                                       N_PROT_STATES);
      exist_dna_models     = (arg_datatype == dt_dna);
      exist_protein_models = (arg_datatype == dt_protein);
      partition.n_categories = exec_opt.n_catg;
      partition.gap_aware = gap_aware;
      partition.partition_name = "DATA";
      partition.regions.push_back(region);
      partition.model_params = exec_opt.model_params;
      partition.asc_bias_corr = exec_opt.asc_bias_corr;
      partition.asc_weights = exec_opt.asc_weights;
      partition.starting_tree = exec_opt.starting_tree;
      partition.unique_id = 1;
      exec_opt.partitions_desc->push_back(partition);
    }

    /* set models template */
    exec_opt.template_models = template_models;
    if (template_models != template_none)
    {
      for (partition_descriptor_t & partition : (*exec_opt.partitions_desc))
      {
        partition.model_params =
          modeltest::Utils::get_parameters_from_template(template_models, partition.datatype)
          & exec_opt.model_params;
        switch (partition.datatype)
        {
        case dt_dna:
        {
            dna_ss = modeltest::Utils::get_dna_matrices_from_template(template_models);
        }
            break;
        case dt_protein:
        {
            mt_size_t n_prot_matrices;
            const mt_index_t *template_matrices = modeltest::Utils::get_prot_matrices_from_template(template_models,
                                                                                                    &n_prot_matrices);
            for (mt_index_t i=0; i<n_prot_matrices; ++i)
                exec_opt.aa_candidate_models.push_back(template_matrices[i]);
        }
            break;
        }
      }
    }

    if (exist_protein_models)
    {
      if (exec_opt.starting_tree == tree_ml_gtr_fixed)
      {
        LOG_ERR << PACKAGE <<
          ": GTR fixed ML tree is not available for protein models" <<
          endl;
        params_ok = false;
      }
      if (!exist_dna_models && (dna_ss != ss_undef))
      {
        LOG_ERR << PACKAGE << ": Warning: Substitution schemes will be ignored" << endl;
      }

      if (user_candidate_models.compare(""))
      {
        int prot_matrices_bitv_add = 0;
        int prot_matrices_bitv_rm = 0;

        bool edit_mode = !user_candidate_models.substr(0,1).compare("-")
                      || !user_candidate_models.substr(0,1).compare("+");
        char action = '+';

        if(edit_mode)
        {
          for (int i=0; i<N_PROT_MODEL_MATRICES; i++)
            prot_matrices_bitv_add |= 1<<prot_model_matrices_indices[i];
        }
        else
        {
          if (exec_opt.aa_candidate_models.size())
            exec_opt.aa_candidate_models.clear();
        }

        /* parse user defined models */
        istringstream f(user_candidate_models);
        string s;
        while (getline(f, s, ',')) {
            mt_index_t i, c_matrix = 0;
            if (edit_mode)
            {
              char cur_action = s.c_str()[0];
              if (!(cur_action == '+' || cur_action == '-'))
              {
                LOG_ERR << PACKAGE
                        << ": Warning: Undefined action {+,-} for model " << s
                        << ". Using previous action: " << action <<  endl;
              }
              else
              {
                action = cur_action;
                s = s.substr(1);
              }
            }

            std::transform(s.begin(), s.end(), s.begin(),
                 [](unsigned char c){ return std::toupper(c); });

            for (i=0; i<N_PROT_MODEL_ALL_MATRICES; i++)
            {
                if (!prot_model_names[i].compare(s))
                {
                    c_matrix = i;
                    break;
                }
            }
            if (i == N_PROT_MODEL_ALL_MATRICES)
            {
                LOG_ERR << PACKAGE << ": Invalid protein matrix: " << s << endl;
                params_ok = false;
            }

            assert (action == '+' || action == '-');
            if (action == '+')
              prot_matrices_bitv_add |= 1<<c_matrix;
            else
              prot_matrices_bitv_rm |= 1<<c_matrix;
        }
        prot_matrices_bitv_rm = ~prot_matrices_bitv_rm;

        for (mt_index_t i=0; i<N_PROT_MODEL_ALL_MATRICES; i++)
        {
            if ((prot_matrices_bitv_add&1) && (prot_matrices_bitv_rm&1))
            {
              exec_opt.aa_candidate_models.push_back(i);
            }

            prot_matrices_bitv_add >>= 1;
            prot_matrices_bitv_rm >>= 1;
        }
      }
      else if (!exec_opt.aa_candidate_models.size())
      {
        /* the whole set is used */
        for (mt_index_t i=0; i<N_PROT_MODEL_MATRICES; i++)
            exec_opt.aa_candidate_models.push_back(i);
      }
    }

    if (exist_dna_models)
    {
      if (user_candidate_models.compare("") && (dna_ss == ss_undef))
      {
        int dna_matrices_bitv_add = 0;
        int dna_matrices_bitv_rm = 0;

        bool edit_mode = !user_candidate_models.substr(0, 1).compare("-")
          || !user_candidate_models.substr(0, 1).compare("+");
        if (edit_mode)
        {
          for (int i = 0; i < 2*N_DNA_MODEL_MATRICES; i++)
            dna_matrices_bitv_add |= 1 << i;
        }
        else
        {
          if (exec_opt.nt_candidate_models.size())
            exec_opt.nt_candidate_models.clear();
        }

        /* parse user defined models */
        istringstream f(user_candidate_models);
        string s;
        while (getline(f, s, ',')) {
          mt_index_t i, c_matrix = 0;
          char action = '+';

          if (edit_mode)
          {
            action = s.c_str()[0];
            s = s.substr(1);
          }

          std::transform(s.begin(), s.end(), s.begin(),
               [](unsigned char c){ return std::toupper(c); });

          for (i=0; i<N_DNA_MODEL_MATRICES; i++)
          {
            if (!uc_dna_model_names[2*i].compare(s) ||
                !uc_dna_model_names[2*i+1].compare(s))
            {
              c_matrix = i;
              break;
            }
          }
          if (i == N_DNA_MODEL_MATRICES)
          {
            LOG_ERR << PACKAGE << ": Invalid dna matrix: " << s << endl;
            params_ok = false;
          }

          assert (action == '+' || action == '-');
          if (action == '+')
            dna_matrices_bitv_add |= 1<<c_matrix;
          else
            dna_matrices_bitv_rm |= 1<<c_matrix;
        }
        dna_matrices_bitv_rm = ~dna_matrices_bitv_rm;

        for (mt_index_t i=0; i<N_DNA_MODEL_MATRICES; i++)
        {
          if ((dna_matrices_bitv_add&1) && (dna_matrices_bitv_rm&1))
          {
            exec_opt.nt_candidate_models.push_back(
                        dna_model_matrices_indices[i]);
          }
          dna_matrices_bitv_add >>= 1;
          dna_matrices_bitv_rm >>= 1;
        }
      }
      else
      {
        if (dna_ss == ss_undef)
            exec_opt.subst_schemes = default_ss;
        else
            exec_opt.subst_schemes = dna_ss;

        exec_opt.rate_clustering = (dna_ss == ss_203);
      }

      /* fill candidate matrices */
      if(exec_opt.rate_clustering)
      {
        /* For rate clustering, fill candidate models only with GTR */
        exec_opt.nt_candidate_models.push_back(DNA_GTR_INDEX);
      }
      else
      {
        switch(exec_opt.subst_schemes)
        {
        case ss_11:
          for (mt_index_t i=0; i<N_DNA_MODEL_MATRICES; i++)
              exec_opt.nt_candidate_models.push_back(dna_model_matrices_indices[i]);
          break;
        case ss_7:
          exec_opt.nt_candidate_models.push_back(dna_model_matrices_indices[6]);
          exec_opt.nt_candidate_models.push_back(dna_model_matrices_indices[9]);

          exec_opt.nt_candidate_models.push_back(dna_model_matrices_indices[2]);
          exec_opt.nt_candidate_models.push_back(dna_model_matrices_indices[3]);
          exec_opt.nt_candidate_models.push_back(dna_model_matrices_indices[0]);
          exec_opt.nt_candidate_models.push_back(dna_model_matrices_indices[1]);
          exec_opt.nt_candidate_models.push_back(dna_model_matrices_indices[10]);
          break;
        case ss_5:
          exec_opt.nt_candidate_models.push_back(dna_model_matrices_indices[2]);
          exec_opt.nt_candidate_models.push_back(dna_model_matrices_indices[3]);

          exec_opt.nt_candidate_models.push_back(dna_model_matrices_indices[0]);
          exec_opt.nt_candidate_models.push_back(dna_model_matrices_indices[1]);
          exec_opt.nt_candidate_models.push_back(dna_model_matrices_indices[10]);
          break;
        case ss_3:
          exec_opt.nt_candidate_models.push_back(dna_model_matrices_indices[0]);
          exec_opt.nt_candidate_models.push_back(dna_model_matrices_indices[1]);
          exec_opt.nt_candidate_models.push_back(dna_model_matrices_indices[10]);
          break;
        case ss_203:
          for (mt_index_t i=0; i<203; i++)
              exec_opt.nt_candidate_models.push_back(i);
          break;
        case ss_undef:
          /* ignore */
          break;
        }
      }
    }

    if (params_ok)
    {
      assert(exec_opt.nt_candidate_models.size() > 0 ||
             exec_opt.aa_candidate_models.size() > 0);

      /* set output files */
      if (!output_basename.compare(""))
          output_basename = exec_opt.msa_filename;

      exec_opt.output_log_file =
      exec_opt.output_tree_file =
      exec_opt.output_results_file =
      exec_opt.output_results_file =
      exec_opt.output_models_file =
      exec_opt.output_topos_file =
      exec_opt.output_raxml_part_file =
              output_basename;

      exec_opt.output_log_file.append(OUTPUT_LOG_SUFFIX);
      exec_opt.output_tree_file.append(OUTPUT_TREE_SUFFIX);
      exec_opt.output_results_file.append(OUTPUT_RESULTS_SUFFIX);
      exec_opt.output_models_file.append(OUTPUT_MODELS_SUFFIX);
      exec_opt.output_topos_file.append(OUTPUT_TOPOS_SUFFIX);
      exec_opt.output_raxml_part_file.append(OUTPUT_RAXML_PART_SUFFIX);

      exec_opt.output_tree_to_file = (exec_opt.starting_tree != tree_user_fixed);

      //TODO: Temporary checkpoint enabled by default
      if (exec_opt.write_checkpoint)
      {
        exec_opt.checkpoint_file = output_basename;
        exec_opt.checkpoint_file.append(CHECKPOINT_SUFFIX);
      }

      /* validate output files */
      output_files_ok = true;
      if (!modeltest::Utils::file_writable(exec_opt.output_log_file))
      {
        LOG_ERR << PACKAGE << ": Log file " <<  exec_opt.output_log_file << " cannot be open for writing" << endl;
        output_files_ok = false;
      }
      if (exec_opt.starting_tree == tree_ml && !modeltest::Utils::file_writable(exec_opt.output_topos_file))
      {
        LOG_ERR << PACKAGE << ": Topologies file " <<  exec_opt.output_topos_file << " cannot be open for writing" << endl;
        output_files_ok = false;
      }
      if (exec_opt.output_tree_to_file && !modeltest::Utils::file_writable(exec_opt.output_tree_file))
      {
        LOG_ERR << PACKAGE << ": Tree file " <<  exec_opt.output_tree_file << " cannot be open for writing" << endl;
        output_files_ok = false;
      }

      if (!exec_opt.force_override && modeltest::Utils::file_exists(exec_opt.output_results_file))
      {
        ifstream fin;
        fin.open(exec_opt.output_results_file);
        if (fin.is_open())
        {
          string line;
          fin.seekg(-5, ios_base::end);
          getline(fin, line);
          if (!line.compare("Done"))
          {
            LOG_ERR << PACKAGE << ": Results file " <<  exec_opt.output_results_file << " already exists" << endl;
            output_files_ok = false;
          }
        }
      }
      else
      {
          if (!modeltest::Utils::file_writable(exec_opt.output_results_file))
          {
              LOG_ERR << PACKAGE << ": Results file " <<  exec_opt.output_results_file << " cannot be open for writing" << endl;
              output_files_ok = false;
          }
      }
      if (!output_files_ok)
      {
          LOG_ERR << PACKAGE << ": - Remove the existing files, or" << endl;
          LOG_ERR << PACKAGE << ": - Select a different output basename (-o argument), or" << endl;
          LOG_ERR << PACKAGE << ": - Force overriding (--force argument)" << endl;

          modeltest::mt_errno = MT_ERROR_IGNORE;
          params_ok = false;
      }
    }

    return params_ok;
}


void Meta::print_ascii_logo(std::ostream  &out)
{
  out << "                             _      _ _            _      _   _  _____ " << endl;
  out << "                            | |    | | |          | |    | \\ | |/ ____|" << endl;
  out << "         _ __ ___   ___   __| | ___| | |_ ___  ___| |_   |  \\| | |  __ " << endl;
  out << "        | '_ ` _ \\ / _ \\ / _` |/ _ \\ | __/ _ \\/ __| __|  | . ` | | |_ |" << endl;
  out << "        | | | | | | (_) | (_| |  __/ | ||  __/\\__ \\ |_   | |\\  | |__| |" << endl;
  out << "        |_| |_| |_|\\___/ \\__,_|\\___|_|\\__\\___||___/\\__|  |_| \\_|\\_____|" << endl;
}

void Meta::print_header(std::ostream  &out)
{
    out << setw(80) << setfill('-') << ""  << setfill(' ') << endl;
    print_version(out);
    out << setw(80) << setfill('-') << ""  << setfill(' ') << endl << endl;
}

void Meta::print_version(std::ostream  &out)
{
    out << "ModelTest-NG v" << MTNG_VERSION << " released on " << MTNG_DATE
        << " by The Exelixis Lab." << endl;
    out << "Written by Diego Darriba." << endl;
    out << "Contributors: Tomas Flouri, Alexey Kozlov, Benoit Morel, David Posada, "
        << endl << "              Alexandros Stamatakis." << endl;
    out << "Latest version: https://github.com/ddarriba/modeltest" << endl;
}

void Meta::print_system_info(std::ostream  &out)
{
//    double memcount_gb = (double)(get_memtotal() >> 30);
    double memcount_gb = ((double) modeltest::Utils::get_memtotal() / BYTE_TO_GB);
    out << "Physical cores: " << modeltest::Utils::count_physical_cores() << endl;
    out << "Logical cores:  " << modeltest::Utils::count_logical_cores() << endl;
    out << "Memory:         " << setprecision(3) << memcount_gb << "GB" << endl;
    out << "Extensions:     ";
    if (have_avx)
      out << "AVX" << endl;
    else if (have_sse3)
      out << "SSE3" << endl;
    else
      out << "none [WARNING!]" << endl;
}

static void print_model_params(mt_mask_t model_params, std::ostream  &out)
{
    out << "  " << left << setw(20) << "include model parameters:" << endl;
    out << "    " << left << setw(17) << "Uniform:" << ((model_params&MOD_PARAM_NO_RATE_VAR)?"true":"false") << endl;
    out << "    " << left << setw(17) << "p-inv (+I):" << ((model_params&MOD_PARAM_INV)?"true":"false") << endl;
    out << "    " << left << setw(17) << "gamma (+G):" << ((model_params&MOD_PARAM_GAMMA)?"true":"false") << endl;
    out << "    " << left << setw(17) << "both (+I+G):" << ((model_params&MOD_PARAM_INV_GAMMA)?"true":"false") << endl;
    out << "    " << left << setw(17) << "free rates (+R):" << ((model_params&MOD_PARAM_FREE_RATES)?"true":"false") << endl;
    out << "    " << left << setw(17) << "fixed freqs:" << ((model_params&MOD_PARAM_FIXED_FREQ)?"true":"false") << endl;
    out << "    " << left << setw(17) << "estimated freqs:" << ((model_params&MOD_PARAM_ESTIMATED_FREQ)?"true":"false") << endl;
}

void Meta::print_options(mt_options_t & opts, ostream &out)
{
    mt_size_t num_cores = modeltest::Utils::count_physical_cores();
    out << setw(80) << setfill('-') << ""  << setfill(' ') << endl;
    out << "ModelTest-NG v" << MTNG_VERSION << endl << endl;
    out << "Input data:" << endl;
    out << "  " << left << setw(12) << "MSA:" << opts.msa_filename << endl;
    out << "  " << left << setw(12) << "Tree:";
    switch(opts.starting_tree)
    {
    case tree_user_fixed:
        assert (opts.tree_filename.compare(""));
        out << "Fixed user" << endl;
        break;
    case tree_mp:
        out << "Maximum parsimony" << endl;
        break;
    case tree_ml_gtr_fixed:
        out << "Fixed ML GTR" << endl;
        break;
    case tree_ml_jc_fixed:
        out << "Fixed ML JC" << endl;
        break;
    case tree_ml:
        out << "Maximum likelihood" << endl;
        break;
    case tree_random:
        out << "Random" << endl;
        break;
    }
    out << "  " << left << setw(18) << "  file:";
    if (opts.tree_filename.compare(""))
    {
      out << opts.tree_filename << endl;
    }
    else
    {
      out << "-" << endl;
    }

    out << "  " << left << setw(18) << "#taxa:" << opts.n_taxa << endl;
    out << "  " << left << setw(18) << "#sites:" << opts.n_sites << endl;
    if (opts.partitions_eff)
    {
      mt_size_t n_patterns = 0;
      mt_size_t max_memb = 0;
      for (mt_index_t i=0; i<opts.partitions_eff->size(); ++i)
      {
        n_patterns += opts.partitions_eff->at(i).site_patterns;
        mt_size_t memb = modeltest::Utils::mem_size(opts.n_taxa,
                        opts.partitions_eff->at(i).site_patterns,
                        opts.n_catg,
                        opts.partitions_eff->at(i).states);
        if (memb > max_memb)
          max_memb = memb;
      }

      if (opts.partitions_eff->size() > 1)
        out << "  " << left << setw(18) << "#patterns (sum):";
      else
        out << "  " << left << setw(18) << "#patterns:";
      out << n_patterns << endl;
      out << "  " << left << setw(18) << "Max. thread mem:"
          << max_memb / (1024*1024) << " MB" << endl;
    }

    out << endl << "Output:" << endl;
    out << "  " << left << setw(15) << "Log:" << opts.output_log_file << endl;
    if (opts.output_tree_to_file)
    {
      out << "  " << left << setw(15) << "Starting tree:" << opts.output_tree_file << endl;
    }
    out << "  " << left << setw(15) << "Results:" << opts.output_results_file << endl;

    out << endl << "Selection options:" << endl;
    if (opts.nt_candidate_models.size())
    {
        out << "  " << left << setw(20) << "# dna schemes:" << opts.nt_candidate_models.size() << endl;
        mt_mask_t model_params = opts.model_params;
        if (opts.partitions_desc)
            for (partition_descriptor_t part : *opts.partitions_desc)
                if (part.datatype == dt_dna)
                {
                    model_params = part.model_params;
                    break;
                }
        out << "  " << left << setw(20) << "# dna models:"
            << modeltest::Utils::number_of_models((mt_size_t)opts.nt_candidate_models.size(), model_params)
            << endl;
        print_model_params(model_params, out);
    }
    if (opts.aa_candidate_models.size())
    {
        out << "  " << left << setw(20) << "# protein matrices:" << opts.aa_candidate_models.size() << endl;
        mt_mask_t model_params = opts.model_params;
        if (opts.partitions_desc)
            for (partition_descriptor_t part : *opts.partitions_desc)
                if (part.datatype == dt_protein)
                {
                    model_params = part.model_params;
                    break;
                }
        out << "  " << left << setw(20) << "# protein models:"
            << modeltest::Utils::number_of_models((mt_size_t)opts.aa_candidate_models.size(), model_params)
            << endl;
        print_model_params(model_params, out);

    }
    if (opts.model_params&(MOD_PARAM_GAMMA | MOD_PARAM_INV_GAMMA | MOD_PARAM_FREE_RATES))
    {
      out << "    " << left << setw(17) << "#categories:" << opts.n_catg << endl;
    }
    out << "  " << left << setw(20) << "gamma rates mode:";
    switch(opts.gamma_rates_mode)
    {
      case PLL_GAMMA_RATES_MEAN:
        out << "mean" << endl;
        break;
      case PLL_GAMMA_RATES_MEDIAN:
        out << "median" << endl;
        break;
      default:
        out << "undefined" << endl;
        break;
    }
    out << "  " << left << setw(20) << "asc bias:";
    switch(opts.asc_bias_corr)
    {
      case asc_none:
        out << "none" << endl;
        break;
      case asc_lewis:
        out << "lewis" << endl;
        break;
      case asc_felsenstein:
        out << "felsenstein" << endl;
        if (opts.asc_weights[0])
        {
          out << "    " << left << setw(18) << "weight: " << opts.asc_weights[0] << endl;
        }
        break;
      case asc_stamatakis:
        {
          out << "stamatakis" << endl;
          out << "    " << left << setw(18) << "weights: ";
          bool weights_undef = true;
          for (int i=0; i<MT_MAX_STATES; ++i)
            if (opts.asc_weights[i])
            {
              out << "[" << i << "]" << opts.asc_weights[i] << " ";
              weights_undef = false;
            }
          if (weights_undef)
          {
            out << "-";
          }
          out << endl;
        }
        break;
    }

    out << "  " << left << setw(20) << "epsilon (opt):" << opts.epsilon_opt << endl;
    out << "  " << left << setw(20) << "epsilon (par):" << opts.epsilon_param << endl;
    out << "  " << left << setw(20) << "keep branches:" << (opts.keep_model_parameters?"true":"false") << endl;

    out << endl << "Additional options:" << endl;
    out << "  " << left << setw(18) << "verbosity:";
    switch (opts.verbose)
    {
    case 0:
        out << "very low" << endl;
        break;
    case VERBOSITY_LOW:
        out << "low" << endl;
        break;
    case VERBOSITY_MID:
        out << "medium" << endl;
        break;
    case VERBOSITY_HIGH:
        out << "high" << endl;
        break;
    case VERBOSITY_ULTRA:
        out << "ultra" << endl;
        break;
    default:
        assert(0);
    }
    out << "  " << left << setw(18) << "threads:" << opts.n_threads << "/" << num_cores << endl;
    out << "  " << left << setw(18) << "RNG seed:" << opts.rnd_seed << endl;
    out << "  " << left << setw(18) << "subtree repeats:" <<
           (modeltest::disable_repeats?"disabled":"enabled") << endl;
    if (opts.verbose == VERBOSITY_MID)
    {
      out << "  " << left << setw(18) << "parameters mask:" << opts.model_params<< endl;
    }

    if (opts.partitions_desc)
    {
        if (opts.partitions_filename.compare(""))
        {
          out << left << setw(15) << "Partitions:" << opts.partitions_filename << endl;
          out << left << setw(15) << "" << opts.partitions_desc->size() << " partitions" << endl;
          mt_size_t n_prot_parts = 0;
          mt_size_t n_dna_parts = 0;
          if (opts.verbose > VERBOSITY_DEFAULT)
          {
            out << setw(15) << " " << setw(10) << setfill('-') << ""
                << setfill(' ') << endl;
            for (mt_index_t i=0; i<opts.partitions_desc->size(); i++)
            {
                out << left << setw(15) << " "
                    << setw(4) << right << i+1 << " ";
                switch (opts.partitions_desc->at(i).datatype)
                {
                case dt_dna:
                    out << "[NT] ";
                    n_dna_parts++;
                    break;
                case dt_protein:
                    out << "[AA] ";
                    n_prot_parts++;
                    break;
                }
                out << opts.partitions_desc->at(i).partition_name << " : ";
                for (partition_region_t & region : opts.partitions_desc->at(i).regions)
                {
                    out << region.start << "-" << region.end;
                    if (region.stride != 1)
                    {
                        out << "/" << region.stride;
                    }
                    out << " ";
                }
                out << endl;
            }
            out << setw(15) << " " << setw(10) << setfill('-') << ""
                << setfill(' ') << endl;
          }
          else
          {
            for (mt_index_t i=0; i<opts.partitions_desc->size(); i++)
            {
                switch (opts.partitions_desc->at(i).datatype)
                {
                case dt_dna:
                    n_dna_parts++;
                    break;
                case dt_protein:
                    n_prot_parts++;
                    break;
                }
            }
          }
          if (n_dna_parts)
          {
            out << left << setw(15) << " "
                << setw(4) << right << n_dna_parts
                << " DNA partitions" << endl;
          }
          if (n_prot_parts)
          {
            out << left << setw(15) << " "
                << setw(4) << right << n_prot_parts
                << " protein partitions" << endl;
          }
        }
    }
    out << setw(80) << setfill('-') << "" << setfill(' ') << endl;
}

void Meta::print_usage(std::ostream& out)
{
    out << "Usage: " << PACKAGE << " -i sequenceFilename" << endl;
    out << "            [-c n_categories] [-d nt|aa] [-F] [-N]"
        << endl;
    out << "            [-p numberOfThreads] [-q partitionsFile]"
        << endl;
    out << "            [-t mp|fixed|user] [-u treeFile] [-v] [-V]" << endl;
    out << "            [-T raxml|phyml|mrbayes|paup]" << endl;
    out << "            [--eps optimizationEpsilonValue] [--tol parameterTolerance]" << endl;
}

void Meta::print_help(std::ostream& out)
{
    out << "selects the best-fit model of amino acid or nucleotide replacement."
        << endl << endl;
    out
            << "mandatory arguments for long options are also mandatory for short options."
            << endl;
    out << endl;

    /************************************************************/

    out << endl << " Main arguments:" << endl;
    out << setw(MAX_OPT_LENGTH) << left << "  -d, --datatype data_type_t"
        << "sets the data type" << endl;
    out << setw(SHORT_OPT_LENGTH) << " " << setw(COMPL_OPT_LENGTH)
        << "           nt"
        << "nucleotide" << endl;
    out << setw(SHORT_OPT_LENGTH) << " " << setw(COMPL_OPT_LENGTH)
        << "           aa"
        << "amino acid" << endl;

    out << setw(MAX_OPT_LENGTH) << left << "  -i, --input input_msa"
        << "sets the input alignment file (FASTA or PHYLIP format, required)" << endl;

    out << setw(MAX_OPT_LENGTH) << left << "  -o, --output output_file"
        << "pipes the output into a file" << endl;

    out << setw(MAX_OPT_LENGTH) << left << "  -p, --threads n_threads"
        << "sets the number of threads to use (shared memory)" << endl;

    out << setw(MAX_OPT_LENGTH) << left << "  -q, --partitions partitions_file"
        << "sets a partitioning scheme" << endl;

    out << setw(MAX_OPT_LENGTH) << left << "  -r, --rngseed seed"
        << "sets the seed for the random number generator" << endl;

    out << setw(MAX_OPT_LENGTH) << left << "  -t, --topology type"
        << "sets the starting topology" << endl;
    out << setw(SHORT_OPT_LENGTH) << " " << setw(COMPL_OPT_LENGTH)
        << "           ml"
        << "maximum likelihood" << endl;
    out << setw(SHORT_OPT_LENGTH) << " " << setw(COMPL_OPT_LENGTH)
        << "           mp"
        << "maximum parsimony" << endl;
    out << setw(SHORT_OPT_LENGTH) << " " << setw(COMPL_OPT_LENGTH)
        << "           fixed-ml-jc"
        << "fixed maximum likelihood (JC)" << endl;
    out << setw(SHORT_OPT_LENGTH) << " " << setw(COMPL_OPT_LENGTH)
        << "           fixed-ml-gtr"
        << "fixed maximum likelihood (GTR)" << endl;
    out << setw(SHORT_OPT_LENGTH) << " " << setw(COMPL_OPT_LENGTH)
        << "           fixed-mp"
        << "fixed maximum parsimony (default)" << endl;
    out << setw(SHORT_OPT_LENGTH) << " " << setw(COMPL_OPT_LENGTH)
        << "           random"
        << "random generated tree" << endl;
    out << setw(SHORT_OPT_LENGTH) << " " << setw(COMPL_OPT_LENGTH)
        << "           user"
        << "fixed user defined (requires -u argument)" << endl;

    out << setw(MAX_OPT_LENGTH) << left << "  -u, --utree tree_file"
        << "sets a user tree" << endl;
    out << setw(MAX_OPT_LENGTH) << left << "      --force"
        << "force output overriding" << endl;
    out << setw(MAX_OPT_LENGTH) << left << "      --disable-checkpoint"
        << "disable checkpoint writing" << endl;

    /************************************************************/

    out << endl << " Candidate models:" << endl;

    out << setw(MAX_OPT_LENGTH) << left << "  -a, --asc-bias algorithm[:values]"
        << "includes ascertainment bias correction" << endl;
    out << setw(MAX_OPT_LENGTH) << left << " "
        << "  check modeltest manual for more information" << endl;
    out << setw(SHORT_OPT_LENGTH) << " " << setw(COMPL_OPT_LENGTH)
        << "           lewis"
        << "Lewis (2001)" << endl;
    out << setw(SHORT_OPT_LENGTH) << " " << setw(COMPL_OPT_LENGTH)
        << "           felsenstein"
        << "Felsenstein" << endl;
    out << setw(MAX_OPT_LENGTH) << left << " "
        << "  requires number of invariant sites" << endl;
    out << setw(SHORT_OPT_LENGTH) << " " << setw(COMPL_OPT_LENGTH)
        << "           stamatakis" << "Leaché et al. (2015)" << endl;
    out << setw(MAX_OPT_LENGTH) << left << " "
        << "  requires invariant sites composition" << endl;

    out << setw(MAX_OPT_LENGTH) << left << "  -f, --frequencies [ef]"
        << "sets the candidate models frequencies" << endl;
    out << setw(MAX_OPT_LENGTH) << left << " "
        << "e: estimated - maximum likelihood (DNA) / empirical (AA)" << endl;
    out << setw(MAX_OPT_LENGTH) << left << " "
        << "f: fixed - equal (DNA) / model defined (AA)" << endl;

    out << setw(MAX_OPT_LENGTH) << left << "  -h, --model-het [uigf]"
        << "sets the candidate models rate heterogeneity" << endl;
    out << setw(MAX_OPT_LENGTH) << left << " "
        << "u: *uniform" << endl;
    out << setw(MAX_OPT_LENGTH) << left << " "
        << "i: *proportion of invariant sites (+I)" << endl;
    out << setw(MAX_OPT_LENGTH) << left << " "
        << "g: *discrite Gamma rate categories (+G)" << endl;
    out << setw(MAX_OPT_LENGTH) << left << " "
        << "f: *both +I and +G (+I+G)" << endl;
    out << setw(MAX_OPT_LENGTH) << left << " "
        << "r: free rate models (+R)" << endl;
    out << setw(MAX_OPT_LENGTH) << left << " "
        << "* included by default" << endl;

    out << setw(MAX_OPT_LENGTH) << left << "  -m, --models list"
        << "sets the candidate model matrices separated by commas." << endl;
    out << setw(MAX_OPT_LENGTH) << left << " "
            << "use '+' or '-' prefix for updating the default list." << endl;
    out << setw(MAX_OPT_LENGTH) << left << " "
                << "e.g., \"-m JTT,LG\" evaluates JTT and LG only ." << endl;
    out << setw(MAX_OPT_LENGTH) << left << " "
                << "      \"-m +LG4X,+LG4M,-LG\" adds LG4 models and removes LG and from the list." << endl;
    out << setw(MAX_OPT_LENGTH) << left << " "
            << "dna:";
    for (int i=0; i<N_DNA_MODEL_MATRICES; ++i)
    {
      const string modelname = i
          ?(dna_model_names[2*i].compare(dna_model_names[2*i+1])<0
              ?dna_model_names[2*i]
              :dna_model_names[2*i+1])
           :dna_model_names[0];
      out << " *" << modelname;
      if ((i+1) % 6 == 0)
        out << endl << setw(MAX_OPT_LENGTH + 4) << left << " ";
    }
    out << endl;
    out << setw(MAX_OPT_LENGTH) << left << " "
        << "protein:";
    int prot_list_len = 0;
    for (int i=0; i<N_PROT_MODEL_ALL_MATRICES; ++i)
    {
      prot_list_len += prot_model_names[i].length() + 1;
      if (prot_list_len > 44)
      {
        out << endl << setw(MAX_OPT_LENGTH + 8) << left << " ";
        prot_list_len = prot_model_names[i].length() + 1;
      }
      out << (i<N_PROT_MODEL_MATRICES?" *":" ") << prot_model_names[i];
    }
    out << endl;
    out << setw(MAX_OPT_LENGTH) << left << " "
                    << "* included by default" << endl;

    out << setw(MAX_OPT_LENGTH) << left << "  -s, --schemes [3|5|7|11|203]"
        << "sets the number of predefined DNA substitution schemes evaluated" << endl;
    out << setw(SHORT_OPT_LENGTH) << " " << setw(COMPL_OPT_LENGTH)
        << " " << "3:   JC/F81, K80/HKY, SYM/GTR" << endl;
    out << setw(SHORT_OPT_LENGTH) << " " << setw(COMPL_OPT_LENGTH)
        << " " << "5:   + TrNef/TrN, TPM1/TPM1uf" << endl;
    out << setw(SHORT_OPT_LENGTH) << " " << setw(COMPL_OPT_LENGTH)
        << " " << "7:   + TIM1ef/TIM1, TVMef/TVM" << endl;
    out << setw(SHORT_OPT_LENGTH) << " " << setw(COMPL_OPT_LENGTH)
        << " " << "11:  + TPM2/TPM2uf, TPM3/TPM3uf, TIM2ef/TIM2, TIM3ef/TIM3" << endl;
    out << setw(SHORT_OPT_LENGTH) << " " << setw(COMPL_OPT_LENGTH)
        << " " << "203: All possible GTR submatrices" << endl;

    out << setw(MAX_OPT_LENGTH) << left << "  -T, --template [tool]"
        << "sets candidate models according to a specified tool" << endl;
    out << setw(SHORT_OPT_LENGTH) << " " << setw(COMPL_OPT_LENGTH)
        << "           raxml"
        << "RAxML (DNA 3 schemes / AA full search)" << endl;
    out << setw(SHORT_OPT_LENGTH) << " " << setw(COMPL_OPT_LENGTH)
        << "           phyml"
        << "PhyML (DNA full search / 14 AA matrices)" << endl;
    out << setw(SHORT_OPT_LENGTH) << " " << setw(COMPL_OPT_LENGTH)
        << "           mrbayes"
        << "MrBayes (DNA 3 schemes / 8 AA matrices)" << endl;
    out << setw(SHORT_OPT_LENGTH) << " " << setw(COMPL_OPT_LENGTH)
        << "           paup"
        << "PAUP* (DNA full search / AA full search)" << endl;

    /************************************************************/

    /*
    // NOT YET AVAILABLE

    out << endl << " Partitioning scheme search:" << endl;
    out << setw(MAX_OPT_LENGTH) << left << "      --psearch algorithm"
        << "sets the partitioning scheme search algorithm" << endl;
    out << setw(SHORT_OPT_LENGTH) << " " << setw(COMPL_OPT_LENGTH)
        << "          kn"
        << "k=n" << endl;
    out << setw(SHORT_OPT_LENGTH) << " " << setw(COMPL_OPT_LENGTH)
        << "          hcluster"
        << "hierarchical clustering (req. 2n operations)" << endl;
    out << setw(SHORT_OPT_LENGTH) << " " << setw(COMPL_OPT_LENGTH)
        << "          greedy"
        << "greedy search (req. up to n^2 operations)" << endl;
    out << setw(SHORT_OPT_LENGTH) << " " << setw(COMPL_OPT_LENGTH)
        << "          kmeans"
        << "additive kmeans algorithm (unavailable)" << endl;
    */

    /************************************************************/

    out << endl << " Other options:" << endl;
    out << setw(MAX_OPT_LENGTH) << left << "      --eps epsilon_value"
        << "sets the model optimization epsilon" << endl;
    out << setw(MAX_OPT_LENGTH) << left << "      --tol tolerance_value"
        << "sets the parameter optimization tolerance" << endl;
    out << setw(MAX_OPT_LENGTH) << left << "      --smooth-frequencies"
        << "forces frequencies smoothing" << endl;

    out << setw(MAX_OPT_LENGTH) << left << "  -g, --gamma-rates [a|g]"
        << "sets gamma rates mode" << endl;
    out << setw(SHORT_OPT_LENGTH) << " " << setw(COMPL_OPT_LENGTH)
        << "               a"
        << "uses the average (or mean) per category (default)" << endl;
    out << setw(SHORT_OPT_LENGTH) << " " << setw(COMPL_OPT_LENGTH)
        << "               m"
        << "uses the median per category" << endl;
    out << setw(MAX_OPT_LENGTH) << left << "      --disable-checkpoint"
        << "does not create checkpoint files" << endl;
    out << setw(MAX_OPT_LENGTH) << left << "  -H, --no-compress"
        << "disables pattern compression" << endl;
    out << setw(MAX_OPT_LENGTH) << left << " "
        << PACKAGE << " ignores if there are missing states" << endl;
    out << setw(MAX_OPT_LENGTH) << left << "  -k, --keep-params"
        << "keep branch lengths fixed" << endl;
    out << setw(MAX_OPT_LENGTH) << left << "  -v, --version"
        << "output version information and exit" << endl;
    out << setw(MAX_OPT_LENGTH) << left << "      --quiet"
        << "run in quiet mode (only essential information shown)" << endl;
    out << setw(MAX_OPT_LENGTH) << left << "  -V, --verbose"
        << "run in verbose mode" << endl;
    out << setw(MAX_OPT_LENGTH) << left << "      --ultraverbose"
        << "run in ultra verbose mode" << endl;

    out << setw(MAX_OPT_LENGTH) << left << "      --help"
        << "display this help message and exit" << endl;
    out << endl;


    out << "Exit status:" << endl;
    out << " 0  if OK," << endl;
    out << " 1  if minor problems (e.g., invalid arguments or data)," << endl;
    out << " 2  if serious trouble (e.g., execution crashed)." << endl;
    out << endl;
    out << "Report " << PACKAGE << " bugs to diego.darriba@h-its.org" << endl;
    out << "ModelTest home page: <http://www.github.com/ddarriba/modeltest/>" << endl;
}
