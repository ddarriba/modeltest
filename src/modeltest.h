#ifndef MODELTEST_H
#define MODELTEST_H

#include "global_defs.h"
#include "msa.h"
#include "tree.h"
#include "tree.h"
#include "model.h"
#include "model_selection.h"
#include "model_optimizer_pll.h"
#include "partition.h"

#include <map>

#include "plldefs.h"

namespace modeltest {

typedef std::map<partition_id_t, Partition *> partitions_map_t;

typedef struct
{
  modeltest::Msa * msa;                       //! input MSA
  modeltest::Tree * tree;                     //! user defined tree (optional)
  std::vector<partition_t> * partitions_eff;  //! partitioning

  tree_type start_tree;           //! starting tree type
  mt_size_t sample_size;          //! sample size for model selection

  mt_size_t n_tips;               //! number of tips
  mt_size_t n_catg;               //! number of Gamma rate categories
} selection_instance;

class ModelTest
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
                  mt_size_t *n_sites);

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
    bool build_instance(mt_options & options);

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
                               double tolerance = 0.0001,
                               double epsilon = 0.001);

    /**
     * @brief Optimizes the whole candidate models set
     * @param[in] part_id      partition to optimize
     * @return true, if the optimization is OK
     */
    bool evaluate_models(const partition_id_t &part_id);

    ModelOptimizer * get_model_optimizer(Model * model,
                                         const partition_id_t &part_id,
                                         mt_index_t thread_number = 0);

    /**
     * @brief Gets the set of candidate models
     * @param[in] part_id  the partition id
     * @return the set of candidate models
     */
    const std::vector<Model *> & get_models(const partition_id_t &part_id);

    /**
     * @brief Updates the existing models with the values of another set
     * @param[in] c_models candidate models
     * @param[in] part_id  the partition id
     * @return false if the models cannot be updated
     */
    bool set_models(const std::vector<Model *> &c_models,
                    const partition_id_t &part_id);

private:
    mt_size_t number_of_threads;            //! number of threads
    selection_instance * current_instance;  //! model optimization parameters
    partitions_map_t partitions;            //! partitions

    /**
     * @brief Creates a new set of model optimization parameters
     */
    void create_instance();

    /**
     * @brief Deallocates all the memory
     */
    void free_stuff();
};

}
#endif // MODELTEST_H
