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

#ifndef MODELTEST_H
#define MODELTEST_H

#include "msa.h"
#include "tree.h"
#include "tree.h"
#include "plldefs.h"
#include "global_defs.h"
#include "model/dnamodel.h"
#include "model/protmodel.h"
#include "model_selection.h"
#include "partitioning_scheme.h"
#include "thread/observer.h"
#include "optimize/model_optimizer_pll.h"

namespace modeltest {

typedef struct
{
  modeltest::Msa * msa;                       //! input MSA
  modeltest::Tree * tree;                     //! user defined tree (optional)
  std::vector<partition_descriptor_t> const* partitions_eff;  //! partitioning

  bool rate_clustering;           //! DNA rate clustering
  tree_type_t start_tree;         //! starting tree type
  mt_size_t sample_size;          //! sample size for model selection

  mt_size_t n_tips;               //! number of tips
  mt_size_t n_catg;               //! number of Gamma rate categories
  int gamma_rates;

  bool keep_model_parameters;
  bool ckp_enabled;
  std::string ckp_filename;
} selection_instance;

class ModelTest : public Observer
{
public:
    ModelTest(mt_size_t number_of_threads = 1);
    ~ModelTest();

    /**
     * @brief Tests whether a MSA file is valid
     * @param[in] msa_filename MSA filename
     * @param[out] n_tips number of tips
     * @param[out] n_sites number of sites
     * @return true, if the file is a valid MSA
     */
    static bool test_msa(std::string const& msa_filename,
                  mt_size_t *n_tips,
                  mt_size_t *n_sites,
                  mt_size_t *n_patterns = 0,
                  msa_format_t * msa_format = 0,
                  data_type_t *datatype = 0);

    /**
     * @brief Tests whether a tree file is valid
     * @param[in] tree_filename tree filename
     * @param[out] n_tips number of tips
     * @return true, if the file is a valid tree in NEWICK format
     */
    static bool test_tree(std::string const& tree_filename,
                  mt_size_t *n_tips);

    /**
     * @brief Tests whether a tree is compatible with a MSA
     * @param[in] msa the MSA
     * @param[in] tree the tree
     * @return true, if the tree and the MSA are compatible
     */
    static bool test_link(const Msa *msa,
                          const Tree *tree);

    /**
     * @brief Tests whether a partitioning scheme is valid
     * @param[in] scheme the partitioning scheme
     * @param[in] n_sites the number of sites
     * @return true, if the partitioning scheme is valid for the msa
     */
    static bool test_partitions(const partitioning_scheme_t &scheme,
                                mt_size_t n_sites);

    /**
     * @brief Creates a model    optimization instance
     * @param options            general optimization options
     * @return true, if everything was OK, false in case of error
     */
    bool build_instance(mt_options_t & options);

    /**
     * @brief Optimizes the parameters for one single model
     * @param model          model to optimize
     * @param part_id        partition to optimize
     * @param thread_number  number of the current thread
     * @param tolerance      tolerance for parameter optimization
     * @param epsilon        tolerance for global optimization
     * @return true, if the optimization is OK
     */
    bool evaluate_single_model(Model * model,
                               const partition_id_t &part_id,
                               mt_index_t thread_number = 0,
                               double tolerance = DEFAULT_PARAM_EPSILON,
                               double epsilon = DEFAULT_OPT_EPSILON);

    /**
     * @brief Optimizes the whole candidate models set
     * @param[in] part_id      partition to optimize
     * @param[in] n_threads    number of threads
     * @return true, if the optimization is OK
     */
    bool evaluate_models(const partition_id_t &part_id,
                         mt_size_t n_threads,
                         double epsilon_param,
                         double epsilon_opt,
                         std::ostream &out = std::cout);

    ModelOptimizer * get_model_optimizer(Model * model,
                                         const partition_id_t &part_id,
                                         mt_index_t thread_number = 0,
                                         bool force_opt_topo = false,
                                         bool keep_model_parameters = false);

    PartitioningScheme & get_partitioning_scheme( void ) const;

    Partition & get_partition(partition_id_t const& part_id) const;

    /**
     * @brief Gets the set of candidate models
     * @param[in] part_id  the partition id
     * @return the set of candidate models
     */
    std::vector<Model *> const& get_models(const partition_id_t &part_id);

    virtual void update(Observable * subject, void * data);
private:
    mt_size_t number_of_threads;              //! number of threads
    selection_instance * current_instance;    //! model optimization parameters
    //partitions_map_t partitions;            //! partitions
    PartitioningScheme * partitioning_scheme; //! partitioning scheme

    /**
     * @brief Creates a new set of model optimization parameters
     */
    void create_instance();

    /**
     * @brief Deallocates all the memory
     */
    void free_stuff();

    int eval_and_print(const partition_id_t &part_id,
                       mt_index_t cur_model,
                       mt_index_t n_models,
                       modeltest::Model *model,
                       mt_index_t thread_id,
                       double epsilon_param,
                       double epsilon_opt,
                       time_t global_ini_time = 0,
                       std::ostream &out = std::cout);
};

}
#endif // MODELTEST_H
