#ifndef MODELTEST_H
#define MODELTEST_H

#include "msa.h"
#include "tree.h"
#include "tree.h"
#include "model.h"
#include "model_selection.h"
#include "model_optimizer_pll.h"

#include <string>
#include <vector>

#define MT_ERROR_UNIMPLEMENTED 10000
#define MT_ERROR_LIBPLL        10100
#define MT_ERROR_ALIGNMENT     10200
#define MT_ERROR_TREE          10300
#define MT_ERROR_INSTANCE      10400
#define MT_ERROR_OPTIMIZE      10500

#include "plldefs.h"

namespace modeltest {

typedef struct
{
  modeltest::Msa * msa;     //! input MSA
  modeltest::Tree * tree;   //! user defined tree (optional)
  tree_type start_tree;     //! starting tree type
  std::vector<modeltest::Model *> c_models; //! candidate models
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
     * @brief Creates a model    optimization instance
     * @param options            general optimization options
     * @param eval_all_matrices  evaluate 203 model matrices
     * @return true, if everything was OK, false in case of error
     */
    bool build_instance(mt_options & options,
                        bool eval_all_matrices);

    /**
     * @brief Optimizes the parameters for one single model
     * @param model          model to optimize
     * @param thread_number  number of the current thread
     * @param tolerance      tolerance for parameter optimization
     * @param epsilon        tolerance for global optimization
     * @return true, if the optimization is OK
     */
    bool evaluate_single_model(Model * model,
                               mt_index_t thread_number = 0,
                               double tolerance = 0.0001,
                               double epsilon = 0.001);

    /**
     * @brief Optimizes the whole candidate models set
     * @return true, if the optimization is OK
     */
    bool evaluate_models();

    ModelOptimizer * get_model_optimizer(Model * model,
                                         mt_index_t thread_number = 0) const;

    /**
     * @brief Gets the set of candidate models
     * @return the set of candidate models
     */
    std::vector<Model *> const& get_models() const;

    /**
     * @brief Updates the existing models with the values of another set
     * @param c_models candidate models
     * @return false if the models cannot be updated
     */
    bool set_models(const std::vector<Model *> &c_models);

private:
    mt_size_t number_of_threads;            //! number of threads
    selection_instance * current_instance;  //! model optimization parameters

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
