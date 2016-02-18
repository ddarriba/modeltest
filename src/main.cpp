#ifndef _NO_GUI_
#include "gui/xmodeltest.h"
#include "service/modeltestservice.h"
#include <QApplication>
#else
#include "service/modeltestservice.h"
#endif
#include "utils.h"

#include <iostream>
#include <ctime>
#include <sstream>
#include <cerrno>
#include <getopt.h>


#include <vector>
#include <chrono>

#include "thread/threadpool.h"

#define MAX_OPT_LENGTH 40
#define SHORT_OPT_LENGTH 6
#define COMPL_OPT_LENGTH MAX_OPT_LENGTH-SHORT_OPT_LENGTH

ModelTestService *ModelTestService::s_instance = 0;

using namespace std;

/** number of parallel processes */
static mt_size_t n_procs = 1;

static void print_usage(std::ostream& out)
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

static void print_help(std::ostream& out)
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
        << "sets the input alignment file (FASTA format, required)" << endl;

    out << setw(MAX_OPT_LENGTH) << left << "  -o, --output output_file"
        << "pipes the output into a file" << endl;

    out << setw(MAX_OPT_LENGTH) << left << "  -p, --processes n_procs"
        << "sets the number of processors to use (shared memory)" << endl;

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
        << "fixed maximum parsimony" << endl;
    out << setw(SHORT_OPT_LENGTH) << " " << setw(COMPL_OPT_LENGTH)
        << "           user"
        << "fixed user defined (requires -u argument)" << endl;

    out << setw(MAX_OPT_LENGTH) << left << "  -u, --utree tree_file"
        << "sets a user tree" << endl;

    /************************************************************/

    out << endl << " Candidate models:" << endl;

    out << setw(MAX_OPT_LENGTH) << left << "  -c, --categories num_cat"
        << "sets the number of gamma rate categories" << endl;

    out << setw(MAX_OPT_LENGTH) << left << "  -f, --frequencies [ef]"
        << "sets the candidate models frequencies" << endl;
    out << setw(MAX_OPT_LENGTH) << left << " "
        << "e: estimated - maximum likelihood (DNA) / empirical (AA)" << endl;
    out << setw(MAX_OPT_LENGTH) << left << " "
        << "f: fixed - equal (DNA) / model defined (AA)" << endl;

    out << setw(MAX_OPT_LENGTH) << left << "  -h, --model-het [uigf]"
        << "sets the candidate models rate heterogeneity" << endl;
    out << setw(MAX_OPT_LENGTH) << left << " "
        << "u: uniform" << endl;
    out << setw(MAX_OPT_LENGTH) << left << " "
        << "i: proportion of invariant sites (+I)" << endl;
    out << setw(MAX_OPT_LENGTH) << left << " "
        << "g: discrite Gamma rate categories (+G)" << endl;
    out << setw(MAX_OPT_LENGTH) << left << " "
        << "f: both +I and +G (+I+G)" << endl;

    out << setw(MAX_OPT_LENGTH) << left << "  -m, --models list"
        << "sets the candidate model matrices separated by commas" << endl;
    out << setw(MAX_OPT_LENGTH) << left << " "
        << "dna: JC HKY TrN TPM1 TPM2 TPM3" << endl;
    out << setw(MAX_OPT_LENGTH) << left << " "
        << "     TIM1 TIM2 TIM3 TVM GTR" << endl;
    out << setw(MAX_OPT_LENGTH) << left << " "
        << "protein: DAYHOFF LG DCMUT JTT MTREV WAG RTREV CPREV" << endl;
    out << setw(MAX_OPT_LENGTH) << left << " "
        << "         VT BLOSUM62 MTMAM MTART MTZOA PMB HIBV HIVW" << endl;
    out << setw(MAX_OPT_LENGTH) << left << " "
        << "         JTTDCMUT FLU SMTREV" << endl;

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

    /************************************************************/

    out << endl << " Other options:" << endl;
    out << setw(MAX_OPT_LENGTH) << left << "      --eps epsilon_value"
        << "sets the model optimization epsilon" << endl;
    out << setw(MAX_OPT_LENGTH) << left << "      --tol tolerance_value"
        << "sets the parameter optimization tolerance" << endl;
    out << setw(MAX_OPT_LENGTH) << left << "      --smooth-frequencies"
        << "forces frequencies smoothing" << endl;
    out << setw(MAX_OPT_LENGTH) << left << " "
        << PACKAGE << " ignores if there are missing states" << endl;
    out << setw(MAX_OPT_LENGTH) << left << "  -v, --verbose"
        << "run in verbose mode" << endl;

    out << setw(MAX_OPT_LENGTH) << left << "      --help"
        << "display this help message and exit" << endl;
    out << setw(MAX_OPT_LENGTH) << left << "      --version"
        << "output version information and exit" << endl;
    out << endl;


    out << "Exit status:" << endl;
    out << " 0  if OK," << endl;
    out << " 1  if minor problems (e.g., invalid arguments or data)," << endl;
    out << " 2  if serious trouble (e.g., execution crashed)." << endl;
    out << endl;
    out << "Report " << PACKAGE << " bugs to diego.darriba@h-its.org" << endl;
    out << "ModelTest home page: <http://www.github.com/ddarriba/modeltest/>" << endl;
}

static bool parse_arguments(int argc, char *argv[], mt_options_t & exec_opt)
{
    bool input_file_ok = false;
    bool exist_dna_models = false;
    bool exist_protein_models = false;
    template_models_t template_models = template_none;
    dna_subst_schemes_t dna_ss = ss_undef;
    string user_candidate_models = "";

    /* defaults */
    dna_subst_schemes_t default_ss = ss_11;

    /* set default options */
    data_type_t arg_datatype   = dt_dna;
    exec_opt.n_catg          = DEFAULT_GAMMA_RATE_CATS;
    exec_opt.epsilon_param   = DEFAULT_PARAM_EPSILON;
    exec_opt.epsilon_opt     = DEFAULT_OPT_EPSILON;
    exec_opt.rnd_seed        = DEFAULT_RND_SEED;
    exec_opt.model_params    = 0;
    exec_opt.smooth_freqs    = false;
    exec_opt.subst_schemes   = ss_undef;
    exec_opt.partitions_desc = NULL;
    exec_opt.partitions_eff  = NULL;
    exec_opt.verbose         = VERBOSITY_DEFAULT;
    exec_opt.n_threads       = 1;
    exec_opt.starting_tree   = tree_mp;

    static struct option long_options[] =
    {
        { "categories", required_argument, 0, 'c' },
        { "datatype", required_argument, 0, 'd' },
        { "model-freqs", required_argument, 0, 'f' },
        { "model-het", required_argument, 0, 'h' },
        { "input", required_argument, 0, 'i' },
        { "models", required_argument, 0, 'm' },
        { "output", required_argument, 0, 'o' },
        { "processes", required_argument, 0, 'p' },
        { "partitions", required_argument, 0, 'q' },
        { "rngseed", required_argument, 0, 'r' },
        { "schemes", required_argument, 0, 's' },
        { "template", required_argument, 0, 'T' },
        { "tree", required_argument, 0, 't' },
        { "utree", required_argument, 0, 'u' },
        { "verbose", no_argument, 0, 'v' },
        { "help", no_argument, 0, 0 },
        { "usage", no_argument, 0, 1 },
        { "version", no_argument, 0, 2 },
        { "psearch", required_argument, 0, 3 },
        { "smooth-frequencies", no_argument, 0, 4 },
        { "eps", required_argument, 0, 10 },
        { "tol", required_argument, 0, 11 },
        { 0, 0, 0, 0 }
    };

    int opt = 0, long_index = 0;
    while ((opt = getopt_long(argc, argv, "c:d:f:h:i:m:o:p:q:r:s:t:T:u:v", long_options,
                              &long_index)) != -1) {
        switch (opt) {
        case 0:
            print_usage(cout);
            cout << endl;
            print_help(cout);
            return false;
        case 1:
            print_usage(cout);
            return false;
        case 2:
            modeltest::Utils::print_version(cout);
            return false;
        case 3:
            /* partitioning search */
            //TODO
            if (!strcasecmp(optarg, "kn"))
            {
                cerr << "Search K=N" << endl;
            }
            else if (!strcasecmp(optarg, "hcluster"))
            {
                cerr << "Search hierarchical clustering" << endl;
            }
            else if (!strcasecmp(optarg, "greedy"))
            {
                cerr << "Search greedy" << endl;
            }
            else if (!strcasecmp(optarg, "kn"))
            {
                cerr << "Search K=N" << endl;
            }
            else
            {
                cerr <<  PACKAGE << ": Invalid scheme search algorithm: " << optarg << endl;
                return false;
            }
            assert(0);
            break;
        case 4:
            /* force frequencies smoothing */
            exec_opt.smooth_freqs = true;
            break;
        case 10:
            exec_opt.epsilon_opt = atof(optarg);
            if (exec_opt.epsilon_opt <= 0.0)
            {
                cerr << PACKAGE << ": Invalid optimization epsilon: " << exec_opt.epsilon_opt << endl;
                return false;
            }
            break;
        case 11:
            exec_opt.epsilon_param = atof(optarg);
            if (exec_opt.epsilon_param <= 0.0)
            {
                cerr << PACKAGE << ": Invalid parameter tolerance: " << exec_opt.epsilon_param << endl;
                return false;
            }
            break;
        case 'c':
            exec_opt.n_catg = (mt_size_t) atoi(optarg);
            if (exec_opt.n_catg <= 0)
            {
                cerr << PACKAGE << ": Invalid number of categories: " << exec_opt.n_catg << endl;
                return false;
            }
            break;
        case 'd':
            if (!strcasecmp(optarg, "nt"))
            {
                arg_datatype = dt_dna;
            }
            else if (!strcasecmp(optarg, "aa"))
            {
                arg_datatype = dt_protein;
            }
            else
            {
                cerr <<  PACKAGE << ": Invalid datatype " << optarg << endl;
                return false;
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
                    exec_opt.model_params  |= MOD_PARAM_FIXED_FREQ;
                    break;
                case 'e':
                case 'E':
                    /* ML freqs (DNA) / model defined freqs (AA) */
                    exec_opt.model_params  |= MOD_PARAM_ESTIMATED_FREQ;
                    break;
                default:
                    cerr <<  PACKAGE << ": Unrecognised rate heterogeneity parameter " << optarg[i] << endl;
                    return false;
                }
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
                default:
                    cerr <<  PACKAGE << ": Unrecognised rate heterogeneity parameter " << optarg[i] << endl;
                    return false;
                }
            }
            break;
        case 'i':
            exec_opt.msa_filename = optarg;
            input_file_ok = strcmp(optarg, "");
            break;
        case 'm':
        {
            user_candidate_models = optarg;
            break;
        }
        case 'o':
            exec_opt.output_filename = optarg;
            break;
        case 'p':
            n_procs = (mt_size_t) atoi(optarg);
            exec_opt.n_threads = n_procs;
            if (n_procs <= 0)
            {
                cerr << PACKAGE << ": Invalid number of parallel processes: " << optarg << endl;
                return false;
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
                cerr << PACKAGE << ": Invalid number of substitution schemes " << optarg << endl;
                return false;
            }
            break;
        case 't':
            if (!strcasecmp(optarg, "user"))
            {
                exec_opt.starting_tree = tree_user_fixed;
            }
            else if (!strcasecmp(optarg, "mp"))
            {
                exec_opt.starting_tree = tree_mp;
            }
            else if (!strcasecmp(optarg, "ml"))
            {
                exec_opt.starting_tree = tree_ml;
            }
            else if (!strcasecmp(optarg, "fixed-ml-gtr"))
            {
                exec_opt.starting_tree = tree_ml_gtr_fixed;
            }
            else if (!strcasecmp(optarg, "fixed-ml-jc"))
            {
                exec_opt.starting_tree = tree_ml_jc_fixed;
            }
            else
            {
                cerr << PACKAGE << ": ERROR: Invalid starting topology " << optarg << endl;
                return false;
            }
            break;
        case 'T':
            /* load template */
            if (!strcasecmp(optarg, "raxml"))
            {
                template_models = template_raxml;
            }
            else if (!strcasecmp(optarg, "phyml"))
            {
                template_models = template_phyml;
            }
            else if (!strcasecmp(optarg, "mrbayes"))
            {
                template_models = template_mrbayes;
            }
            else if (!strcasecmp(optarg, "paup"))
            {
                template_models = template_paup;
            }
            else
            {
                cerr <<  PACKAGE << ": Invalid template: " << optarg << endl;
                return false;
            }
            break;
        case 'u':
            exec_opt.tree_filename = optarg;
            exec_opt.starting_tree = tree_user_fixed;
            break;
        case 'v':
            exec_opt.verbose = VERBOSITY_HIGH;
            break;
        default:
            exit(1);
            //cerr << "Unrecognised argument -" << opt << endl;
            //return false;
        }
    }

    srand(exec_opt.rnd_seed);

    /* validate input file */
    if (input_file_ok) {
        if (!modeltest::MsaPll::test(exec_opt.msa_filename,
                                    &exec_opt.n_taxa,
                                    &exec_opt.n_sites))
        {
            cerr << PACKAGE << ": Cannot parse the msa: " << exec_opt.msa_filename << endl;
            cerr << PACKAGE << ": [" << pll_errno << "]: " << pll_errmsg << endl;
            return false;
        }
    }
    else
    {
        cerr << PACKAGE << ": You must specify an alignment file (-i)" << endl;
        return false;
    }

    /* validate partitions file */
    if (exec_opt.partitions_filename.compare(""))
    {
        exec_opt.partitions_desc =
                modeltest::Utils::parse_partitions_file(
                    exec_opt.partitions_filename);
        if (!exec_opt.partitions_desc)
        {
            switch (modeltest::mt_errno)
            {
            case MT_ERROR_IO:
                cerr << PACKAGE << ": Cannot read partitions file: "
                     << exec_opt.partitions_filename << endl;
                break;
            case MT_ERROR_IO_FORMAT:
                cerr << PACKAGE << ": Cannot parse partitions: "
                     << exec_opt.partitions_filename << endl;
                break;
            default:
                assert(0);
            }
            return false;
        }

        if (!modeltest::ModelTest::test_partitions(*exec_opt.partitions_desc,
                                                   exec_opt.n_sites))
        {
            cerr << PACKAGE << ": Error in partitions file: "
                 << exec_opt.partitions_filename << endl;
            cerr << modeltest::mt_errmsg << endl;
            return false;
        }
        else
        {
            if (modeltest::mt_errno)
            {
                cerr << PACKAGE << ": Warning: "
                     << modeltest::mt_errmsg << endl;
                modeltest::mt_errno = 0;
            }
        }

        assert(exec_opt.partitions_desc);
        for (partition_descriptor_t & partition : (*exec_opt.partitions_desc))
        {
            partition.model_params = exec_opt.model_params;
            partition.states = (partition.datatype == dt_dna?N_DNA_STATES:
                                                             N_PROT_STATES);
            exist_dna_models     |= (partition.datatype == dt_dna);
            exist_protein_models |= (partition.datatype == dt_protein);
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
        partition.partition_name = "DATA";
        partition.regions.push_back(region);
        partition.model_params = exec_opt.model_params;
        exec_opt.partitions_desc->push_back(partition);
    }

    /* set models template */
    exec_opt.template_models = template_models;
    if (template_models != template_none)
    {
        for (partition_descriptor_t & partition : (*exec_opt.partitions_desc))
        {
            partition.model_params = modeltest::Utils::get_parameters_from_template(template_models, partition.datatype);
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
        if (!exist_dna_models && (dna_ss != ss_undef))
            cerr << PACKAGE << ": Warning: Substitution schemes will be ignored" << endl;

        if (user_candidate_models.compare(""))
        {
            if (exec_opt.aa_candidate_models.size())
                exec_opt.aa_candidate_models.clear();

            int prot_matrices_bitv = 0;
            /* parse user defined models */
            istringstream f(user_candidate_models);
            string s;
            while (getline(f, s, ',')) {
                mt_index_t i, c_matrix = 0;
                for (i=0; i<N_PROT_MODEL_ALL_MATRICES; i++)
                {
                    if (!strcasecmp(prot_model_names[i].c_str(), s.c_str()))
                    {
                        c_matrix = i;
                        break;
                    }
                }
                if (i == N_PROT_MODEL_ALL_MATRICES)
                {
                    cerr << PACKAGE << ": Invalid protein matrix: " << s << endl;
                    return false;
                }
                prot_matrices_bitv |= 1<<c_matrix;
            }
            for (mt_index_t i=0; i<N_PROT_MODEL_ALL_MATRICES; i++)
            {
                if (prot_matrices_bitv&1)
                {
                    exec_opt.aa_candidate_models.push_back(i);
                }
                prot_matrices_bitv >>= 1;
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
            if (exec_opt.nt_candidate_models.size())
                exec_opt.nt_candidate_models.clear();

            int dna_matrices_bitv = 0;
            /* parse user defined models */
            istringstream f(user_candidate_models);
            string s;
            while (getline(f, s, ',')) {
                mt_index_t i, c_matrix = 0;
                for (i=0; i<N_DNA_MODEL_MATRICES; i++)
                {
                    if (!strcasecmp(dna_model_names[2*i].c_str(), s.c_str()) ||
                        !strcasecmp(dna_model_names[2*i+1].c_str(), s.c_str()))
                    {
                        c_matrix = i;
                        break;
                    }
                }
                if (i == N_DNA_MODEL_MATRICES)
                {
                    cerr << PACKAGE << ": Invalid dna matrix: " << s << endl;
                    return false;
                }
                dna_matrices_bitv |= 1<<c_matrix;
            }
            for (mt_index_t i=0; i<N_DNA_MODEL_MATRICES; i++)
            {
                if (dna_matrices_bitv&1)
                {
                    exec_opt.nt_candidate_models.push_back(
                                dna_model_matrices_indices[i]);
                }
                dna_matrices_bitv >>= 1;
            }
        }
        else
        {
            if (dna_ss == ss_undef)
                exec_opt.subst_schemes = default_ss;
            else
                exec_opt.subst_schemes = dna_ss;
        }

        /* fill candidate matrices */
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

    assert(exec_opt.nt_candidate_models.size() > 0 ||
           exec_opt.aa_candidate_models.size() > 0);

    /* if there are no model specifications, include all */
    mt_mask_t all_params = MOD_PARAM_NO_RATE_VAR |
                           MOD_PARAM_INV |
                           MOD_PARAM_GAMMA |
                           MOD_PARAM_INV_GAMMA;
    if (!(exec_opt.model_params &
          all_params))
        exec_opt.model_params |=
                all_params;

    /* if there are no frequencies specifications, include all */
    if (!(exec_opt.model_params &
         (MOD_PARAM_FIXED_FREQ | MOD_PARAM_ESTIMATED_FREQ)))
        exec_opt.model_params |= (MOD_PARAM_FIXED_FREQ | MOD_PARAM_ESTIMATED_FREQ);

    return true;
}

int main(int argc, char *argv[])
{
    int return_val = EXIT_SUCCESS;

    if (argc > 1)
    {
        /* command line */
        mt_options_t opts;
        mt_size_t num_cores = modeltest::Utils::count_physical_cores();
        time_t ini_global_time = time(NULL);

        if (!parse_arguments(argc, argv, opts))
        {
            return(EXIT_FAILURE);
        }

        if (!ModelTestService::instance()->create_instance(opts))
        {
            cerr << modeltest::mt_errmsg << endl;
            return (int)modeltest::mt_errno;
        }

        cout << endl;
        modeltest::Utils::print_options(opts, cout);
        cout << PACKAGE << " was called as follows: " << endl << ">> ";
        for (int i=0; i<argc; i++)
            cout << argv[i] << " ";
        cout << endl << endl;

        if (n_procs == 1 && num_cores > 1)
        {
            /* We warn only if the number of processors is 1. */
            /* Otherwise we assume that the user is aware of this feature */
            cerr << PACKAGE << ": Warning: You are using one single thread out of "
                    << num_cores << " physical cores." << endl;
            cerr << PACKAGE
                 << ":          You can set the number of threads with -p argument."
                 << endl;
            cerr << PACKAGE << ": Try '" << PACKAGE << " --help' or '"
                 << PACKAGE << " --usage' for more information" << endl;
        }

        vector<map<modeltest::ic_type, modeltest::selection_model>> best_models(opts.partitions_eff->size());

        for(mt_index_t i=0; i<opts.partitions_eff->size(); i++)
        {

            cout << endl << "Partition "
                 << i+1 << "/" << opts.partitions_eff->size()
                 << endl << endl;

            partition_id_t part_id = {i};
            ModelTestService::instance()->evaluate_models(part_id, n_procs,
                               opts.epsilon_param, opts.epsilon_opt);

            std::cout << std::endl << "Computation of likelihood scores completed. It took " <<
                         modeltest::Utils::format_time(time(NULL) - ini_global_time) << std::endl;

            modeltest::ModelSelection * bic_selection = ModelTestService::instance()->select_models(part_id, modeltest::ic_bic);
            bic_selection->print(cout, 10);
            cout << "Best model according to BIC" << endl;
            cout << "---------------------------" << endl;
            bic_selection->print_best_model(cout);
            cout << "---------------------------" << endl;
            cout << "Parameter importances" << endl;
            cout << "---------------------------" << endl;
            bic_selection->print_importances(cout);
            cout << endl;
            cout << ModelTestService::instance()->get_raxml_command_line(*(bic_selection->get_model(0).model)) << endl;
            best_models[i][modeltest::ic_bic] = bic_selection->get_model(0);
            delete bic_selection;

            modeltest::ModelSelection * aic_selection = ModelTestService::instance()->select_models(part_id, modeltest::ic_aic);
            aic_selection->print(cout, 10);
            cout << "Best model according to AIC" << endl;
            cout << "---------------------------" << endl;
            aic_selection->print_best_model(cout);
            cout << "---------------------------" << endl;
            cout << "Parameter importances" << endl;
            cout << "---------------------------" << endl;
            aic_selection->print_importances(cout);
            cout << endl;
            best_models[i][modeltest::ic_aic] = aic_selection->get_model(0);
            delete aic_selection;

            modeltest::ModelSelection * aicc_selection = ModelTestService::instance()->select_models(part_id, modeltest::ic_aicc);
            aicc_selection->print(cout, 10);
            cout << "Best model according to AICc" << endl;
            cout << "----------------------------" << endl;
            aicc_selection->print_best_model(cout);
            cout << "---------------------------" << endl;
            cout << "Parameter importances" << endl;
            cout << "---------------------------" << endl;
            aicc_selection->print_importances(cout);
            cout << endl;
            best_models[i][modeltest::ic_aicc] = aicc_selection->get_model(0);
            delete aicc_selection;

            /* ignore DT if topology is not fixed */
            if (opts.starting_tree != tree_ml)
            {
                modeltest::ModelSelection * dt_selection = ModelTestService::instance()->select_models(part_id, modeltest::ic_dt);
                dt_selection->print(cout, 10);
                cout << "Best model according to DT" << endl;
                cout << "--------------------------" << endl;
                dt_selection->print_best_model(cout);
                cout << "---------------------------" << endl;
                cout << "Parameter importances" << endl;
                cout << "---------------------------" << endl;
                dt_selection->print_importances(cout);
                cout << endl;
                best_models[i][modeltest::ic_dt] = dt_selection->get_model(0);
                delete dt_selection;
            }
        }

        cout << "Summary:" << endl;
        for(mt_index_t i=0; i<opts.partitions_eff->size(); i++)
        {
            cout <<  endl << "Partition " << i+1 << "/" << opts.partitions_eff->size() << ":" << endl;
            modeltest::ModelSelection::print_inline_header(cout);
            modeltest::ModelSelection::print_inline_best_model(modeltest::ic_bic, best_models[i][modeltest::ic_bic], cout);
            modeltest::ModelSelection::print_inline_best_model(modeltest::ic_aic, best_models[i][modeltest::ic_aic], cout);
            modeltest::ModelSelection::print_inline_best_model(modeltest::ic_aicc, best_models[i][modeltest::ic_aicc], cout);
            if (opts.starting_tree != tree_ml)
                modeltest::ModelSelection::print_inline_best_model(modeltest::ic_dt, best_models[i][modeltest::ic_dt], cout);
//            cout << "phyml " << ModelTestService::instance()->get_phyml_command_line(*best_models[i][modeltest::ic_bic].model, opts) << endl;
//            cout << "raxmlHPC-SSE3 " << ModelTestService::instance()->get_raxml_command_line(*best_models[i][modeltest::ic_bic].model, opts.msa_filename) << endl;
        }

        /* clean */
        if (opts.partitions_desc)
            delete opts.partitions_desc;
        if (opts.partitions_eff)
            delete opts.partitions_eff;

        ModelTestService::instance()->destroy_instance();
    }
    else
    {
        #ifndef _NO_GUI_
        /* launch GUI */
        QApplication a(argc, argv);
        //Q_INIT_RESOURCE(mtgraphics);

        xmodeltest xmt;
        xmt.show();
//        modeltest::jModelTest w;
//        w.show();

        return_val = a.exec();
        #else
        cerr << PACKAGE << " was compiled without GUI support" << endl;
        cerr << "Try '" << PACKAGE << " --help' or '" << PACKAGE << " --usage' for more information" << endl;
        #endif
    }

    return return_val;
}
