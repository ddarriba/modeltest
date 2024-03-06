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

#include "modeltestservice.h"
#include "../model_defs.h"
#include "../thread/parallel_context.h"

#include <sstream>
#include <algorithm>

using namespace modeltest;
using namespace std;

static string raxmlng_name(Model const& model);

ModelTestService::~ModelTestService()
{
    destroy_instance();
}

bool ModelTestService::create_instance( mt_options_t & options )
{
    bool build_ok;
    if(modeltest_instance)
        return false;

    modeltest_instance = new ModelTest(options.n_threadprocs, options.n_mpiprocs, options.n_threads);
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
                     Model *model,
                     mt_index_t thread_id,
                     double epsilon_param,
                     double epsilon_opt,
                     const vector<Observer *> observers)
{
    assert(modeltest_instance);

    ModelOptimizer * mopt = modeltest_instance->get_model_optimizer(model,
        part_id,
        thread_id);

    if (!mopt)
      return false;

    for (Observer * observer : observers)
        mopt->attach(observer);
    mopt->run(epsilon_opt, epsilon_param);
    delete mopt;

    return true;
}

bool ModelTestService::evaluate_models(partition_id_t const& part_id,
                                       mt_size_t n_threadprocs,
                                       double epsilon_param,
                                       double epsilon_opt,
                                       ostream &out)
{
    assert(modeltest_instance);

    return modeltest_instance->evaluate_models(part_id,
                                               n_threadprocs,
                                               epsilon_param,
                                               epsilon_opt,
                                               out);
}

ModelSelection * ModelTestService::select_models(partition_id_t const& part_id,
                                                 ic_type type)
{
    return new ModelSelection(modeltest_instance->get_models(part_id), type);
}

bool ModelTestService::print_selection(ModelSelection const& selection,
                                       ostream  &out) const
{
    selection.print(out, 10);
    selection.get_name();
    out << "Best model according to " << selection.get_name() << endl;
    out << "---------------------------" << endl;
    selection.print_best_model(out);
    out << "---------------------------" << endl;
    out << "Parameter importances" << endl;
    out << "---------------------------" << endl;
    selection.print_importances(out);
    out << "---------------------------" << endl;
    out << "Model averaged estimates" << endl;
    out << "---------------------------" << endl;
    selection.print_averages(out);
    out << endl;

    return true;
}

void ModelTestService::print_command_lines(modeltest::Model const& model,
                                           std::string const& msa_filename,
                                           std::ostream  &out) const
{
  out << "Commands:" << endl;
  out << "  > phyml " << get_phyml_command_line(model, msa_filename) << endl;
  out << "  > raxmlHPC-SSE3 " << get_raxml8_command_line(model, msa_filename) << endl;
  out << "  > raxml-ng " << get_raxmlng_command_line(model, msa_filename) << endl;
 // out << "  > paup " << get_paup_command_line(model, msa_filename) << endl;
  out << "  > iqtree " << get_iqtree_command_line(model, msa_filename) << endl;
}

void ModelTestService::print_raxml_partitions(
                              const partitioning_scheme_t & partitions,
                              const vector<modeltest::selection_model> & models,
                              std::ostream  &out) const
{
  assert(partitions.size() == models.size());

  for (mt_index_t i=0; i < partitions.size(); ++i)
  {
    out << models[i].model->get_name();
    out << ", " << partitions[i].partition_name << " = ";
    for (mt_index_t j=0; j<partitions[i].regions.size(); ++j)
    {
      if (j>0) out << ", ";
      partition_region_t region = partitions[i].regions[j];
      out << region.start << "-" << region.end;
      if (region.stride > 1)
        out << "\\" << region.stride;
    }
    out << endl;
  }
}

mt_size_t ModelTestService::get_number_of_models(partition_id_t const& part_id) const
{
    return (mt_size_t) modeltest_instance->get_models(part_id).size();
}

Model * ModelTestService::get_model(partition_id_t const& part_id,
                                    mt_index_t model_idx) const
{
    return modeltest_instance->get_models(part_id).at(model_idx);
}

PartitioningScheme & ModelTestService::get_partitioning_scheme( void ) const
{
    return modeltest_instance->get_partitioning_scheme();
}

static void print_newick_recurse(pll_unode_t * node, std::ostream  &out)
{
  pll_unode_t * child;

  if (pllmod_utree_is_tip(node))
  {
    out <<  node->label;
    return;
  }

  out << "(";
  child = node->next;
  while(child != node)
  {
    print_newick_recurse(child->back, out);

    if (child->next != node)
      out << ",";

    child = child->next;
  }
  out << ")";
  if (node->data)
  {
    pll_consensus_data_t * cdata = (pll_consensus_data_t *) node->data;
    out << "[" << cdata->support << "]";
  }
}

static void print_newick(pll_unode_t * tree, std::ostream  &out)
{
  out << "(";
  print_newick_recurse(tree->back, out);
  pll_unode_t * child = tree->next;
  while(child != tree)
  {
    out << ",";
    print_newick_recurse(child->back, out);
    child = child->next;
  }
  out << ");";
}

typedef struct
{
  mt_index_t id;
  unsigned int models_count;
  double aic_support;
  double aicc_support;
  double bic_support;
  char * tree_str;
  pll_utree_t * tree_topo; /* sample of tree structure */
} topo_info_t;


static bool sort_topos_bic(const topo_info_t & t1, const topo_info_t & t2)
{
    /* sort by bic */
    return t1.bic_support > t2.bic_support;
}

bool ModelTestService::topological_summary(partition_id_t const& part_id,
                                           ModelSelection const& bic_selection,
                                           ModelSelection const& aic_selection,
                                           ModelSelection const& aicc_selection,
                                           std::string const& topologies_filename,
                                           std::ostream  &out)
{
  const vector<Model *> &c_models = modeltest_instance->get_models(part_id);
  Partition &partition = modeltest_instance->get_partition(part_id);
  pll_split_t **all_splits = (pll_split_t**) calloc(c_models.size(), sizeof(pll_split_t*));
  mt_size_t n_models = (mt_index_t) c_models.size();
  mt_size_t n_tips = partition.get_n_sequences();
  mt_index_t *topo_v = new mt_index_t[n_models]();
  mt_size_t n_topologies;
  vector<topo_info_t> topologies;

  for (mt_index_t tree_id = 0; tree_id < n_models; ++tree_id)
  {
    if (!c_models[tree_id]->get_tree_graph())
    {
      out << endl << "Cannot build topological summary (missing trees)" << endl << endl;
      return false;
    }
    all_splits[tree_id] = pllmod_utree_split_create(c_models[tree_id]->get_tree_graph(),
                                                    n_tips,
                                                    NULL);
  }

  n_topologies = 0;
  for (mt_index_t i=0; i<n_models; ++i)
  {
    if (!topo_v[i])
    {
      ++n_topologies;
      topo_v[i] = n_topologies;
    }
    for (mt_index_t j=i+1; j<n_models; ++j)
    {
      mt_size_t rf_dist = pllmod_utree_split_rf_distance(all_splits[i], all_splits[j], n_tips);
      if (!rf_dist)
        topo_v[j] = topo_v[i];
    }
  }

  out << endl << "There are " << n_topologies << " different topologies\n";
  topologies.resize(n_topologies);

  for (mt_index_t tree_id = 0; tree_id < c_models.size(); ++tree_id)
  {
    pllmod_utree_split_destroy(all_splits[tree_id]);
    if (!topologies[topo_v[tree_id] - 1].id)
    {
      topologies[topo_v[tree_id] - 1].id = topo_v[tree_id];
      topologies[topo_v[tree_id] - 1].tree_str = pll_utree_export_newick(c_models[tree_id]->get_tree_graph(), NULL);
      topologies[topo_v[tree_id] - 1].tree_topo = c_models[tree_id]->get_tree();
      topologies[topo_v[tree_id] - 1].models_count = 0;
      topologies[topo_v[tree_id] - 1].bic_support  = 0.0;
      topologies[topo_v[tree_id] - 1].aic_support  = 0.0;
      topologies[topo_v[tree_id] - 1].aicc_support = 0.0;
    }
    ++topologies[topo_v[tree_id] - 1].models_count;
    topologies[topo_v[tree_id] - 1].bic_support += bic_selection.get_weight(c_models[tree_id]);
    topologies[topo_v[tree_id] - 1].aic_support += aic_selection.get_weight(c_models[tree_id]);
    topologies[topo_v[tree_id] - 1].aicc_support += aicc_selection.get_weight(c_models[tree_id]);
  }

  sort(topologies.begin(),
       topologies.end(),
       sort_topos_bic);
  delete[] topo_v;
  free(all_splits);

  /* print topologies */
  if (topologies_filename.compare(""))
  {
    out << "Topologies written to " << topologies_filename << endl << endl;
    ofstream * topo_stream = modeltest::Utils::open_file_for_writing(topologies_filename);
    for (mt_index_t i = 0; i < n_topologies; ++i)
    {
        *topo_stream << topologies[i].id << " " << topologies[i].bic_support << " " << topologies[i].aic_support << " " << topologies[i].aicc_support << " " << topologies[i].tree_str << endl;
        free(topologies[i].tree_str);
    }
    topo_stream->close();
    delete topo_stream;
  }
  else
  {
    for (mt_index_t i = 0; i < n_topologies; ++i)
    {
        out << topologies[i].id << " " << topologies[i].bic_support << " " << topologies[i].aic_support << " " << topologies[i].aicc_support << " " << topologies[i].tree_str << endl;
        free(topologies[i].tree_str);
    }
    out << endl;
  }

  /* print topological support */
  out << "topo_id   models_count   bic_support   aic_support   aicc_support" << endl;
  out << "-----------------------------------------------------------------" << endl;
  for (mt_index_t i = 0; i < n_topologies; ++i)
  {
      out << setw(7) << topologies[i].id
          << setw(15) << topologies[i].models_count
          << setw(14) << fixed << setprecision(5) << topologies[i].bic_support
          << setw(14) << fixed << setprecision(5) << topologies[i].aic_support
          << setw(14) << fixed << setprecision(5) << topologies[i].aicc_support
          << endl;
  }
  out << endl;

  /* build weighted consensus tree */
  pll_utree_t ** trees; /* set of tree structures */
  double * weights;

  trees = (pll_utree_t **) malloc(n_topologies * sizeof(pll_utree_t *));
  weights = (double *) malloc(n_topologies * sizeof(double));
  for (mt_index_t i = 0; i < n_topologies; ++i)
  {
    trees[i] = topologies[i].tree_topo;
    weights[i] = topologies[i].bic_support;
    /* set node indices consistent with each other */
    if (i)
      assert(pllmod_utree_consistency_check(trees[0], trees[i]));
  }

  double sumw = 0.0;
  for (mt_index_t i = 0; i < n_topologies; ++i)
  {
    sumw += weights[i];
  }

  if (sumw > 1.0)
  {
    /* rounding error */
    assert( sumw - 1.0 < 1e-12 );
    sumw = 1.0;
  }
  pll_consensus_utree_t * constree = pllmod_utree_weight_consensus(trees,
                                                         weights,
                                                         0.0,
                                                         n_topologies);
  if (constree)
  {
    out << "extended majority-rule consensus: ";
    print_newick(constree->tree, out);
    out << endl;
    pllmod_utree_consensus_destroy(constree);
  }
  else
  {
    out << "error computing majority-rule consensus " << pll_errmsg << endl << endl;
  }

  constree = pllmod_utree_weight_consensus(trees,
                                           weights,
                                           sumw,
                                           n_topologies);
  if (constree)
  {
    out << "strict consensus: ";
    print_newick(constree->tree, out);
    out << endl << endl;
    pllmod_utree_consensus_destroy(constree);
  }
  else
  {
    out << "error computing strict consensus " << pll_errmsg << endl << endl;
  }

  free(weights);
  free(trees);

  return true;
}

string ModelTestService::get_iqtree_command_line(Model const& model,
                                                 string const& msa_filename) const
{
    stringstream iqtree_args;
    //mt_index_t matrix_index = model.get_matrix_index();

    iqtree_args << "-s " << msa_filename;
   
    // fix model name incompatibilities
    if (!model.get_name().find("JTT-DCMUT"))
    {
      iqtree_args << " -m JTTDCMut" << model.get_name().substr(9);
    }
    else
       iqtree_args << " -m " << model.get_name();

    return iqtree_args.str();
}

string ModelTestService::get_paup_command_line(Model const& model,
                                               string const& msa_filename) const
{
    UNUSED(model);

    //TODO:
    stringstream paup_args;
    //mt_index_t matrix_index = model.get_matrix_index();

    paup_args << "-s " << msa_filename;

    return paup_args.str();
}

string ModelTestService::get_raxml8_command_line(Model const& model,
                                                string const& msa_filename) const
{
    stringstream raxml_args;
    mt_index_t matrix_index = model.get_matrix_index();

    raxml_args << "-s " << msa_filename;
    if (model.get_datatype() == dt_dna)
    {
        if (model.is_G())
            raxml_args << " -m GTRGAMMA";
        else
            raxml_args << " -c 1 -m GTRCAT";
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
        // fix model name incompatibilities
        if (!model.get_name().find("JTT-DCMUT"))
          raxml_args << "JTTDCMUT";
        else
          raxml_args << prot_model_names[matrix_index];
        if (model.is_F())
            raxml_args << "F";
    }
    raxml_args << " -n EXEC_NAME";
    raxml_args << " -p PARSIMONY_SEED";

    return raxml_args.str();
}

string ModelTestService::get_raxmlng_command_line(Model const& model,
                                                string const& msa_filename) const
{
    stringstream raxml_args;

    raxml_args << "--msa " << msa_filename;

    raxml_args << " --model " << raxmlng_name(model);


    return raxml_args.str();
}

string ModelTestService::get_phyml_command_line(Model const& model,
                                                string const& msa_filename) const
{
    stringstream phyml_args;
    mt_index_t matrix_index = model.get_matrix_index();

    phyml_args << " -i " << msa_filename;
    if (model.get_datatype() == dt_dna)
    {
        stringstream matrix_symmetries;
        const int *sym_v = model.get_symmetries();
        for (mt_index_t i=0; i<N_DNA_SUBST_RATES; i++)
            matrix_symmetries << sym_v[i];
        phyml_args << " -m " << matrix_symmetries.str();
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
        phyml_args << " -a e -c " << model.get_n_categories();
    else
        phyml_args << " -a 0 -c 1";
    phyml_args << " -o tlr";

    return phyml_args.str();
}

void ModelTestService::finalize( bool force )
{
    if (s_instance)
      delete s_instance;
    s_instance = 0;

    ParallelContext::finalize(force);
}


/* static */

static string raxmlng_name(Model const& model)
{
  stringstream modelname;
  mt_index_t matrix_index = model.get_matrix_index();

  if (model.get_datatype() == dt_dna)
  {
     /* matrix name */
     mt_index_t standard_matrix_index = (mt_index_t) (find(dna_model_matrices_indices,
                                      dna_model_matrices_indices + N_DNA_MODEL_MATRICES,
                                      matrix_index) - dna_model_matrices_indices);
     if (standard_matrix_index < N_DNA_MODEL_MATRICES)
     {
         modelname << dna_model_names[2 * standard_matrix_index + (model.is_F()?1:0)];
     }
     else
     {
         modelname << dna_model_matrices[matrix_index];
         if (model.is_F())
         {
           modelname << "+FO";
         }
     }

     if (model.is_I())
        modelname << "+I";

     if (model.is_G())
        modelname << "+G" << model.get_n_categories();
  }
  else
  {
      modelname << prot_model_names[matrix_index];;
      if (model.is_I())
        modelname << "+I";
      if (model.is_G())
        modelname << "+G" << model.get_n_categories();
      if (model.is_F())
        modelname << "+FC";
  }

  if (model.get_asc_bias_corr() == asc_lewis)
  {
    modelname << "+ASC_LEWIS";
  }
  else if (model.get_asc_bias_corr() == asc_felsenstein)
  {
    const mt_size_t * asc_weights = model.get_asc_weights();
    modelname << "+ASC_FELS{" << asc_weights[0] << "}";
  }
  else if (model.get_asc_bias_corr() == asc_stamatakis)
  {
    const mt_size_t * asc_weights = model.get_asc_weights();
    modelname << "+ASC_STAM{";
    modelname << asc_weights[0];
    for (mt_index_t i=1; i<model.get_n_states(); ++i)
    {
      modelname << "/" << asc_weights[i];
    }
    modelname << "}";
  }

  return modelname.str();
}
