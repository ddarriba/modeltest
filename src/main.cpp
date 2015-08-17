#include "modeltest_gui.h"

#include <iostream>
#include <iomanip>
#include <ctime>
#include <getopt.h>
#include <QApplication>

using namespace std;

mt_options parse_arguments(int argc, char *argv[])
{
    mt_options exec_opt;
    bool input_file_ok = false;
    dna_subst_schemes dna_ss = ss_undef;
    mt_size_t n_sequences, n_sites;

    /* defaults */
    dna_subst_schemes default_ss = ss_11;

    /* set default options */
    exec_opt.n_catg        = 4;
    exec_opt.epsilon_param = 0.0001;
    exec_opt.epsilon_opt   = 0.001;
    exec_opt.rnd_seed      = 12345;
    exec_opt.model_params  = 0;
    exec_opt.datatype      = dt_dna;

    static struct option long_options[] =
    {
        { "categories", required_argument, 0, 'c' },
        { "datatype", required_argument, 0, 'd' },
        { "epsilon", required_argument, 0, 'e' },
        { "model-freqs", required_argument, 0, 'F' },
        { "help", no_argument, 0, 'h' },
        { "input", required_argument, 0, 'i' },
        { "model-het", required_argument, 0, 'H' },
        { "tree", required_argument, 0, 't' },
        { "partitions", required_argument, 0, 'q' },
        { "rngseed", required_argument, 0, 'r' },
        { "schemes", required_argument, 0, 'S' },
        { "output", required_argument, 0, 'o' },
        { 0, 0, 0, 0 }
    };

    int opt = 0, long_index = 0;
    while ((opt = getopt_long(argc, argv, "c:d:e:hH:i:t:q:r:S:o:", long_options,
                              &long_index)) != -1) {
        switch (opt) {
        case 'c':
            exec_opt.n_catg = atoi(optarg);
            break;
        case 'd':
            if (!strcasecmp(optarg, "nt"))
            {
                exec_opt.datatype = dt_dna;
            }
            else if (!strcasecmp(optarg, "aa"))
            {
                exec_opt.datatype = dt_protein;
            }
            else
            {
                cerr << "ERROR: Invalid datatype " << optarg << endl;
                exit(EXIT_FAILURE);
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
                    exit(EXIT_FAILURE);
                }
            }
            break;
        case 'h':
            cerr << "ModelTest Help:" << endl;
            exit(EXIT_FAILURE);
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
                    cerr << "ERROR: Unrecognised rate heterogeneity parameter " << optarg[i] << endl;
                    exit(EXIT_FAILURE);
                }
            }
            break;
        case 'i':
            exec_opt.msa_filename = optarg;
            input_file_ok = strcmp(optarg, "");
            break;
        case 't':
            exec_opt.tree_filename = optarg;
            exec_opt.starting_tree = tree_user_fixed;
            break;
        case 'q':
            exec_opt.partitions_filename = optarg;
            //partitionsFileDefined = true;
            break;
        case 'o':
            exec_opt.output_filename = optarg;
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
                cerr << "ERROR: Invalid number of substitution schemes " << optarg << endl;
                exit(EXIT_FAILURE);
            }
            break;
        default:
            cerr << "Unrecognised argument -" << opt << endl;
            exit(EXIT_FAILURE);
        }
    }

    srand(exec_opt.rnd_seed);

    /* validate input file */
    if (input_file_ok) {
        if (modeltest::MsaPll::test(exec_opt.msa_filename, &n_sequences, &n_sites))
        {
            cout << "Read OK: " << n_sequences << " x " << n_sites << endl;
        }
        else
        {
            cerr << "Error parsing alignment: " << exec_opt.msa_filename << endl;
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        cerr << "Alignment file (-i) is required" << endl;
        exit(EXIT_FAILURE);
    }

    if (exec_opt.datatype == dt_protein)
    {
        if (dna_ss != ss_undef)
            cerr << "Warning: Substitution schemes will be ignored" << endl;

        /* TODO: temporary the whole set is used */
        for (int i=0; i<N_PROT_MODEL_MATRICES; i++)
            exec_opt.candidate_models.push_back(i);
    }
    else
    {
        if (dna_ss == ss_undef)
            exec_opt.subst_schemes = default_ss;
        else
            exec_opt.subst_schemes = dna_ss;

        /* fill candidate matrices */
        switch(exec_opt.subst_schemes)
        {
        case ss_11:
            exec_opt.candidate_models.push_back(dna_model_matrices_indices[4]);
            exec_opt.candidate_models.push_back(dna_model_matrices_indices[5]);
            exec_opt.candidate_models.push_back(dna_model_matrices_indices[7]);
            exec_opt.candidate_models.push_back(dna_model_matrices_indices[8]);
        case ss_7:
            exec_opt.candidate_models.push_back(dna_model_matrices_indices[6]);
            exec_opt.candidate_models.push_back(dna_model_matrices_indices[9]);
        case ss_5:
            exec_opt.candidate_models.push_back(dna_model_matrices_indices[2]);
            exec_opt.candidate_models.push_back(dna_model_matrices_indices[3]);
        case ss_3:
            exec_opt.candidate_models.push_back(dna_model_matrices_indices[0]);
            exec_opt.candidate_models.push_back(dna_model_matrices_indices[1]);
            exec_opt.candidate_models.push_back(dna_model_matrices_indices[10]);
            break;
        case ss_203:
            for (int i=0; i<203; i++)
                exec_opt.candidate_models.push_back(i);
            break;
        case ss_undef:
        default:
            assert(0);
        }
    }

    /* if there are no model specifications, include all */
    if (!exec_opt.model_params)
        exec_opt.model_params =
                MOD_PARAM_NO_RATE_VAR |
                MOD_PARAM_INV |
                MOD_PARAM_GAMMA |
                MOD_PARAM_INV_GAMMA;

    if (!(exec_opt.model_params & (MOD_PARAM_FIXED_FREQ | MOD_PARAM_ESTIMATED_FREQ)))
        exec_opt.model_params |= MOD_PARAM_FIXED_FREQ;

    return exec_opt;
}

int main(int argc, char *argv[])
{
    int return_val = 0;
    time_t ini_global_time = time(NULL);

    if (argc > 1)
    {
        /* command line */
        modeltest::ModelTest mt;
        mt_options opts;
        mt_index_t cur_model;

        opts = parse_arguments(argc, argv);

        mt.build_instance(opts, tree_user_fixed);

        //mt.evaluate_models();
        cur_model = 0;
        for (cur_model=0; cur_model<mt.get_models().size(); cur_model++)
        {
//            if (cur_model && cur_model < 59)
//                continue;
            modeltest::Model *model = mt.get_models()[cur_model];
            time_t ini_t = time(NULL);
            if (!mt.evaluate_single_model(model, 0, opts.epsilon_param, opts.epsilon_opt))
            {
                cerr << "ERROR OPTIMIZING MODEL" << endl;
                exit(MT_ERROR_OPTIMIZE);
            }

            /* print progress */
            cout << setw(5) << cur_model << "/" << mt.get_models().size()
                 << setw(20) << model->get_name()
                 << setw(18) << setprecision(4) << fixed << model->get_lnl()
                 << setw(8) << time(NULL) - ini_t
                 << setw(8) << time(NULL) - ini_global_time << endl;
        }

        modeltest::ModelSelection bic_selection(mt.get_models(), modeltest::ic_bic);
        bic_selection.print(cout);
    }
    else
    {
        /* launch GUI */
        QApplication a(argc, argv);
        modeltest::jModelTest w;
        w.show();
        return_val = a.exec();
    }

    return return_val;
}
