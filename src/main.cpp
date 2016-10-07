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

#ifndef _NO_GUI_
#include "gui/xmodeltest.h"
#include "gui2/xmodeltestfancy.h"
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

#include "genesis/logging.h"
#include "thread/threadpool.h"

ModelTestService *ModelTestService::s_instance = 0;

using namespace std;

/** number of parallel processes */
static mt_size_t n_procs = 1;

#define FANCY_GUI

int main(int argc, char *argv[])
{
    int return_val = EXIT_SUCCESS;

    genesis::utils::Logging::log_to_stream(cout);
    genesis::utils::Logging::err_to_stream(cerr);
    genesis::utils::Logging::max_level (genesis::utils::Logging::kProgress);
    Meta::print_ascii_logo(MT_INFO);
    Meta::print_header(MT_INFO);

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

        if (opts.output_log_file.compare(""))
        {
          genesis::utils::Logging::log_to_file(opts.output_log_file);
        }

        Meta::print_system_info(MT_INFO);
        MT_INFO << endl;

        Meta::print_options(opts, MT_INFO);
        MT_INFO << PACKAGE << " was called as follows: " << endl << ">> ";
        for (int i=0; i<argc; i++)
            MT_INFO << argv[i] << " ";
        MT_INFO << endl << endl;

        if (!ModelTestService::instance()->create_instance(opts))
        {
            LOG_ERR << modeltest::mt_errmsg << endl;
            return (int)modeltest::mt_errno;
        }

        if (n_procs == 1 && num_cores > 1)
        {
            /* We warn only if the number of processors is 1. */
            /* Otherwise we assume that the user is aware of this feature */
            LOG_WARN << endl << PACKAGE << ": Warning: You are using one single thread out of "
                    << num_cores << " physical cores." << endl;
            LOG_WARN << PACKAGE
                 << ":          You can set the number of threads with -p argument."
                 << endl;
            LOG_WARN << PACKAGE << ": Try '" << PACKAGE << " --help' or '"
                 << PACKAGE << " --usage' for more information" << endl;
        }

        vector<map<modeltest::ic_type, modeltest::selection_model>> best_models(opts.partitions_eff->size());

        ofstream * results_stream = modeltest::Utils::open_file_for_writing(opts.output_results_file);
        if (results_stream)
            Meta::print_options(opts, *results_stream);

        for(mt_index_t i=0; i<opts.partitions_eff->size(); i++)
        {

            MT_INFO << endl << "Partition "
                 << i+1 << "/" << opts.partitions_eff->size()
                 << endl << endl;

            partition_id_t part_id = {i};
            ModelTestService::instance()->evaluate_models(part_id,
                                                          n_procs,
                                                          opts.epsilon_param,
                                                          opts.epsilon_opt,
                                                          MT_INFO);

            MT_INFO << endl << "Computation of likelihood scores completed. It took " <<
                         modeltest::Utils::format_time(time(NULL) - ini_global_time) << endl;

            modeltest::ModelSelection * bic_selection = ModelTestService::instance()->select_models(part_id, modeltest::ic_bic);
            ModelTestService::instance()->print_selection(*bic_selection, MT_INFO);
            if (results_stream)
                ModelTestService::instance()->print_selection(*bic_selection, *results_stream);
            best_models[i][modeltest::ic_bic] = bic_selection->get_model(0);

            modeltest::ModelSelection * aic_selection = ModelTestService::instance()->select_models(part_id, modeltest::ic_aic);
            ModelTestService::instance()->print_selection(*aic_selection, MT_INFO);
            if (results_stream)
                ModelTestService::instance()->print_selection(*aic_selection, *results_stream);
            best_models[i][modeltest::ic_aic] = aic_selection->get_model(0);

            modeltest::ModelSelection * aicc_selection = ModelTestService::instance()->select_models(part_id, modeltest::ic_aicc);
            ModelTestService::instance()->print_selection(*aicc_selection, MT_INFO);
            if (results_stream)
                ModelTestService::instance()->print_selection(*aicc_selection, *results_stream);
            best_models[i][modeltest::ic_aicc] = aicc_selection->get_model(0);

            /* ignore DT if topology is not fixed */
            if (opts.starting_tree != tree_ml)
            {
                modeltest::ModelSelection * dt_selection = ModelTestService::instance()->select_models(part_id, modeltest::ic_dt);
                ModelTestService::instance()->print_selection(*dt_selection, MT_INFO);
                if (results_stream)
                    ModelTestService::instance()->print_selection(*dt_selection, *results_stream);
                best_models[i][modeltest::ic_dt] = dt_selection->get_model(0);
                delete dt_selection;
            }

            /* topological summary */
            ModelTestService::instance()->topological_summary(part_id,
                                                              *bic_selection,
                                                              *aic_selection,
                                                              *aicc_selection);
            delete bic_selection;
            delete aic_selection;
            delete aicc_selection;
        }

        if (results_stream)
        {
            results_stream->close();
            delete results_stream;
        }

        MT_INFO << "Summary:" << endl;
        for(mt_index_t i=0; i<opts.partitions_eff->size(); i++)
        {
            MT_INFO <<  endl << "Partition " << i+1 << "/" << opts.partitions_eff->size() << ":" << endl;
            modeltest::ModelSelection::print_inline_header(MT_INFO);
            modeltest::ModelSelection::print_inline_best_model(modeltest::ic_bic, best_models[i][modeltest::ic_bic], MT_INFO);
            modeltest::ModelSelection::print_inline_best_model(modeltest::ic_aic, best_models[i][modeltest::ic_aic], MT_INFO);
            modeltest::ModelSelection::print_inline_best_model(modeltest::ic_aicc, best_models[i][modeltest::ic_aicc], MT_INFO);
            if (opts.starting_tree != tree_ml)
                modeltest::ModelSelection::print_inline_best_model(modeltest::ic_dt, best_models[i][modeltest::ic_dt], MT_INFO);
        }

        MT_INFO << endl;
        MT_INFO << "Execution results written to " << opts.output_results_file << endl;
        if (opts.output_tree_to_file)
            MT_INFO << "Starting tree written to " << opts.output_tree_file << endl;
        //MT_INFO << "Log written to " << opts.output_log_file << endl;

        /* clean */
        if (opts.partitions_desc)
            delete opts.partitions_desc;
        if (opts.partitions_eff)
            delete opts.partitions_eff;
    }
    else
    {
        #ifndef _NO_GUI_
        /* launch GUI */
        QApplication a(argc, argv);
        //Q_INIT_RESOURCE(mtgraphics);

#ifdef FANCY_GUI
        XModelTestFancy xmt;
#else
        xmodeltest xmt;
#endif
        xmt.show();
//        modeltest::jModelTest w;
//        w.show();

        return_val = a.exec();
        #else
        LOG_ERR << PACKAGE << " was compiled without GUI support" << endl;
        LOG_ERR << "Try '" << PACKAGE << " --help' or '"
                << PACKAGE << " --usage' for more information" << endl;
        #endif
    }

    ModelTestService::finalize();

    return return_val;
}
