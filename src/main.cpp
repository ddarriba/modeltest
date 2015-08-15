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

    /* set default options */
    exec_opt.n_catg        = 4;
    exec_opt.epsilon_param = 0.0001;
    exec_opt.epsilon_opt   = 0.001;
    exec_opt.rnd_seed      = 12345;
    exec_opt.model_params  = 0;

    static struct option long_options[] =
    {
        { "categories", required_argument, 0, 'c' },
        { "datatype", required_argument, 0, 'd' },
        { "epsilon", required_argument, 0, 'e' },
        { "input", required_argument, 0, 'i' },
        { "tree", required_argument, 0, 't' },
        { "partitions", required_argument, 0, 'q' },
        { "seed", required_argument, 0, 's' },
        { "schemes", required_argument, 0, 'S' },
        { "output", required_argument, 0, 'o' },
        { 0, 0, 0, 0 }
    };

    int opt = 0, long_index = 0;
    while ((opt = getopt_long(argc, argv, "c:d:e:hi:t:q:s:o:", long_options,
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
                exit(1);
            }
        case 'e':
            exec_opt.epsilon_opt = atof(optarg);
            exec_opt.epsilon_param = atof(optarg);
            break;
        case 'h':
            cerr << "ModelTest Help:" << endl;
            exit(1);
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
        case 's':
            exec_opt.rnd_seed = (unsigned int) atoi(optarg);
            break;
        case 'S':
            if (!strcmp(optarg, "3"))
            {
                exec_opt.subst_schemes = ss_3;
            }
            else if (!strcmp(optarg, "5"))
            {
                exec_opt.subst_schemes = ss_5;
            }
            else if (!strcmp(optarg, "7"))
            {
                exec_opt.subst_schemes = ss_7;
            }
            else if (!strcmp(optarg, "11"))
            {
                exec_opt.subst_schemes = ss_11;
            }
            else if (!strcmp(optarg, "203"))
            {
                exec_opt.subst_schemes = ss_203;
            }
            else
            {
                cerr << "ERROR: Invalid number of substitution schemes " << optarg << endl;
                exit(1);
            }
            break;
        default:
            cerr << "Unrecognised argument -" << opt << endl;
            exit(1);
        }
    }

    srand(exec_opt.rnd_seed);
    if (!input_file_ok) {
        cerr << "Alignment file (-i) is required" << endl;
        exit(1);
    }

    if (exec_opt.datatype == dt_protein && dna_ss != ss_undef)
    {
        cerr << "Warning: Substitution schemes will be ignored" << endl;
    }

    /* fill candidate matrices */
    for (int i=0; i<=10; i++)
    {
        exec_opt.candidate_models.push_back(dna_model_matrices_indices[i]);
    }

    /* if there are no model specifications, include all */
    if (!exec_opt.model_params)
        exec_opt.model_params = 63;

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
            modeltest::Model *model = mt.get_models()[cur_model];
            time_t ini_t = time(NULL);
            if (!mt.evaluate_single_model(model, 0, opts.epsilon_param, opts.epsilon_opt))
            {
                cerr << "ERROR OPTIMIZING MODEL" << endl;
                exit(MT_ERROR_OPTIMIZE);
            }
            cout << setw(2) << cur_model << "/" << mt.get_models().size() << setw(12) << model->get_name() << setw(18) << setprecision(4) << fixed << model->get_lnl() << setw(8) << time(NULL) - ini_t << setw(8) << time(NULL) - ini_global_time << endl;
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
