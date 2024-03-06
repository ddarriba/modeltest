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
#include "thread/parallel_context.h"

#include <ctime>
#include <cerrno>

#include <vector>
#include <chrono>

#include "genesis/logging.h"

ModelTestService *ModelTestService::s_instance = 0;

using namespace std;

/** extensions **/
bool have_avx2;
bool have_avx;
bool have_sse3;

/** number of parallel processes */
int mpi_rank;
int mpi_numprocs;

mt_size_t num_cores_p;
mt_size_t num_cores_l;

#if(MPI_ENABLED)
MPI_Comm master_mpi_comm;
#endif

#define FANCY_GUI

int main(int argc, char *argv[])
{
#if(MPI_ENABLED)

    /* so far, allow only for single-process tasks */
    master_mpi_comm = MPI_COMM_WORLD;

    ParallelContext::init_mpi(argc, argv, 0);

    mpi_numprocs = ParallelContext::num_procs();
    mpi_rank = ParallelContext::proc_id();

    cout << "MPI Start: Size: " << mpi_numprocs
         << " Rank: " << mpi_rank << endl;
#else
    mpi_rank = 0;
    mpi_numprocs = 1;
#endif

    BARRIER;

    int return_val = EXIT_SUCCESS;

    num_cores_p = modeltest::Utils::count_physical_cores();
    num_cores_l = modeltest::Utils::count_logical_cores();

    pll_hardware_probe();
    have_avx2 = pll_hardware.avx2_present;
    have_avx = pll_hardware.avx_present;
    have_sse3 = pll_hardware.sse3_present;

    modeltest::disable_repeats = false;

    genesis::utils::Logging::log_to_stream(cout);
    genesis::utils::Logging::err_to_stream(cerr);
    genesis::utils::Logging::max_level (genesis::utils::Logging::kProgress);

    if (argc > 1)
    {
        /* command line */
        mt_options_t opts;
        time_t ini_global_time = time(NULL);

        Meta::print_ascii_logo(MT_INFO);
        Meta::print_header(MT_INFO);

        /* initialization */

        if (!Meta::parse_arguments(argc, argv, opts))
        {
          if (modeltest::mt_errno == MT_ERROR_IGNORE)
          {
            ModelTestService::finalize( false);
            return EXIT_SUCCESS;
          }
          else
          {
            modeltest::Utils::exit_with_error(
              "Invalid arguments\nTry `%s --help` for more information",
              PACKAGE);
          }
        }

        opts.n_mpiprocs = mpi_numprocs;

        if (opts.output_log_file.compare(""))
        {
          genesis::utils::Logging::log_to_file(opts.output_log_file);
        }

        Meta::print_system_info(MT_INFO);
        MT_INFO << endl << flush;

        if (!ModelTestService::instance()->create_instance(opts))
        {
            LOG_ERR << PACKAGE << ": " << modeltest::mt_errmsg << endl;
            return (int)modeltest::mt_errno;
        }

        Meta::print_options(opts, MT_INFO);
        MT_INFO << PACKAGE << " was called as follows: " << endl << ">> ";
        for (int i=0; i<argc; i++)
            MT_INFO << argv[i] << " ";
        MT_INFO << endl << endl;

        /* start processing */

        MT_INFO << flush;
        LOG_INFO << flush;

        if (mpi_numprocs > 1 && opts.n_threadprocs > 1)
        {
           LOG_WARN << PACKAGE << ": MPI + multiple high-level processes are not supported: shared processes set to 1" << endl;
           opts.n_threadprocs = 1;
        }

        if (mpi_numprocs > 1 && opts.n_threads > 1)
        {
           LOG_WARN << PACKAGE << ": Hybrid MPI + Multithreading is under evaluation. Handle results with care" << endl;
        }

        if (mpi_numprocs == 1 && opts.n_threadprocs == 1 && num_cores_p > 1)
        {
            /* We warn only if the number of processors is 1. */
            /* Otherwise we assume that the user is aware of this feature */
            LOG_WARN << PACKAGE << ": You are using one single thread out of "
                    << num_cores_p << " physical cores." << endl;
            LOG_WARN << PACKAGE
                 << ": You can set the number of threads with -p argument."
                 << endl;
            LOG_WARN << PACKAGE << ": Try '" << PACKAGE << " --help' or '"
                 << PACKAGE << " --usage' for more information" << endl;
        }

        // vector<map<modeltest::ic_type, modeltest::selection_model>> best_models(opts.partitions_eff->size());
        map<modeltest::ic_type, vector<modeltest::selection_model>> best_models;
        best_models[modeltest::ic_bic].resize(opts.partitions_eff->size());
        best_models[modeltest::ic_aic].resize(opts.partitions_eff->size());
        best_models[modeltest::ic_aicc].resize(opts.partitions_eff->size());
        best_models[modeltest::ic_dt].resize(opts.partitions_eff->size());

        ofstream * results_stream = 0;

        if (ROOT)
          results_stream = modeltest::Utils::open_file_for_writing(opts.output_results_file);

        if (results_stream)
           Meta::print_options(opts, *results_stream);

        for(mt_index_t i=0; i<opts.partitions_eff->size(); i++)
        {
          bool exec_ok;
          MT_INFO << endl << "Partition "
               << i+1 << "/" << opts.partitions_eff->size()
               << endl << endl;

          partition_id_t part_id = {i};

          exec_ok = ModelTestService::instance()->evaluate_models(part_id,
                                                        opts.n_threadprocs,
                                                        opts.epsilon_param,
                                                        opts.epsilon_opt,
                                                        MT_INFO);

          if (ROOT)
          {
            if (exec_ok)
            {
              MT_INFO << endl << "Computation of likelihood scores completed. It took " <<
                           modeltest::Utils::format_time(time(NULL) - ini_global_time) << endl;

              modeltest::ModelSelection * bic_selection = ModelTestService::instance()->select_models(part_id, modeltest::ic_bic);
              best_models[modeltest::ic_bic][i] = bic_selection->get_model(0);
              ModelTestService::instance()->print_selection(*bic_selection, MT_INFO);
              ModelTestService::instance()->print_command_lines(*best_models[modeltest::ic_bic][i].model,
                                                                opts.msa_filename,
                                                                MT_INFO);
              if (results_stream)
              {
                  ModelTestService::instance()->print_selection(*bic_selection, *results_stream);
                  ModelTestService::instance()->print_command_lines(*best_models[modeltest::ic_bic][i].model,
                                                                    opts.msa_filename,
                                                                    *results_stream);
              }

              modeltest::ModelSelection * aic_selection = ModelTestService::instance()->select_models(part_id, modeltest::ic_aic);
              best_models[modeltest::ic_aic][i] = aic_selection->get_model(0);
              ModelTestService::instance()->print_selection(*aic_selection, MT_INFO);
              ModelTestService::instance()->print_command_lines(*best_models[modeltest::ic_aic][i].model,
                                                                opts.msa_filename,
                                                                MT_INFO);
              if (results_stream)
              {
                  ModelTestService::instance()->print_selection(*aic_selection, *results_stream);
                  ModelTestService::instance()->print_command_lines(*best_models[modeltest::ic_aic][i].model,
                                                                    opts.msa_filename,
                                                                    *results_stream);
              }

              modeltest::ModelSelection * aicc_selection = ModelTestService::instance()->select_models(part_id, modeltest::ic_aicc);
              best_models[modeltest::ic_aicc][i] = aicc_selection->get_model(0);
              ModelTestService::instance()->print_selection(*aicc_selection, MT_INFO);
              ModelTestService::instance()->print_command_lines(*best_models[modeltest::ic_aicc][i].model,
                                                                opts.msa_filename,
                                                                MT_INFO);
              if (results_stream)
              {
                ModelTestService::instance()->print_selection(*aicc_selection, *results_stream);
                ModelTestService::instance()->print_command_lines(*best_models[modeltest::ic_aicc][i].model,
                                                                  opts.msa_filename,
                                                                  *results_stream);
              }

#if(ENABLE_DT_OUTPUT)
              /* ignore DT if topology is not fixed */
              if (opts.starting_tree != tree_ml)
              {
                  modeltest::ModelSelection * dt_selection = ModelTestService::instance()->select_models(part_id, modeltest::ic_dt);
                  best_models[modeltest::ic_dt][i] = dt_selection->get_model(0);
                  ModelTestService::instance()->print_selection(*dt_selection, MT_INFO);
                  ModelTestService::instance()->print_command_lines(*best_models[modeltest::ic_dt][i].model,
                                                                    opts.msa_filename,
                                                                    MT_INFO);
                  if (results_stream)
                  {
                      ModelTestService::instance()->print_selection(*dt_selection, *results_stream);
                      ModelTestService::instance()->print_command_lines(*best_models[modeltest::ic_dt][i].model,
                                                                        opts.msa_filename,
                                                                        *results_stream);
                  }
                  delete dt_selection;
              }
#endif

              /* ignore topological summary if topology is fixed */
              if (opts.starting_tree == tree_ml)
              {
                /* topological summary */
                ModelTestService::instance()->topological_summary(part_id,
                                                                  *bic_selection,
                                                                  *aic_selection,
                                                                  *aicc_selection,
                                                                  opts.output_topos_file,
                                                                  MT_INFO);
              }

              delete bic_selection;
              delete aic_selection;
              delete aicc_selection;
            }
            else
            {
              LOG_ERR << modeltest::mt_errmsg << endl;
              return (int)modeltest::mt_errno;
            }
          }
        }

        if (ROOT)
        {
          if (results_stream)
          {
              *results_stream << "Done" << endl;

              results_stream->close();
              delete results_stream;
          }

          MT_INFO << "Summary:" << endl;
          for(mt_index_t i=0; i<opts.partitions_eff->size(); i++)
          {
              MT_INFO <<  endl << "Partition " << i+1 << "/" << opts.partitions_eff->size() << ":" << endl;
              modeltest::ModelSelection::print_inline_header(MT_INFO);
              modeltest::ModelSelection::print_inline_best_model(modeltest::ic_bic, best_models[modeltest::ic_bic][i], MT_INFO);
              modeltest::ModelSelection::print_inline_best_model(modeltest::ic_aic, best_models[modeltest::ic_aic][i], MT_INFO);
              modeltest::ModelSelection::print_inline_best_model(modeltest::ic_aicc, best_models[modeltest::ic_aicc][i], MT_INFO);
#if(ENABLE_DT_OUTPUT)
              if (opts.starting_tree != tree_ml)
                modeltest::ModelSelection::print_inline_best_model(modeltest::ic_dt, best_models[modeltest::ic_dt][i], MT_INFO);
#endif
          }

          if (opts.partitions_eff->size() > 1)
          {
            ofstream * parts_stream = 0;
            parts_stream = modeltest::Utils::open_file_for_writing(
              opts.output_raxml_part_file + ".bic");
            ModelTestService::instance()->print_raxml_partitions(
              *opts.partitions_desc,
              best_models[modeltest::ic_bic],
              *parts_stream);
            parts_stream->close();

            parts_stream = modeltest::Utils::open_file_for_writing(
              opts.output_raxml_part_file + ".aic");
            ModelTestService::instance()->print_raxml_partitions(
              *opts.partitions_desc,
              best_models[modeltest::ic_aic],
              *parts_stream);
            parts_stream->close();

            parts_stream = modeltest::Utils::open_file_for_writing(
              opts.output_raxml_part_file + ".aicc");
            ModelTestService::instance()->print_raxml_partitions(
              *opts.partitions_desc,
              best_models[modeltest::ic_aicc],
              *parts_stream);
            parts_stream->close();

#if(ENABLE_DT_OUTPUT)
            parts_stream = modeltest::Utils::open_file_for_writing(
              opts.output_raxml_part_file + ".dt");
            ModelTestService::instance()->print_raxml_partitions(
              *opts.partitions_desc,
              best_models[modeltest::ic_dt],
              *parts_stream);
            parts_stream->close();
#endif
          }

          MT_INFO << endl;
          if (opts.partitions_eff->size() > 1)
            MT_INFO << "Result partition files written to "
                    << opts.output_raxml_part_file << ".*" << endl;
          MT_INFO << "Execution results written to "
                  << opts.output_results_file << endl;
          if (opts.output_tree_to_file)
              MT_INFO << "Starting tree written to "
                      << opts.output_tree_file << endl;
          //MT_INFO << "Log written to " << opts.output_log_file << endl;

          /* clean */
          if (opts.partitions_desc)
              delete opts.partitions_desc;
        }
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
BARRIER;

    ModelTestService::finalize( false );

    return return_val;
}
