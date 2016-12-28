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

#include "treepll.h"
#include "utils.h"

#include <cerrno>
#include <iostream>

using namespace std;

/* a callback function for resetting missing branches */
static int cb_set_missing_branches(pll_utree_t * node, void *data)
{
  UNUSED(data);

  /* reset branches */
  if (!(node->length > 0.))
  {
    node->length = 0.1;
    node->back->length = 0.1;
  }

  return 1;
}

static int cb_set_names(pll_utree_t * node, void *data)
{
  modeltest::Msa *msa = (modeltest::Msa *) data;

  if (!node->label)
  {
    if (!node->next)
    {
      /* update name name */
      node->label = (char *) malloc (strlen(msa->get_header(node->node_index))+1);
      strcpy(node->label, msa->get_header(node->node_index));
    }
    else
    {
      node->label = (char *) malloc (1);
      node->next->label = (char *) malloc (1);
      node->next->next->label = (char *) malloc (1);
      strcpy(node->label,"");
      strcpy(node->next->label,"");
      strcpy(node->next->next->label,"");
    }
  }

  return 1;
}

static int cb_set_missing_names(pll_utree_t * node, void *data)
{
  pll_utree_t **tip_nodes = (pll_utree_t **) data;
  pll_utree_t **tip_nodes_ptr = tip_nodes;

  if (!node->label)
  {
    if (!node->next)
    {
      /* find name */
      while ((*tip_nodes_ptr)->node_index != node->node_index)
        ++tip_nodes_ptr;

      /* update missing name */
      node->label = (char *) malloc (strlen((*tip_nodes_ptr)->label)+1);
      strcpy(node->label, (*tip_nodes_ptr)->label);
    }
    else
    {
      node->label = (char *) malloc (2);
      node->next->label = (char *) malloc (2);
      node->next->next->label = (char *) malloc (2);
      strcpy(node->label,"A");
      strcpy(node->next->label,"A");
      strcpy(node->next->next->label,"A");
    }
  }

  return 1;
}

/* a callback function for resetting all branches */
static int cb_set_all_branches(pll_utree_t * node, void *data)
{
  double * length = (double *) data;

  /* reset branches */
  node->length = *length;
  node->back->length = *length;

  return 1;
}

static void scale_branch_length_recursive (pll_utree_t * tree,
                                           double factor)
{
  if (tree)
  {
    /* set branch length to default if not set */
    tree->length *= factor;
    tree->back->length *= factor;

    if (tree->next)
    {
      scale_branch_length_recursive (tree->next->back, factor);
      scale_branch_length_recursive (tree->next->next->back, factor);
    }
  }
}

/* scale all branch lengths */
static void scale_branch_length (pll_utree_t * tree,
                                 double factor)
{
  scale_branch_length_recursive (tree, factor);
  scale_branch_length_recursive (tree->back, factor);
}

namespace modeltest
{

  Tree::~Tree() {}

  static void set_indices(pll_utree_t * pll_tree, Msa &msa)
  {
    mt_size_t n_tips = msa.get_n_sequences();
    pll_utree_t ** tip_nodes;

    tip_nodes = (pll_utree_t **) Utils::c_allocate(n_tips,
                                                   sizeof(pll_utree_t *));
    pll_utree_query_tipnodes(pll_tree, tip_nodes);

    /* find sequences and link them with the corresponding taxa */
    for (mt_index_t i = 0; i < n_tips; ++i)
    {
        mt_index_t tip_clv_index = MT_SIZE_UNDEF;
        const char * header = msa.get_header (i);

        for (mt_index_t j = 0; j < n_tips; ++j)
        {
            if (!strcmp (tip_nodes[j]->label, header))
            {
                tip_nodes[j]->clv_index = tip_clv_index = i;
                break;
            }
        }

        if (tip_clv_index == MT_SIZE_UNDEF)
            cerr << "ERROR: Cannot find tip \"" << header << "\"" << endl;
        assert(tip_clv_index != MT_SIZE_UNDEF);
    }

    free(tip_nodes);
  }

  TreePll::TreePll (tree_type_t type,
                    string const& filename,
                    Msa &msa,
                    mt_size_t number_of_threads,
                    int random_seed)
    : Tree(type, filename, msa, number_of_threads, random_seed)
  {
    pll_utree_t * starting_tree;

    bl_optimized = false;
    pll_tree = (pll_utree_t **) Utils::allocate(number_of_threads, sizeof(pll_utree_t *));
    pll_start_tree = (pll_utree_t **) Utils::allocate(number_of_threads, sizeof(pll_utree_t *));
    pll_tip_nodes = (pll_utree_t ***) Utils::c_allocate(number_of_threads, sizeof(pll_utree_t **));
    pll_inner_nodes = (pll_utree_t ***) Utils::c_allocate(number_of_threads, sizeof(pll_utree_t **));

    if (ROOT)
    {
      if (filename.compare(""))
      {
        starting_tree = pll_utree_parse_newick (filename.c_str(), &(n_tips));
      }
      else if (type == tree_random)
      {
        starting_tree = pllmod_utree_create_random(n_tips, msa.get_headers());
      }
      else
      {
        cout << "Building MP starting tree" << endl;
        unsigned int * cost = new unsigned int[msa.get_n_patterns()];
        starting_tree = pllmod_utree_create_parsimony(n_tips,
                                                      msa.get_n_patterns(),
                                                      msa.get_headers(),
                                                      msa.get_sequences(),
                                                      msa.get_weights(),
                                                      pll_map_nt,
                                                      4,
                                                      0, /* attributes */
                                                      random_seed,
                                                      cost /* scores */
                                                    );
        delete[] cost;
      }
    }

    #if (MPI_ENABLED)

    /* broadcast starting topology */
    pll_utree_t * serialized_tree;
    if (ROOT)
    {
      serialized_tree = pllmod_utree_serialize(starting_tree,
                                               n_tips);

      MPI_Bcast(serialized_tree,
                sizeof(pll_utree_t) * (2*n_tips - 2),
                MPI_BYTE,
                0,
                master_mpi_comm );
    }
    else
    {
      serialized_tree = (pll_utree_t *) malloc(sizeof(pll_utree_t) * (2*n_tips - 2));
      MPI_Bcast(serialized_tree,
                sizeof(pll_utree_t) * (2*n_tips - 2),
                MPI_BYTE,
                0,
                master_mpi_comm );
      starting_tree = pllmod_utree_expand(serialized_tree, n_tips);
      update_names(starting_tree, &msa);
      free(serialized_tree);
    }
    #endif

    if (!starting_tree)
    {
      cleanup();
      snprintf(mt_errmsg, 400, "Error %d creating starting tree: %s", pll_errno, pll_errmsg);
      throw EXCEPTION_TREE_MP;
    }

    set_indices(starting_tree, msa);
    clone_tree( starting_tree );
    pll_utree_destroy(starting_tree, NULL);
  }

  TreePll::~TreePll ()
  {
    cleanup();
  }

  void TreePll::reroot_random(mt_index_t thread_number)
  {

    if (!pll_inner_nodes)
        cerr << "INER NODES FAIL" << endl;
    assert(pll_inner_nodes);

    /* move to random node */
    int inner_index = rand () % (int) n_inner;

    if (!pll_inner_nodes[thread_number])
        cerr << "INER NODES THREAD FAIL " << thread_number << " " << number_of_threads << endl;
    pll_tree[thread_number] = pll_inner_nodes[thread_number][inner_index];

    assert(pll_tree[thread_number]);
  }

  bool TreePll::set_branches(double length, mt_index_t thread_number)
  {
    assert(thread_number < number_of_threads);
    assert(length > 0);
    pllmod_utree_traverse_apply(pll_tree[thread_number],
                                NULL,
                                NULL,
                                &cb_set_all_branches,
                                &length);
    return true;
  }

  bool TreePll::scale_branches(double factor, mt_index_t thread_number)
  {
    assert(factor > 0);
    assert(thread_number < number_of_threads);
    scale_branch_length(pll_tree[thread_number], factor);
    return true;
  }

  bool TreePll::reset_branches(mt_index_t thread_number)
  {
    assert(thread_number < number_of_threads);
    return false;
  }

  bool TreePll::test_tree(std::string const& tree_filename, mt_size_t *n_tips)
  {
    pll_utree_t * tree = pll_utree_parse_newick (tree_filename.c_str(), n_tips);
    if (!tree)
    {
      mt_errno = pll_errno;
      snprintf(mt_errmsg, 400, "PLL Error %d testing tree: %s", pll_errno, pll_errmsg);
      return false;
    }
    else
    {
      pll_utree_destroy(tree, NULL);
      return true;
    }
  }

  const string TreePll::get_label( mt_index_t index, mt_index_t thread_number) const
  {
    assert(thread_number < number_of_threads);
    if (index >= n_tips)
        return "";
    else
        return pll_tip_nodes[thread_number][index]->label;
  }

  string TreePll::newick(mt_index_t thread_number) const
  {
    assert(thread_number < number_of_threads);
    char *nw_cstr = pll_utree_export_newick(pll_tree[thread_number]);
    string nw = string(nw_cstr);
    free (nw_cstr);
    return (nw);
  }

  void TreePll::set_pll_tree( pll_utree_t * new_tree, mt_index_t thread_number)
  {
    assert(pll_utree_check_integrity(new_tree));
    pll_utree_destroy(pll_tree[thread_number], NULL);
    pll_tree[thread_number] = new_tree;
    //pll_start_tree[thread_number] = new_tree;

    /* update node arrays */
    pll_utree_query_tipnodes(new_tree, pll_tip_nodes[thread_number]);
    pll_utree_query_innernodes(new_tree, pll_inner_nodes[thread_number]);
  }

  void TreePll::update_names( pll_utree_t * tree, Msa * msa )
  {
    if (msa)
    {
      pllmod_utree_traverse_apply(tree,
                                  NULL,
                                  NULL,
                                  &cb_set_names,
                                  msa);
    }
    else
    {
      assert(pll_tip_nodes[0]);
      pllmod_utree_traverse_apply(tree,
                                  NULL,
                                  NULL,
                                  &cb_set_missing_names,
                                  pll_tip_nodes[0]);
    }
  }

  void * TreePll::extract_tree ( mt_index_t thread_number) const
  {
    assert(thread_number < number_of_threads);
    pll_utree_t * new_tree = pll_utree_clone(pll_tip_nodes[thread_number][0]);
    return new_tree;
  }

  static double recurse_euclidean_distance(pll_utree_t * tree1, pll_utree_t * tree2)
  {
    double sum_branches = (tree1->length - tree2->length) * (tree1->length - tree2->length);

    /* ensure that topology is the same and we compare same branches */
    assert(tree1->clv_index == tree2->clv_index &&
           tree1->back->clv_index == tree2->back->clv_index);

    if (tree1->back->next)
    {
        sum_branches += recurse_euclidean_distance(tree1->back->next, tree1->back->next);
        sum_branches += recurse_euclidean_distance(tree1->back->next->next, tree2->back->next->next);
    }
    return sum_branches;
  }

  double TreePll::compute_euclidean_distance(pll_utree_t * tree1, pll_utree_t * tree2)
  {
    assert(!(tree1->next || tree2->next) && tree1->node_index == tree2->node_index);//!strcmp(tree1->label, tree2->label));

    double sum_branches = recurse_euclidean_distance(tree1, tree2);

    return sqrt(sum_branches);
  }

  void TreePll::cleanup( void )
  {
    if (pll_tree)
    {
      for (mt_index_t i=0; i<number_of_threads; i++)
      {
        if (pll_tree[i])
        {
          pll_utree_destroy(pll_tree[i], NULL);
        }
        if (pll_tip_nodes[i])
          free(pll_tip_nodes[i]);
        if (pll_inner_nodes[i])
          free(pll_inner_nodes[i]);
      }
      if(pll_start_tree) free(pll_start_tree);
      free(pll_tree);
      free(pll_tip_nodes);
      free(pll_inner_nodes);
    }
    pll_tree        = 0;
    pll_start_tree  = 0;
    pll_tip_nodes   = 0;
    pll_inner_nodes = 0;
  }

  void TreePll::clone_tree( pll_utree_t * tree )
  {
    pllmod_utree_traverse_apply(tree,
                                NULL,
                                NULL,
                                &cb_set_missing_branches,
                                NULL);

    for (mt_index_t i=0; i<number_of_threads; i++)
    {
      pll_tree[i] = pll_utree_clone (tree);
      pll_start_tree[i] = pll_tree[i];

      pll_tip_nodes[i] = (pll_utree_t **) Utils::c_allocate(n_tips, sizeof(pll_utree_t *));
      pll_utree_query_tipnodes(pll_tree[i], pll_tip_nodes[i]);
      pll_inner_nodes[i] = (pll_utree_t **) Utils::c_allocate(n_tips-2, sizeof(pll_utree_t *));
      pll_utree_query_innernodes(pll_tree[i], pll_inner_nodes[i]);
    }
  }
} /* namespace modeltest */
