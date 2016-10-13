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

#include "modeltest.h"

#include "utils.h"
#include "msapll.h"
#include "treepll.h"
#include "genesis/logging.h"
#include "thread/threadpool.h"
#include "optimize/partition_optimizer.h"

#include <sstream>
#include <cassert>
#include <iostream>
#include <algorithm>

namespace modeltest {

int mt_errno;
char mt_errmsg[MT_ERROR_LENGTH] = {0};
double alpha_guess = 1.0;
double pinv_guess  = 1.0;
double alpha_inv_guess  = 1.0;
double pinv_alpha_guess = 1.0;
int verbosity = VERBOSITY_DEFAULT;
time_t global_ini_time = time(NULL);

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
                                     mt_index_t thread_number,
                                     bool force_opt_topo)
{
  bool opt_topology;

  if( thread_number > number_of_threads)
  {
      return 0;
  }

  opt_topology = force_opt_topo || current_instance->start_tree == tree_ml;
  MsaPll *msa = static_cast<MsaPll *>(current_instance->msa);
  TreePll *tree = static_cast<TreePll *>(current_instance->tree);

  Partition &partition = partitioning_scheme->get_partition(part_id);
  return new ModelOptimizerPll(*msa, *tree, *model,
                               partition,
                               opt_topology,
                               current_instance->n_catg,
                               thread_number);
}

static void print_execution_header( bool print_elapsed_time,
                                    ostream &out )
{
  /* print header */
  out << setw(11) << right << " ----ID--- "
      << setw(15) << left << " ----MODEL---- "
      << setw(11) << "---Time---";
  if (print_elapsed_time)
  {
    out << setw(11) << "-Elapsed---";
  }
  out << setw(18) << right
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
                          time_t global_ini_time,
                          ostream &out)
{
  time_t ini_t = time(NULL);
  int res = PLL_SUCCESS;

  if (!model->is_optimized())
  {
    res = evaluate_single_model(model,
                                part_id,
                                thread_id,
                                epsilon_param,
                                epsilon_opt);

    if (!res)
    {
        cerr << "ERROR OPTIMIZING MODEL" << endl;
        return(MT_ERROR_OPTIMIZE);
    }
  }

  /* print progress */
  model->print_inline(cur_model+1, n_models, ini_t, global_ini_time, out);

  return res;
}

bool ModelTest::evaluate_models(const partition_id_t &part_id,
                                mt_size_t n_procs,
                                double epsilon_param,
                                double epsilon_opt,
                                ostream &out)
{
  Partition &partition = partitioning_scheme->get_partition(part_id);

  mt_size_t n_models = partition.get_number_of_models();

  assert(n_procs > 0);

  if (!n_models)
      return true;

  bool opt_topology = current_instance->start_tree == tree_ml;
  MsaPll *msa = static_cast<MsaPll *>(current_instance->msa);
  TreePll *tree = static_cast<TreePll *>(current_instance->tree);

  print_execution_header( global_ini_time, out );

  part_opt_t opt_type;

  //TODO Check condition
  if (current_instance->rate_clustering)
    opt_type = partition_optimize_greedy;
  else
    opt_type = partition_optimize_all;

  PartitionOptimizer p_opt(partition,
                           *msa,
                           *tree,
                           current_instance->n_catg,
                           opt_type,
                           opt_topology,
                           epsilon_param,
                           epsilon_opt);
  p_opt.attach(this);
  p_opt.evaluate(n_procs);

  return true;
}

bool ModelTest::test_msa(string const& msa_filename,
                         mt_size_t *n_tips,
                         mt_size_t *n_sites,
                         msa_format_t *msa_format,
                         data_type_t *datatype)
{
 return MsaPll::test(msa_filename, n_tips, n_sites, msa_format, datatype);
}

bool ModelTest::test_tree(string const& tree_filename,
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
  {
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

  current_instance->ckp_enabled = options.write_checkpoint;
  if (current_instance->ckp_enabled)
  {
    current_instance->ckp_filename = options.checkpoint_file;

    if (!Utils::file_exists(current_instance->ckp_filename))
    {
      cout << "Creating checkpoint file: " << current_instance->ckp_filename << endl;
      pll_binary_header_t bin_header;
      FILE * bin_file = pllmod_binary_create(current_instance->ckp_filename.c_str(),
                           &bin_header,
                           PLLMOD_BIN_ACCESS_RANDOM,
                           2048);
      if (!bin_file)
      {
        printf("Cannot create binary file: %s\n", current_instance->ckp_filename.c_str());
        return false;
      }
      pllmod_binary_close(bin_file);
    }
  }

  if (options.msa_format == mf_fasta)
    current_instance->msa = new MsaPll (options.msa_filename, options.n_taxa);
  else if (options.msa_format == mf_phylip)
    current_instance->msa = new MsaPll (options.msa_filename, options.msa_format);
  else
    return false;

  current_instance->start_tree = options.starting_tree;
  current_instance->rate_clustering = options.rate_clustering;

  if (options.partitions_desc)
  {
    if (options.partitions_eff)
      delete options.partitions_eff;
    options.partitions_eff = new vector<partition_descriptor_t>(*options.partitions_desc);
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
//            cerr << "Error computing frequencies in " << partition.partition_name << endl;
//            return false;
//        }
//        if (!current_instance->msa->compute_empirical_pinv(partition))
//        {
//            cerr << "Error computing invariant sites in " << partition.partition_name << endl;
//            return false;
//        }
//        if (partition.datatype == dt_dna && !current_instance->msa->compute_empirical_subst_rates(partition))
//        {
//            cerr << "Error computing invariant sites in " << partition.partition_name << endl;
//            return false;
//        }
//    }

  /* create starting tree */
  switch (options.starting_tree)
  {
  case tree_user_fixed:
    if( options.tree_filename.compare ("") )
    {
      try
      {
        current_instance->tree = new TreePll (options.starting_tree,
                                              options.tree_filename,
                                              *current_instance->msa,
                                              number_of_threads);
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
    try
    {
      current_instance->tree = new TreePll (options.starting_tree,
                                            options.tree_filename,
                                            *current_instance->msa,
                                            number_of_threads);
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
    try
    {
    //TODO: Create MP as starting tree, instead of random
    current_instance->tree = new TreePll (options.starting_tree,
                                          options.tree_filename,
                                          *current_instance->msa,
                                          number_of_threads);
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
                                 options.model_params,
                                 options.checkpoint_file);
        else if (partition.datatype == dt_protein)
            new_part = new Partition(part_id,
                                 *current_instance->msa,
                                 *current_instance->tree,
                                 partition,
                                 options.aa_candidate_models,
                                 options.model_params,
                                 options.checkpoint_file);
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

  /* update tree */
  int start_matrix_index = 0;
  mt_mask_t start_model_params =
    (options.model_params & MOD_PARAM_ESTIMATED_FREQ) ?
      MOD_PARAM_ESTIMATED_FREQ : MOD_PARAM_FIXED_FREQ;
  start_model_params |=
    (options.model_params & MOD_PARAM_GAMMA) ?
      MOD_PARAM_GAMMA : MOD_PARAM_NO_RATE_VAR;

  switch (options.starting_tree)
  {
  case tree_user_fixed:
  case tree_random:
  case tree_mp:
  case tree_ml:
    /* ignore */
    break;
  case tree_ml_gtr_fixed:
  case tree_ml_jc_fixed:
    start_matrix_index = (options.starting_tree == tree_ml_jc_fixed?
      DNA_JC_INDEX : DNA_GTR_INDEX);
    Model * start_model = partitioning_scheme->get_partition({0}).get_model_by_matrix(start_matrix_index, start_model_params);
    assert (start_model);

    LOG_INFO << endl << "Optimizing tree for " << start_model->get_name() << endl;

    //TODO: Optimize per partition
    ModelOptimizer * start_opt = get_model_optimizer(start_model,
                                         {0},
                                          0,
                                          true);

    start_opt->run(options.epsilon_opt, options.epsilon_param);
    assert(start_model->is_optimized());
    LOG_INFO << "Starting tree fixed " << start_model->get_loglh() << endl;
    TreePll *tree = static_cast<TreePll *>(current_instance->tree);
    tree->set_pll_tree(pll_utree_clone(start_model->get_tree()));
    start_model->set_tree((pll_utree_t *)tree->extract_tree());

    delete start_opt;
    
    break;
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

  return true;
}

vector<Model *> const& ModelTest::get_models(partition_id_t const& part_id)
{
  return partitioning_scheme->get_partition(part_id).get_models();
}

bool ModelTest::set_models(const vector<Model *> &c_models,
                           const partition_id_t &part_id)
{
  return partitioning_scheme->get_partition(part_id).set_models(c_models);
}

void ModelTest::update(Observable * subject, void * data)
{
  //TODO: Use struct here
  opt_info_t * opt_info = static_cast<opt_info_t *>(data);
  opt_info->model->print_inline(opt_info->model_index, opt_info->n_models,
                                opt_info->start_time, global_ini_time,
                                MT_INFO);

  if (current_instance->ckp_enabled)
  {
    opt_info->model->output_bin(current_instance->ckp_filename);
  }
}

PartitioningScheme & ModelTest::get_partitioning_scheme( void ) const
{
  return *partitioning_scheme;
}

Partition & ModelTest::get_partition(partition_id_t const& part_id) const
{
  return partitioning_scheme->get_partition(part_id);
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
