#include "modeltestservice.h"
#include "model_defs.h"

#include <algorithm>
#include <sstream>

using namespace modeltest;
using namespace std;

ModelTestService::~ModelTestService()
{
    destroy_instance();
}

bool ModelTestService::test_msa( std::string const& msa_filename,
               mt_size_t * n_seqs,
               mt_size_t * n_sites,
               data_type_t * datatype)
{
    return ModelTest::test_msa(msa_filename, n_seqs, n_sites, datatype);
}

bool ModelTestService::test_tree( std::string const& tree_filename,
                                  mt_size_t * n_tips )
{
    return ModelTest::test_tree(tree_filename, n_tips);
}

bool ModelTestService::create_instance( mt_options_t & options )
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

bool ModelTestService::reset_instance( mt_options_t & options )
{
    if (modeltest_instance)
        destroy_instance();
    return create_instance(options);
}

bool ModelTestService::optimize_single(const partition_id_t &part_id,
                     modeltest::Model *model,
                     mt_index_t thread_id,
                     double epsilon_param,
                     double epsilon_opt,
                     const std::vector<Observer *> observers)
{
    assert(modeltest_instance);

    ModelOptimizer * mopt = modeltest_instance->get_model_optimizer(model,
        part_id,
        thread_id);
    for (Observer * observer : observers)
        mopt->attach(observer);
    mopt->run(epsilon_param, epsilon_opt);
    delete mopt;

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

mt_size_t ModelTestService::get_number_of_models(partition_id_t const& part_id) const
{
    return (mt_size_t) modeltest_instance->get_models(part_id).size();
}

Model * ModelTestService::get_model(partition_id_t const& part_id, mt_index_t model_idx) const
{
    return modeltest_instance->get_models(part_id).at(model_idx);
}

PartitioningScheme & ModelTestService::get_partitioning_scheme( void ) const
{
    return modeltest_instance->get_partitioning_scheme();
}

string ModelTestService::get_raxml_command_line(Model const& model,
                                                string const& msa_filename)
{
    stringstream raxml_args;
    mt_index_t matrix_index = model.get_matrix_index();

    raxml_args << "-s " << msa_filename;
    if (model.get_datatype() == dt_dna)
    {
        raxml_args << " -m GTRGAMMA";
        if (model.is_I())
            raxml_args << "I";
        if (model.is_F())
            raxml_args << "X";

        mt_index_t standard_matrix_index = (mt_index_t) (find(raxml_matrices_indices,
                                         raxml_matrices_indices + N_DNA_RAXML_MATRICES,
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

string ModelTestService::get_phyml_command_line(modeltest::Model const& model,
                                   mt_options_t const& exec_opt)
{
    stringstream phyml_args;
    mt_index_t matrix_index = model.get_matrix_index();

    phyml_args << " -i " << exec_opt.msa_filename;
    if (model.get_datatype() == dt_dna)
    {
        stringstream matrix_symmetries;
        const int *sym_v = model.get_symmetries();
        for (mt_index_t i=0; i<N_DNA_SUBST_RATES; i++)
            matrix_symmetries << sym_v[i];
        phyml_args << " -m " << matrix_symmetries;
        if (model.is_F())
            phyml_args << " -f m";
        else
            phyml_args << " -f 0.25,0.25,0.25,0.25";
    }
    else
    {
        phyml_args << " -d aa";
        phyml_args << " -m ";
        phyml_args << prot_model_names[matrix_index];
        if (model.is_F())
            phyml_args << " -f e";
        else
            phyml_args << " -f m";
    }
    if (model.is_I())
        phyml_args << " -v e";
    else
        phyml_args << " -v 0";
    if (model.is_G())
        phyml_args << " -a e -c " << exec_opt.n_catg;
    else
        phyml_args << " -a 0 -c 1";
    phyml_args << " -o tlr";

    return phyml_args.str();
}
