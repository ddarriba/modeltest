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

#ifndef MODELTESTSERVICE_H
#define MODELTESTSERVICE_H

#include "../modeltest.h"
#include "../global_defs.h"
#include "../model/model.h"
#include "../model_selection.h"
#include "../thread/observer.h"

#include <string>
#include <vector>

/**
 * @brief Methods for accessing ModelTest background
 */
class ModelTestService
{
  modeltest::ModelTest *modeltest_instance;
  ModelTestService()
  {
      modeltest_instance = 0;
  }
  static ModelTestService *s_instance;

public:
  static ModelTestService *instance()
  {
      if (!s_instance)
          s_instance = new ModelTestService();
      return s_instance;
  }

  ~ModelTestService();

  static bool initialized( void ) { return s_instance != NULL; }
  static void finalize( void );

  bool create_instance( mt_options_t & options );
  bool destroy_instance( void );
  bool reset_instance( mt_options_t & options );

  bool optimize_single(const partition_id_t &part_id,
                       modeltest::Model *model,
                       mt_index_t thread_id,
                       double epsilon_param,
                       double epsilon_opt,
                       const std::vector<Observer *> observers = {});

  bool evaluate_models(partition_id_t const& part_id,
                       mt_size_t n_procs,
                       double epsilon_param,
                       double epsilon_opt,
                       std::ostream &out = std::cout);

  modeltest::ModelSelection * select_models(partition_id_t const& part_id,
                                            modeltest::ic_type type);

  bool print_selection(modeltest::ModelSelection const& selection,
                       std::ostream  &out = std::cout) const;

  void print_command_lines(modeltest::Model const& model,
                           std::string const& msa_filename = "MSA_FILENAME",
                           std::ostream  &out = std::cout) const;

  void print_raxml_partitions(const partitioning_scheme_t & partitions,
                              const std::vector<modeltest::selection_model> & selection,
                              std::ostream  &out = std::cout) const;

  mt_size_t get_number_of_models(partition_id_t const& part_id) const;

  modeltest::PartitioningScheme & get_partitioning_scheme( void ) const;

  modeltest::Model * get_model(partition_id_t const& part_id,
                               mt_index_t model_idx) const;

  modeltest::ModelTest * get_modeltest()
  {
      return modeltest_instance;
  }

  void topological_summary(partition_id_t const& part_id,
                           modeltest::ModelSelection const& bic_selection,
                           modeltest::ModelSelection const& aic_selection,
                           modeltest::ModelSelection const& aicc_selection,
                           std::string const& topologies_filename = "",
                           std::ostream  &out = std::cout);

  std::string get_iqtree_command_line(modeltest::Model const& model,
                      std::string const& msa_filename = "MSA_FILENAME") const;

  std::string get_paup_command_line(modeltest::Model const& model,
                      std::string const& msa_filename = "MSA_FILENAME") const;

  std::string get_raxml8_command_line(modeltest::Model const& model,
                      std::string const& msa_filename = "MSA_FILENAME") const;

  std::string get_raxmlng_command_line(modeltest::Model const& model,
                      std::string const& msa_filename = "MSA_FILENAME") const;

  std::string get_phyml_command_line(modeltest::Model const& model,
                      const std::string &msa_filename = "MSA_FILENAME") const;

//public:
//    ModelTestService(ModelTestService const&) = delete;
//    void operator=(ModelTestService const&)   = delete;
};

#endif // MODELTESTSERVICE_H
