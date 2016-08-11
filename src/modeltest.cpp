#include "modeltest.h"

#include "utils.h"
#include "msapll.h"
#include "treepll.h"
#include "thread/threadpool.h"

#include <iostream>
#include <sstream>
#include <cassert>
#include <algorithm>

namespace modeltest {

int mt_errno;
char mt_errmsg[MT_ERROR_LENGTH] = {0};
double alpha_guess = 1.0;
double pinv_guess  = 1.0;
double alpha_inv_guess  = 1.0;
double pinv_alpha_guess = 1.0;
int verbosity = VERBOSITY_DEFAULT;

using namespace std;

ModelTest::ModelTest(mt_size_t _number_of_threads)
    : number_of_threads(_number_of_threads)
{
    setlocale(LC_NUMERIC, "C");
    partitioning_scheme = 0;
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
  bool optimize_topology;

    if( thread_number > number_of_threads)
    {
        return 0;
    }

    optimize_topology = current_instance->start_tree == tree_ml;
    MsaPll *msa = static_cast<MsaPll *>(current_instance->msa);
    TreePll *tree = static_cast<TreePll *>(current_instance->tree);
    Partition &partition = partitioning_scheme->get_partition(part_id);
    return new ModelOptimizerPll(*msa, *tree, *model,
                                 partition,
                                 optimize_topology,
                                 current_instance->n_catg,
                                 thread_number);
}

static void print_execution_header( bool print_elapsed_time )
{
    /* print header */
    cout << setw(11) << right << " ----ID--- "
         << setw(15) << left << " ----MODEL---- "
         << setw(11) << "---Time---";
    if (print_elapsed_time)
        cout << setw(11) << "-Elapsed---";
    cout << setw(18) << right
         << " -------LnL-------"
         << setw(8) << " -Alpha-"
         << setw(8) << " -P-inv-"
         << endl;
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

int ModelTest::eval_and_print(const partition_id_t &part_id,
                          mt_index_t cur_model,
                          mt_index_t n_models,
                          modeltest::Model *model,
                          mt_index_t thread_id,
                          double epsilon_param,
                          double epsilon_opt,
                          time_t global_ini_time)
{
    time_t ini_t = time(NULL);

    int res = evaluate_single_model(model,
                                    part_id,
                                    thread_id,
                                    epsilon_param,
                                    epsilon_opt);

    if (!res)
    {
        cerr << "ERROR OPTIMIZING MODEL" << endl;
        return(MT_ERROR_OPTIMIZE);
    }

//    ostringstream out;
//    model->output_log(out);

//    istringstream sinstr;
//    sinstr.str(out.str());
//    cout << out.str() << endl;
//    model->input_log(sinstr);

    /* print progress */
    model->print_inline(cur_model+1, n_models, ini_t, global_ini_time);

     return res;
}

bool ModelTest::evaluate_models(const partition_id_t &part_id,
                                mt_size_t n_procs,
                                double epsilon_param,
                                double epsilon_opt)
{
    Partition &partition = partitioning_scheme->get_partition(part_id);

    mt_index_t cur_model;
    mt_size_t n_models = partition.get_number_of_models();
    time_t global_ini_time = time(NULL);

    assert(n_procs > 0);

    if (!n_models)
        return true;

    if (n_procs == 1)
    {
        print_execution_header( global_ini_time );

        for (cur_model=0; cur_model < n_models; cur_model++)
        {
            Model *model = partition.get_model(cur_model);
            eval_and_print(part_id, cur_model, n_models, model, 0,
                           epsilon_param, epsilon_opt,
                           global_ini_time);
        }
    }
    else
    {
        std::cerr << "Creating pool with " << n_procs << " threads" << std::endl;
        ThreadPool pool(n_procs);
        std::vector< std::future<int> > results;
        std::map<thread::id, mt_index_t> thread_map = pool.worker_ids;

        std::cerr << "Starting jobs... (output might be unsorted)"
                  << std::endl << std::endl;

        print_execution_header( global_ini_time );

        for (cur_model=0; cur_model < n_models; cur_model++)
        {
            Model *model = partition.get_model(cur_model);

            results.emplace_back(
              pool.enqueue([cur_model, model, n_models, part_id, epsilon_param, epsilon_opt, this, &thread_map, global_ini_time] {
                thread::id my_id(__gthread_self());
                int res = eval_and_print(part_id, cur_model, n_models, model,
                                         thread_map[my_id],
                                         epsilon_param, epsilon_opt,
                                         global_ini_time);
                return res;
              })
            );
        }
    }
    return true;
}

bool ModelTest::test_msa(std::string const& msa_filename,
                         mt_size_t *n_tips,
                         mt_size_t *n_sites,
                         msa_format_t *msa_format,
                         data_type_t *datatype)
{
   return MsaPll::test(msa_filename, n_tips, n_sites, msa_format, datatype);
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

    /* number of sequences and tips must agree */
    if (msa->get_n_sequences() == tree->get_n_tips())
        n_tips = msa->get_n_sequences();
    else
    {
        mt_errno = MT_ERROR_IO;
        snprintf(mt_errmsg, ERR_MSG_SIZE,
                 "Number of sequences/taxa differs: %d tips / %d sequences",
                 msa->get_n_sequences(), tree->get_n_tips());
        return false;
    }

    vector<bool> tree_taxa_found(n_tips);

    /* find sequences and link them with the corresponding taxa */
    for (i = 0; i < n_tips; ++i)
    {
        bool found = false;
        const char * header = msa->get_header (i);
        for (j = 0; j < n_tips; ++j)
        {
            if (!tree->get_label(j).compare(header))
            {
                if (tree_taxa_found[j])
                {
                    mt_errno = MT_ERROR_ALIGNMENT_DUPLICATED;
                    snprintf(mt_errmsg, ERR_MSG_SIZE, "Duplicated sequence %s", header);
                    return false;
                }
                tree_taxa_found[j] = true;
                found = true;
                break;
            }
        }

        if (!found)
        {
            mt_errno = MT_ERROR_TREE_MISSING;
            snprintf(mt_errmsg, ERR_MSG_SIZE, "Missing taxon %s in tree", header);
            return false;
        }
    }

    for (i=0;i<n_tips;i++)
    {
        if (!(tree_taxa_found[i] ))
        {
            mt_errno = MT_ERROR_ALIGNMENT_MISSING;
             snprintf(mt_errmsg, ERR_MSG_SIZE, "Missing sequence %s in MSA",tree->get_label(i).c_str());
            return false;
        }
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
    for (const partition_descriptor_t &partition : scheme)
        for (const partition_region_t &region : partition.regions)
        {
            if (region.start < 1 || region.start > n_sites ||
                region.end < region.start || region.end > n_sites ||
                region.stride < 1)
            {
                /* out of bounds */
                mt_errno = MT_ERROR_PARTITIONS_OUTBOUNDS;
                snprintf(mt_errmsg, ERR_MSG_SIZE, "Partition %s is out of bounds",partition.partition_name.c_str());
                free(sites);
                return false;
            }
            for (mt_index_t i=(region.start-1); i<region.end; i+=region.stride)
            {
                if (sites[i])
                {
                    /* overlapping */
                    mt_errno = MT_ERROR_PARTITIONS_OVERLAP;
                    snprintf(mt_errmsg, ERR_MSG_SIZE, "Partition %s overlaps another partition",partition.partition_name.c_str());
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
            snprintf(mt_errmsg, ERR_MSG_SIZE, "Site %d is not assigned to any partition", i+1);
            break;
        }
    }
    free(sites);
    return true;
}

bool ModelTest::build_instance(mt_options_t & options)
{
    free_stuff ();
    create_instance ();

    if (options.msa_format == mf_fasta)
      current_instance->msa = new MsaPll (options.msa_filename, options.n_taxa);
    else if (options.msa_format == mf_phylip)
      current_instance->msa = new MsaPll (options.msa_filename, options.msa_format);
    else
      return false;

    current_instance->start_tree = options.starting_tree;

    if (options.partitions_desc)
    {
        if (options.partitions_eff)
            delete options.partitions_eff;
        options.partitions_eff = new std::vector<partition_descriptor_t>(*options.partitions_desc);
        if (!current_instance->msa->reorder_sites(*options.partitions_eff))
        {
            return false;
        }
        current_instance->partitions_eff = options.partitions_eff;
    }

    verbosity = options.verbose;

//    /* evaluate partitions */
//    for (partition_descriptor_t & partition : *options.partitions_eff)
//    {
//        /* compute empirical frequencies */
//        if (!current_instance->msa->compute_empirical_frequencies(partition, options.smooth_freqs))
//        {
//            std::cerr << "Error computing frequencies in " << partition.partition_name << std::endl;
//            return false;
//        }
//        if (!current_instance->msa->compute_empirical_pinv(partition))
//        {
//            std::cerr << "Error computing invariant sites in " << partition.partition_name << std::endl;
//            return false;
//        }
//        if (partition.datatype == dt_dna && !current_instance->msa->compute_empirical_subst_rates(partition))
//        {
//            std::cerr << "Error computing invariant sites in " << partition.partition_name << std::endl;
//            return false;
//        }
//    }

    /* create starting tree */
    switch (options.starting_tree)
    {
    case tree_user_fixed:
        if( options.tree_filename.compare ("") )
      {
        try {
                current_instance->tree = new TreePll (options.starting_tree, options.tree_filename, number_of_threads);
            }
            catch(int e)
            {
                UNUSED(e);
                free_stuff();
                return false;
            }
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
        snprintf(mt_errmsg, ERR_MSG_SIZE, "User tree is not defined");
        return false;
      }
        break;
    case tree_random:
    case tree_mp:
    case tree_ml_gtr_fixed:
    case tree_ml_jc_fixed:
        try {
          current_instance->tree = new TreePll (options.starting_tree, *current_instance->msa, number_of_threads);
        }
        catch(int e)
        {
            switch (e)
            {
            case EXCEPTION_TREE_MISSING:
                mt_errno = MT_ERROR_TREE;
                break;
            case EXCEPTION_TREE_SCRIPT:
                mt_errno = MT_ERROR_IO;
                break;
            case EXCEPTION_TREE_USER:
                mt_errno = MT_ERROR_TREE;
                break;
            default:
                assert(0);
            }
            free_stuff();
            return false;
        }
        break;
    case tree_ml:
        mt_errno = MT_ERROR_UNIMPLEMENTED;
        snprintf(mt_errmsg, ERR_MSG_SIZE, "Per-model ML tree is not implemented yet");
        return false;
    }

    /* print tree */
    if (options.output_tree_to_file)
    {
        string nw = current_instance->tree->newick();
        if (!Utils::append_to_file(options.output_tree_file, nw))
        {
            mt_errno = MT_ERROR_IO;
            snprintf(mt_errmsg, ERR_MSG_SIZE, "Cannot write starting tree: %s",
                     options.output_tree_file.c_str());
            return false;
        }
    }

    if (current_instance->msa->get_n_sequences() !=
	current_instance->tree->get_n_tips())
      {
        mt_errno = MT_ERROR_TREE;
        snprintf(mt_errmsg, ERR_MSG_SIZE, "Tips do not agree! %d sequences vs %d tips",
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

    assert(!partitioning_scheme);
    partitioning_scheme = new PartitioningScheme();
    mt_index_t cur_part_id = 0;
    for (partition_descriptor_t & partition : (*current_instance->partitions_eff))
    {
        partition_id_t part_id(1);
        part_id[0] = cur_part_id;
        Partition * new_part;
        try {
            if (partition.datatype == dt_dna)
                new_part = new Partition(part_id,
                                     *current_instance->msa,
                                     *current_instance->tree,
                                     partition,
                                     options.nt_candidate_models,
                                     options.model_params);
            else if (partition.datatype == dt_protein)
                new_part = new Partition(part_id,
                                     *current_instance->msa,
                                     *current_instance->tree,
                                     partition,
                                     options.aa_candidate_models,
                                     options.model_params);
            else
                assert(0);
        }
        catch(int e)
        {
            switch (e)
            {
            case EXCEPTION_PARTITION_EMP_FREQS:
                mt_errno = MT_ERROR_ALIGNMENT_ILLEGAL;
                break;
            case EXCEPTION_PARTITION_EMP_RATES:
                mt_errno = MT_ERROR_ALIGNMENT_ILLEGAL;
                break;
            case EXCEPTION_PARTITION_EMP_PINV:
                mt_errno = MT_ERROR_ALIGNMENT_ILLEGAL;
                break;
            default:
                assert(0);
            }
            free_stuff();
            return false;
        }

        /*
         * sort candidate models by
         * estimated computational needs
         */
        new_part->sort_models(number_of_threads == 1);

        partitioning_scheme->add_partition(part_id, new_part);
        cur_part_id++;
    }

    return true;
}

vector<Model *> const& ModelTest::get_models(partition_id_t const& part_id)
{
    return partitioning_scheme->get_partition(part_id).get_models();
}

bool ModelTest::set_models(const std::vector<Model *> &c_models,
                           const partition_id_t &part_id)
{
    return partitioning_scheme->get_partition(part_id).set_models(c_models);
}

PartitioningScheme & ModelTest::get_partitioning_scheme( void ) const
{
    return *partitioning_scheme;
}

void ModelTest::free_stuff()
{
    if (current_instance)
    {
        if (current_instance->msa)
            delete current_instance->msa;
        if (current_instance->tree)
            delete current_instance->tree;

        if (partitioning_scheme)
        {
            partitioning_scheme->delete_partitions();
            delete partitioning_scheme;
            partitioning_scheme = 0;
        }

        delete current_instance;
        current_instance = 0;
    }
}
}
