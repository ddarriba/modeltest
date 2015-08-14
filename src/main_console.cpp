#include "modeltest.h"
#include "global_defs.h"

#include <iostream>
#include <iomanip>
#include <time.h>

#ifdef CONSOLE

using namespace std;
int main(int argc, char *argv[])
{
    int return_val = 0;

    time_t ini_global_time = time(NULL);

    if (argc > 1)
    {
        /* command line */
        modeltest::ModelTest mt;
        std::vector<int> matrices;
//        for (int i=0; i<11; i++)
        for (int i=10; i>=0; i--)
        {
            cout << "Add Matrix " << model_matrices_ind[i] << endl;
            matrices.push_back(model_matrices_ind[i]);
        }

        mt_size_t n_sites, n_seqs;
        if (mt.test_msa(std::string(argv[1]), &n_seqs, &n_sites))
        	cout << " Test OK! " << n_seqs << " x " << n_sites << endl;
        else
        {
            cerr << " ERROR READING MSA: " << argv[1] << endl;
            exit(MT_ERROR_ALIGNMENT);
        }

        if (!mt.build_instance(MOD_PARAM_EQUAL_FREQ | MOD_PARAM_NO_RATE_VAR, 4, matrices, std::string(argv[1]), std::string(argv[2]), tree_user_fixed))
        {
            cerr << "ERROR BUILDING INSTANCE" << endl;
            exit(MT_ERROR_INSTANCE);
        }

        cout << "Evaluating " << mt.get_models().size() << " models..." << endl;

        //mt.evaluate_models();
        mt_index_t cur_model = 0;
        for (modeltest::Model *model : mt.get_models())
        {
            time_t ini_t = time(NULL);
            if (!mt.evaluate_single_model(model, 0, 0.0001, 0.001))
            {
                cerr << "ERROR OPTIMIZING MODEL" << endl;
                exit(MT_ERROR_OPTIMIZE);
            }
            cout << setw(2) << ++cur_model << "/" << mt.get_models().size() << setw(12) << model->get_name() << setw(18) << setprecision(4) << fixed << model->get_lnl() << setw(8) << time(NULL) - ini_t << setw(8) << time(NULL) - ini_global_time << endl;
        }

        modeltest::ModelSelection bic_selection(mt.get_models(), modeltest::ic_bic);
        bic_selection.print(cout);
    }

    cout << "Elapsed time: " << time(NULL) - ini_global_time << endl;
    return return_val;
}
#endif
