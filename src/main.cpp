#ifndef CONSOLE
#include "modeltest_gui.h"
#include <QApplication>
#else
#include "modeltest.h"
#endif
#include "utils.h"

#include <iostream>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <cerrno>
#include <getopt.h>

using namespace std;

/** number of parallel processes */
static mt_size_t n_procs = 1;

static bool parse_arguments(int argc, char *argv[], mt_options & exec_opt)
{
    bool input_file_ok = false;
    bool exist_dna_models = false;
    bool exist_protein_models = false;
    dna_subst_schemes dna_ss = ss_undef;
    string user_candidate_models = "";

    /* defaults */
    dna_subst_schemes default_ss = ss_11;

    /* set default options */
    data_type arg_datatype   = dt_dna;
    exec_opt.n_catg          = DEFAULT_GAMMA_RATE_CATS;
    exec_opt.epsilon_param   = DEFAULT_PARAM_EPSILON;
    exec_opt.epsilon_opt     = DEFAULT_OPT_EPSILON;
    exec_opt.rnd_seed        = DEFAULT_RND_SEED;
    exec_opt.model_params    = 0;
    exec_opt.subst_schemes   = ss_undef;
    exec_opt.partitions_desc = NULL;
    exec_opt.partitions_eff  = NULL;
    exec_opt.verbose         = VERBOSITY_DEFAULT;
    exec_opt.n_threads       = 1;

    static struct option long_options[] =
    {
        { "categories", required_argument, 0, 'c' },
        { "datatype", required_argument, 0, 'd' },
        { "epsilon", required_argument, 0, 'e' },
        { "model-freqs", required_argument, 0, 'F' },
        { "help", no_argument, 0, 'h' },
        { "model-het", required_argument, 0, 'H' },
        { "input", required_argument, 0, 'i' },
        { "models", required_argument, 0, 'm' },
        { "tree", required_argument, 0, 't' },
        { "partitions", required_argument, 0, 'q' },
        { "rngseed", required_argument, 0, 'r' },
        { "schemes", required_argument, 0, 'S' },
        { "processes", required_argument, 0, 'p' },
        { "output", required_argument, 0, 'o' },
        { "verbose", no_argument, 0, 'v' },
        { 0, 0, 0, 0 }
    };

    int opt = 0, long_index = 0;
    while ((opt = getopt_long(argc, argv, "c:d:e:F:hH:i:m:o:p:q:r:S:t:v", long_options,
                              &long_index)) != -1) {
        switch (opt) {
        case 'c':
            exec_opt.n_catg = (mt_size_t) atoi(optarg);
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
                cerr << "ERROR: Invalid datatype " << optarg << endl;
                return false;
            }
            break;
        case 'e':
            exec_opt.epsilon_opt = atof(optarg);
            exec_opt.epsilon_param = atof(optarg);
            break;
        case 'F':
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
                    cerr << "ERROR: Unrecognised rate heterogeneity parameter " << optarg[i] << endl;
                    return false;
                }
            }
            break;
        case 'h':
            cerr << "ModelTest Help:" << endl;
            return false;
        case 'H':
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
                    cerr << "Unrecognised rate heterogeneity parameter " << optarg[i] << endl;
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
            if (n_procs <= 0)
            {
                cerr <<"Invalid number of parallel processes: " << optarg << endl;
                return false;
            }
            break;
        case 'q':
            exec_opt.partitions_filename = optarg;
            break;
        case 'r':
            exec_opt.rnd_seed = (unsigned int) atoi(optarg);
            break;
        case 'S':
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
                cerr << "Invalid number of substitution schemes " << optarg << endl;
                return false;
            }
            break;
        case 't':
            exec_opt.tree_filename = optarg;
            exec_opt.starting_tree = tree_user_fixed;
            break;
        case 'v':
            exec_opt.verbose = VERBOSITY_HIGH;
            break;
        default:
            cerr << "Unrecognised argument -" << opt << endl;
            return false;
        }
    }

    srand(exec_opt.rnd_seed);

    /* validate input file */
    if (input_file_ok) {
        if (!modeltest::MsaPll::test(exec_opt.msa_filename,
                                    &exec_opt.n_taxa,
                                    &exec_opt.n_sites))
        {
            cerr << "Error parsing alignment: " << exec_opt.msa_filename << endl;
            return false;
        }
    }
    else
    {
        cerr << "Alignment file (-i) is required" << endl;
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
                cerr << "Cannot read partitions file: "
                     << exec_opt.partitions_filename << endl;
                break;
            case MT_ERROR_IO_FORMAT:
                cerr << "Error parsing partitions: "
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
            cerr << "Error in partitions file: "
                 << exec_opt.partitions_filename << endl;
            cerr << modeltest::mt_errmsg << endl;
            return false;
        }
        else
        {
            if (modeltest::mt_errno)
            {
                cerr << "WARNING: "
                     << modeltest::mt_errmsg << endl;
                modeltest::mt_errno = 0;
            }
        }

        assert(exec_opt.partitions_desc);
        for (partition_t & partition : (*exec_opt.partitions_desc))
        {
            exist_dna_models     |= (partition.datatype == dt_dna);
            exist_protein_models |= (partition.datatype == dt_protein);
        }
    }
    else
    {
        /* create single partition / single region */
        exec_opt.partitions_desc = new vector<partition_t>();
        partition_region_t region;
        partition_t partition;
        region.start = 1;
        region.end = exec_opt.n_sites;
        region.stride = 1;
        partition.datatype = arg_datatype;
        exist_dna_models     = (arg_datatype == dt_dna);
        exist_protein_models = (arg_datatype == dt_protein);
        partition.partition_name = "DATA";
        partition.regions.push_back(region);
        exec_opt.partitions_desc->push_back(partition);
    }

    if (exist_protein_models)
    {
        if (!exist_dna_models && (dna_ss != ss_undef))
            cerr << "Warning: Substitution schemes will be ignored" << endl;

        if (user_candidate_models.compare(""))
        {
            int prot_matrices_bitv = 0;
            /* parse user defined models */
            istringstream f(user_candidate_models);
            string s;
            while (getline(f, s, ',')) {
                mt_index_t i, c_matrix = 0;
                for (i=0; i<N_PROT_MODEL_MATRICES; i++)
                {
                    if (!strcasecmp(prot_model_names[i].c_str(), s.c_str()))
                    {
                        c_matrix = i;
                        break;
                    }
                }
                if (i == N_PROT_MODEL_MATRICES)
                {
                    cerr << "Invalid protein matrix: " << s << endl;
                    return false;
                }
                prot_matrices_bitv |= 1<<c_matrix;
            }
            for (mt_index_t i=0; i<N_PROT_MODEL_MATRICES; i++)
            {
                if (prot_matrices_bitv&1)
                {
                    exec_opt.aa_candidate_models.push_back(i);
                }
                prot_matrices_bitv >>= 1;
            }
        }
        else
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
                    cerr << "Invalid dna matrix: " << s << endl;
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
    if (!(exec_opt.model_params &
          ~(MOD_PARAM_FIXED_FREQ | MOD_PARAM_ESTIMATED_FREQ)))
        exec_opt.model_params =
                MOD_PARAM_NO_RATE_VAR |
                MOD_PARAM_INV |
                MOD_PARAM_GAMMA |
                MOD_PARAM_INV_GAMMA;

    /* if there are no frequencies specifications, include all */
    if (!(exec_opt.model_params &
         (MOD_PARAM_FIXED_FREQ | MOD_PARAM_ESTIMATED_FREQ)))
        exec_opt.model_params |= MOD_PARAM_FIXED_FREQ;

    return true;
}

int main(int argc, char *argv[])
{
    int return_val = EXIT_SUCCESS;
    time_t ini_global_time = time(NULL);

    if (argc > 1)
    {
        /* command line */
        mt_options opts;
        mt_index_t cur_model;

        if (!parse_arguments(argc, argv, opts))
        {
            return(EXIT_FAILURE);
        }

        modeltest::ModelTest mt(n_procs);

        if (!mt.build_instance(opts))
        {
            cerr << modeltest::mt_errmsg << endl;
            return (int)modeltest::mt_errno;
        }

        cout << endl;
        modeltest::Utils::print_options(opts, cout);
        cout << endl;

        //mt.evaluate_models();
        for(mt_index_t i=0; i<opts.partitions_eff->size(); i++)
        {

            cout << endl << "Partition " << i+1 << "/" << opts.partitions_eff->size() << endl << endl;

            partition_id_t part_id = {i};
            cur_model = 0;
            for (cur_model=0; cur_model<mt.get_models(part_id).size(); cur_model++)
            {
                modeltest::Model *model = mt.get_models(part_id)[cur_model];
                time_t ini_t = time(NULL);
                if (!mt.evaluate_single_model(model,
                                              part_id,
                                              0,
                                              opts.epsilon_param,
                                              opts.epsilon_opt))
                {
                    cerr << "ERROR OPTIMIZING MODEL" << endl;
                    return(MT_ERROR_OPTIMIZE);
                }

                /* print progress */
                cout << setw(5) << right << (cur_model+1) << "/"
                     << setw(5) << left << mt.get_models(part_id).size()
                     << setw(15) << left << model->get_name()
                     << setw(18) << right << setprecision(MT_PRECISION_DIGITS) << fixed
                     << model->get_lnl()
                     << setw(8) << time(NULL) - ini_t
                     << setw(8) << time(NULL) - ini_global_time << endl;
            }

            modeltest::ModelSelection bic_selection(mt.get_models(part_id),
                                                    modeltest::ic_bic);
            bic_selection.print(cout);
            cout << "Best model according to BIC" << endl;
            cout << "---------------------------" << endl;
            bic_selection.print_best_model(cout);
            cout << endl;

            modeltest::ModelSelection aic_selection(mt.get_models(part_id),
                                                    modeltest::ic_aic);
            aic_selection.print(cout, 10);
            cout << "Best model according to AIC" << endl;
            cout << "---------------------------" << endl;
            aic_selection.print_best_model(cout);
            cout << endl;

            modeltest::ModelSelection aicc_selection(mt.get_models(part_id),
                                                    modeltest::ic_aicc);
            aicc_selection.print(cout, 10);
            cout << "Best model according to AICc" << endl;
            cout << "----------------------------" << endl;
            aicc_selection.print_best_model(cout);
            cout << endl;

            /* ignore DT if topology is fixed */
            if (opts.starting_tree == tree_ml)
            {
                modeltest::ModelSelection dt_selection(mt.get_models(part_id),
                                                    modeltest::ic_dt);
                dt_selection.print(cout, 10);
                cout << "Best model according to DT" << endl;
                cout << "--------------------------" << endl;
                dt_selection.print_best_model(cout);
                cout << endl;
            }
        }
        /* clean */
        if (opts.partitions_desc)
            delete opts.partitions_desc;
        if (opts.partitions_eff)
            delete opts.partitions_eff;
    }
    else
    {
        #ifndef CONSOLE
        /* launch GUI */
        QApplication a(argc, argv);
        modeltest::jModelTest w;
        w.show();
        return_val = a.exec();
        #endif
    }

    return return_val;
}
