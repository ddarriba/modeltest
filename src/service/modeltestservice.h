#ifndef MODELTESTSERVICE_H
#define MODELTESTSERVICE_H

#include "global_defs.h"
#include "modeltest.h"
#include "model_selection.h"
#include "model/model.h"
#include "thread/observer.h"

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

    static bool test_msa(std::string const& msa_filename,
                         mt_size_t * n_seqs,
                         mt_size_t * n_sites,
                         msa_format_t *msa_format = 0,
                         data_type_t * datatype = 0);

    static bool test_tree( std::string const& tree_filename,
                           mt_size_t *n_tips );

    static bool initialized( void ) { return s_instance != NULL; }

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
                         double epsilon_opt);

    modeltest::ModelSelection * select_models(partition_id_t const& part_id,
                                              modeltest::ic_type type);

    bool print_selection(modeltest::ModelSelection * selection,
                         std::ostream  &out = std::cout) const;

    mt_size_t get_number_of_models(partition_id_t const& part_id) const;

    modeltest::PartitioningScheme & get_partitioning_scheme( void ) const;

    modeltest::Model * get_model(partition_id_t const& part_id,
                                 mt_index_t model_idx) const;

    modeltest::ModelTest * get_modeltest()
    {
        return modeltest_instance;
    }

    std::string get_iqtree_command_line(modeltest::Model const& model,
                        std::string const& msa_filename = "MSA_FILENAME") const;

    std::string get_paup_command_line(modeltest::Model const& model,
                        std::string const& msa_filename = "MSA_FILENAME") const;

    std::string get_raxml_command_line(modeltest::Model const& model,
                        std::string const& msa_filename = "MSA_FILENAME") const;

    std::string get_phyml_command_line(modeltest::Model const& model,
                        const std::string &msa_filename = "MSA_FILENAME") const;

//public:
//    ModelTestService(ModelTestService const&) = delete;
//    void operator=(ModelTestService const&)   = delete;
};

#endif // MODELTESTSERVICE_H
