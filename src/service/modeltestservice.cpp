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

#include "model_defs.h"
#include "service/modeltestservice.h"

#include <sstream>
#include <algorithm>

using namespace modeltest;
using namespace std;

ModelTestService::~ModelTestService()
{
    destroy_instance();
}

bool ModelTestService::test_msa( string const& msa_filename,
                                 mt_size_t * n_seqs,
                                 mt_size_t * n_sites,
                                 msa_format_t * msa_format,
                                 data_type_t * datatype)
{
    return test_msa(msa_filename,
                               n_seqs, n_sites,
                               msa_format, datatype);
}

bool ModelTestService::test_tree( string const& tree_filename,
                                  mt_size_t * n_tips )
{
    return test_tree(tree_filename, n_tips);
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
    for (Observer * observer : observers)
        mopt->attach(observer);
    mopt->run(epsilon_param, epsilon_opt);
    delete mopt;

    return true;
}

bool ModelTestService::evaluate_models(partition_id_t const& part_id,
                                       mt_size_t n_procs,
                                       double epsilon_param,
                                       double epsilon_opt,
                                       ostream &out)
{
    assert(modeltest_instance);
    return modeltest_instance->evaluate_models(part_id,
                                               n_procs,
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
    out << "Commands:" << endl;
    out << "  > phyml " << get_phyml_command_line(*(selection.get_model(0).model)) << endl;
    out << "  > raxmlHPC-SSE3 " << get_raxml_command_line(*(selection.get_model(0).model)) << endl;
    out << "  > paup " << get_paup_command_line(*(selection.get_model(0).model)) << endl;
    out << "  > iqtree " << get_iqtree_command_line(*(selection.get_model(0).model)) << endl;

    return true;
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

typedef struct
{
  mt_index_t id;
  double aic_support;
  double aicc_support;
  double bic_support;
  char * tree_str;
} topo_info_t;

void ModelTestService::topological_summary(partition_id_t const& part_id,
                                           ModelSelection const& bic_selection,
                                           ModelSelection const& aic_selection,
                                           ModelSelection const& aicc_selection)
{
  const vector<Model *> &c_models = modeltest_instance->get_models(part_id);
  Partition &partition = modeltest_instance->get_partition(part_id);
  pll_split_t **all_splits = (pll_split_t**) calloc(c_models.size(), sizeof(pll_split_t*));
  mt_size_t n_models = c_models.size();
  mt_size_t n_tips = partition.get_n_sequences();
  mt_size_t n_splits;
  mt_index_t *topo_v = new mt_index_t[n_models]();
  mt_size_t n_topologies;
  vector<topo_info_t> topologies;

  for (mt_index_t tree_id = 0; tree_id < n_models; ++tree_id)
  {
    all_splits[tree_id] = pllmod_utree_split_create(c_models[tree_id]->get_tree(),
                                                    n_tips,
                                                    &n_splits);
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

  cout << endl << "There are " << n_topologies << " different topologies\n";
  topologies.resize(n_topologies);

  for (mt_index_t tree_id = 0; tree_id < c_models.size(); ++tree_id)
  {
    pllmod_utree_split_destroy(all_splits[tree_id]);
    if (!topologies[topo_v[tree_id] - 1].id)
    {
      topologies[topo_v[tree_id] - 1].id = topo_v[tree_id];
      topologies[topo_v[tree_id] - 1].tree_str = pll_utree_export_newick(c_models[tree_id]->get_tree());
    }
    topologies[topo_v[tree_id] - 1].bic_support += bic_selection.get_weight(c_models[tree_id]);
    topologies[topo_v[tree_id] - 1].aic_support += aic_selection.get_weight(c_models[tree_id]);
    topologies[topo_v[tree_id] - 1].aicc_support += aicc_selection.get_weight(c_models[tree_id]);
  }

  delete[] topo_v;
  free(all_splits);

  for (mt_index_t i = 0; i < n_topologies; ++i)
  {
    cout << topologies[i].id << " " << topologies[i].bic_support << " " << topologies[i].aic_support << " " << topologies[i].aicc_support << " " << topologies[i].tree_str << endl;
    free(topologies[i].tree_str);
  }
  cout << endl;

}

string ModelTestService::get_iqtree_command_line(Model const& model,
                                                 string const& msa_filename) const
{
    //TODO:
    stringstream iqtree_args;
    //mt_index_t matrix_index = model.get_matrix_index();

    iqtree_args << "-s " << msa_filename << " -m " << model.get_name();
    if (model.is_G())
      iqtree_args << model.get_n_categories();

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

string ModelTestService::get_raxml_command_line(Model const& model,
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
        raxml_args << prot_model_names[matrix_index];
        if (model.is_F())
            raxml_args << "F";
    }
    raxml_args << " -n EXEC_NAME";
    raxml_args << " -p PARSIMONY_SEED";

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
