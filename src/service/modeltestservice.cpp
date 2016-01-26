#include "modeltestservice.h"
#include "model_defs.h"

#include <algorithm>
#include <sstream>

using namespace modeltest;

ModelTestService::~ModelTestService()
{
    destroy_instance();
}

bool ModelTestService::test_msa( std::string const& msa_filename,
               mt_size_t * n_seqs,
               mt_size_t * n_sites)
{
    return ModelTest::test_msa(msa_filename, n_seqs, n_sites);
}

bool ModelTestService::test_tree( std::string const& tree_filename,
                                  mt_size_t * n_tips )
{
    return ModelTest::test_tree(tree_filename, n_tips);
}

//bool ModelTestService::test_partitions( std::string const&parts_filename,
//                                        mt_size_t * n_partitions )
//{
//    ModelTest::
//    ModelTest::test_partitions()
//}

bool ModelTestService::create_instance( mt_options & options )
{
    bool build_ok;
    if(modeltest_instance)
        return false;

    modeltest_instance = new ModelTest(options.n_threads);
    build_ok = modeltest_instance->build_instance(options);

    return build_ok;
}

bool ModelTestService::destroy_instance( void )
{
    if(!modeltest_instance)
        return false;

    delete modeltest_instance;
    modeltest_instance = 0;

    return true;
}

bool ModelTestService::evaluate_models(partition_id_t const& part_id,
                                       mt_size_t n_procs,
                                       double epsilon_param,
                                       double epsilon_opt)
{
    assert(modeltest_instance);
    return modeltest_instance->evaluate_models(part_id,
                                               n_procs,
                                               epsilon_param,
                                               epsilon_opt);
}

string ModelTestService::get_raxml_command_line(Model const& model)
{
    std::stringstream raxml_args;
    mt_index_t matrix_index = model.get_matrix_index();

    raxml_args << "-s FILENAME";
    if (model.get_datatype() == dt_dna)
    {
        raxml_args << " -m GTRGAMMA";
        if (model.is_I())
            raxml_args << "I";
        if (model.is_F())
            raxml_args << "X";

        mt_index_t standard_matrix_index = (mt_index_t) (find(raxml_matrices_indices,
                                         raxml_matrices_indices + N_DNA_MODEL_MATRICES,
                                         matrix_index) - raxml_matrices_indices);

        switch (standard_matrix_index)
        {
        case DNA_JC_INDEX:
            raxml_args << " --JC69";
            break;
        case DNA_HKY_INDEX:
            if (model.is_F())
                raxml_args << " --HKY85";
            else
                raxml_args << " --K80";
            break;
        default:
            /* ignore */
            break;
        }
    }
    else
    {
        raxml_args << " -m PROTGAMMA";
        if (model.is_I())
            raxml_args << "I";
        raxml_args << prot_model_names[matrix_index];
        if (model.is_F())
            raxml_args << "F";
    }
    raxml_args << " -n EXEC_NAME";
    raxml_args << " -p PARSIMONY_SEED";

    return raxml_args.str();
}
