#ifndef _NO_GUI_
#include "gui/xmodeltest.h"
#include "service/modeltestservice.h"
#include <QApplication>
#else
#include "service/modeltestservice.h"
#endif
#include "utils.h"
#include "meta.h"

#include <ctime>
#include <cerrno>

#include <vector>
#include <chrono>

#include "thread/threadpool.h"

ModelTestService *ModelTestService::s_instance = 0;

using namespace std;

/** number of parallel processes */
static mt_size_t n_procs = 1;

int main(int argc, char *argv[])
{
    int return_val = EXIT_SUCCESS;

    Meta::print_ascii_logo(cout);
    Meta::print_header(cout);

    if (argc > 1)
    {
        /* command line */
        mt_options_t opts;
        mt_size_t num_cores = modeltest::Utils::count_physical_cores();
        time_t ini_global_time = time(NULL);

        if (!Meta::parse_arguments(argc, argv, opts, &n_procs))
        {
            return(EXIT_FAILURE);
        }

        if (!ModelTestService::instance()->create_instance(opts))
        {
            cerr << modeltest::mt_errmsg << endl;
            return (int)modeltest::mt_errno;
        }

        Meta::print_system_info(cout);
        cout << endl;
        Meta::print_options(opts, cout);
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

        ofstream * results_stream = modeltest::Utils::open_file_for_writing(opts.output_results_file);
        if (results_stream)
            Meta::print_options(opts, *results_stream);

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
            ModelTestService::instance()->print_selection(bic_selection, cout);
            if (results_stream)
                ModelTestService::instance()->print_selection(bic_selection, *results_stream);
            best_models[i][modeltest::ic_bic] = bic_selection->get_model(0);
            delete bic_selection;

            modeltest::ModelSelection * aic_selection = ModelTestService::instance()->select_models(part_id, modeltest::ic_aic);
            ModelTestService::instance()->print_selection(aic_selection, cout);
            if (results_stream)
                ModelTestService::instance()->print_selection(aic_selection, *results_stream);
            best_models[i][modeltest::ic_aic] = aic_selection->get_model(0);
            delete aic_selection;

            modeltest::ModelSelection * aicc_selection = ModelTestService::instance()->select_models(part_id, modeltest::ic_aicc);
            ModelTestService::instance()->print_selection(aicc_selection, cout);
            if (results_stream)
                ModelTestService::instance()->print_selection(aicc_selection, *results_stream);
            best_models[i][modeltest::ic_aicc] = aicc_selection->get_model(0);
            delete aicc_selection;

            /* ignore DT if topology is not fixed */
            if (opts.starting_tree != tree_ml)
            {
                modeltest::ModelSelection * dt_selection = ModelTestService::instance()->select_models(part_id, modeltest::ic_dt);
                ModelTestService::instance()->print_selection(dt_selection, cout);
                if (results_stream)
                    ModelTestService::instance()->print_selection(dt_selection, *results_stream);
                best_models[i][modeltest::ic_dt] = dt_selection->get_model(0);
                delete dt_selection;
            }
        }

        if (results_stream)
        {
            results_stream->close();
            delete results_stream;
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
        }

        cout << endl;
        cout << "Execution results written to " << opts.output_results_file << endl;
        if (opts.output_tree_to_file)
            cout << "Starting tree written to " << opts.output_tree_file << endl;
        //cout << "Log written to " << opts.output_log_file << endl;

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
