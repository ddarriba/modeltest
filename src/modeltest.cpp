#include "modeltest.h"

#include "utils.h"
#include "msapll.h"
#include "treepll.h"

#include <iostream>
#include <cassert>
#include <algorithm>

namespace modeltest {

unsigned int mt_errno;
char mt_errmsg[200] = {0};

using namespace std;

ModelTest::ModelTest(mt_size_t _number_of_threads)
    : number_of_threads(_number_of_threads)
{
    setlocale(LC_NUMERIC, "C");
    create_instance();
}

ModelTest::~ModelTest()
{
    free_stuff();
}

void ModelTest::create_instance()
{
    current_instance = new selection_instance;
    current_instance->msa = 0;
    current_instance->tree = 0;
}

ModelOptimizer * ModelTest::get_model_optimizer(Model * model,
                                     const partition_id_t &part_id,
                                     mt_index_t thread_number)
{
    if( thread_number > number_of_threads)
    {
        return 0;
    }
    MsaPll *msa = static_cast<MsaPll *>(current_instance->msa);
    TreePll *tree = static_cast<TreePll *>(current_instance->tree);
    const Partition * partition = partitions[part_id];
    return new ModelOptimizerPll(msa, tree, model,
                                 partition->get_descriptor(),
                                 current_instance->n_catg,
                                 thread_number);
}

bool ModelTest::evaluate_single_model(Model * model,
                                      const partition_id_t &part_id,
                                      mt_index_t thread_number,
                                      double tolerance,
                                      double epsilon)
{
    ModelOptimizer * mopt = get_model_optimizer(model,
                                                part_id,
                                                thread_number);
    assert(mopt);

    bool result = mopt->run(epsilon, tolerance);
    delete mopt;

    return result;
}

bool ModelTest::evaluate_models(const partition_id_t &part_id)
{
    assert(partitions.size());
    Partition * partition = partitions[part_id];
    assert(partition);

    vector<Model *> models = partition->get_models();
    for (Model * model : models)
    {
        evaluate_single_model(model, part_id);
    }
    return true;
}

bool ModelTest::test_msa(std::string const& msa_filename,
                         mt_size_t *n_tips,
                         mt_size_t *n_sites)
{
   return MsaPll::test(msa_filename, n_tips, n_sites);
}

bool ModelTest::test_tree(std::string const& tree_filename,
              mt_size_t *n_tips)
{
    return TreePll::test_tree(tree_filename, n_tips);
}

bool ModelTest::test_link(Msa const *msa,
                          Tree const *tree)
{
    mt_index_t i, j;
    mt_index_t n_tips;
    mt_size_t tree_taxa_found = 0;

    /* number of sequences and tips must agree */
    if (msa->get_n_sequences() == tree->get_n_tips())
        n_tips = msa->get_n_sequences();
    else
        return false;

    /* find sequences and link them with the corresponding taxa */
    for (i = 0; i < n_tips; ++i)
    {
        bool found = false;
        const char * header = msa->get_header (i);
        for (j = 0; j < n_tips; ++j)
        {
            if (!tree->get_label(j).compare(header))
            {
                if (tree_taxa_found &= 1<<j)
                {
                    mt_errno = MT_ERROR_ALIGNMENT_DUPLICATED;
                     snprintf(mt_errmsg, 200, "Duplicated sequence %s", header);
                    return false;
                }
                tree_taxa_found |= 1<<j;
                found = true;
                break;
            }
        }

        if (!found)
        {
            mt_errno = MT_ERROR_TREE_MISSING;
             snprintf(mt_errmsg, 200, "Missing taxon %s in tree", header);
            return false;
        }
    }

    i = 1;
    mt_index_t limit = (mt_index_t)1<<n_tips;
    while (i <= limit)
    {
        if (!(tree_taxa_found | i ))
        {
            mt_errno = MT_ERROR_ALIGNMENT_MISSING;
             snprintf(mt_errmsg, 200, "Missing sequence %s in MSA",tree->get_label(i).c_str());
            return false;
        }
        i = i<<1;
    }

    return true;
}

bool ModelTest::test_partitions(const partitioning_scheme_t &scheme,
                                mt_size_t n_sites)
{
    /* Overlapping:         FATAL */
    /* Uncovered sites:     WARN  */
    /* Sites out of bounds: FATAL */

    mt_errno = 0;
    char * sites = (char *) Utils::c_allocate(n_sites, sizeof(char));
    for (const partition_t &partition : scheme)
        for (const partition_region_t &region : partition.regions)
        {
            if (region.start < 1 || region.start > n_sites ||
                region.end < region.start || region.end > n_sites ||
                region.stride < 1)
            {
                /* out of bounds */
                mt_errno = MT_ERROR_PARTITIONS_OUTBOUNDS;
                snprintf(mt_errmsg, 200, "Partition %s is out of bounds",partition.partition_name.c_str());
                free(sites);
                return false;
            }
            for (mt_index_t i=(region.start-1); i<region.end; i+=region.stride)
            {
                if (sites[i])
                {
                    /* overlapping */
                    mt_errno = MT_ERROR_PARTITIONS_OVERLAP;
                    snprintf(mt_errmsg, 200, "Partition %s overlaps another partition",partition.partition_name.c_str());
                    free(sites);
                    return false;
                }
                sites[i] = 1;
            }
        }
    for (mt_index_t i=0; i<n_sites; i++)
    {
        if (!sites[i])
        {
            /* uncovered site */
            mt_errno = MT_WARN_PARTITIONS_UNASIGNED;
            snprintf(mt_errmsg, 200, "Site %d is not assigned to any partition", i+1);
            break;
        }
    }
    free(sites);
    return true;
}

bool ModelTest::build_instance(mt_options & options)
{
    free_stuff ();
    create_instance ();

    current_instance->msa = new MsaPll (options.msa_filename);
    current_instance->start_tree = options.starting_tree;

    if (options.partitions_desc)
    {
        if (options.partitions_eff)
            delete options.partitions_eff;
        options.partitions_eff = new std::vector<partition_t>(*options.partitions_desc);
        current_instance->msa->reorder_sites(*options.partitions_eff);
        current_instance->partitions_eff = options.partitions_eff;
        //current_instance->msa->print();
    }

    switch (options.starting_tree)
    {
    case tree_user_fixed:
        if( options.tree_filename.compare ("") )
      {
        current_instance->tree = new TreePll (options.starting_tree, options.tree_filename, number_of_threads);
        if (!test_link(current_instance->msa, current_instance->tree))
        {
            /* clean memory */
            delete current_instance->msa;
            current_instance->msa = 0;
            delete current_instance->tree;
            current_instance->tree = 0;
            return false;
        }
      }
    else
      {
        /* unimplemented */
        mt_errno = MT_ERROR_TREE;
        snprintf(mt_errmsg, 200, "User tree is not defined");
        return false;
      }
        break;
    case tree_mp:
    case tree_ml_gtr_fixed:
    case tree_ml_jc_fixed:
        current_instance->tree = new TreePll (options.starting_tree, options.msa_filename, number_of_threads);
        current_instance->tree->print();
        break;
    case tree_ml:
        mt_errno = MT_ERROR_UNIMPLEMENTED;
        snprintf(mt_errmsg, 200, "Per-model ML tree is not implemented yet");
        return false;
    }

    if (current_instance->msa->get_n_sequences() !=
	current_instance->tree->get_n_tips())
      {
        mt_errno = MT_ERROR_TREE;
         snprintf(mt_errmsg, 200, "Tips do not agree! %d sequences vs %d tips",
                 current_instance->msa->get_n_sequences(),
                 current_instance->tree->get_n_tips());
        return false;
      }
    current_instance->n_tips = current_instance->msa->get_n_sequences();

    /* evaluate partitions */
    if( options.partitions_filename.compare (""))
    {

    }

    if (options.model_params & (MOD_PARAM_GAMMA | MOD_PARAM_INV_GAMMA))
      current_instance->n_catg = options.n_catg;
    else
      current_instance->n_catg = 1;

    mt_index_t cur_part_id = 0;
    for (partition_t & partition : (*current_instance->partitions_eff))
    {
        partition_id_t part_id(1);
        part_id[0] = cur_part_id;
        Partition * new_part;
        if (partition.datatype == dt_dna)
            new_part = new Partition(current_instance->msa,
                                     current_instance->tree,
                                     partition,
                                     options.nt_candidate_models,
                                     options.model_params);
        else if (partition.datatype == dt_protein)
            new_part = new Partition(current_instance->msa,
                                     current_instance->tree,
                                     partition,
                                     options.aa_candidate_models,
                                     options.model_params);
        else
            assert(0);

        /*
         * sort candidate models by
         * estimated computational needs
         */
        new_part->sort_models(number_of_threads == 1);

        partitions[part_id] = new_part;
        cur_part_id++;
    }

    return true;
}

const vector<Model *> & ModelTest::get_models(const partition_id_t &part_id)
{
    return partitions[part_id]->get_models();
}

bool ModelTest::set_models(const std::vector<Model *> &c_models,
                           const partition_id_t &part_id)
{
    return partitions[part_id]->set_models(c_models);
}

void ModelTest::free_stuff()
{
    if (current_instance)
    {
        if (current_instance->msa)
            delete current_instance->msa;
        if (current_instance->tree)
            delete current_instance->tree;
        for (partitions_map_t::iterator it=partitions.begin(); it!=partitions.end(); ++it)
            delete it->second;
        partitions.clear();
        delete current_instance;
    }
}
}
