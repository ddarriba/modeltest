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
#include "genesis/logging.h"

#include <cerrno>
#include <iostream>

using namespace std;

static int cb_set_names(pll_unode_t * node, void *data)
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

static int cb_set_missing_names(pll_unode_t * node, void *data)
{
  pll_unode_t **tip_nodes = (pll_unode_t **) data;
  pll_unode_t **tip_nodes_ptr = tip_nodes;

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

namespace modeltest
{

  Tree::~Tree() {}

  static bool set_indices(pll_utree_t * pll_tree, Msa &msa)
  {
    mt_size_t n_tips = msa.get_n_sequences();
    pll_unode_t ** tip_nodes = pll_tree->nodes;
    bool * tips_found = new bool[pll_tree->tip_count] {false};
    bool return_val = true;

    if(n_tips != pll_tree->tip_count)
    {
      LOG_ERR << "Error: Number of tips in tree (" << n_tips << ") and MSA ("
              << pll_tree->tip_count << ") are different" << endl;
      return_val = false;
    }

    /* find sequences and link them with the corresponding taxa */
    for (mt_index_t i = 0; i < n_tips; ++i)
    {
        mt_index_t tip_clv_index = MT_SIZE_UNDEF;
        const char * header = msa.get_header (i);

        bool cur_tip_found = false;
        for (mt_index_t j = 0; j < pll_tree->tip_count; ++j)
        {
            if (!strcmp (tip_nodes[j]->label, header))
            {
                tips_found[j] = true;
                tip_nodes[j]->clv_index = tip_nodes[j]->node_index = tip_clv_index = i;

                if (cur_tip_found)
                {
                  LOG_ERR << "Error: Duplicated tip " << header << " in the tree" << endl;
                }
                cur_tip_found = true;
                //break;
            }
        }

        if (tip_clv_index == MT_SIZE_UNDEF)
        {
          LOG_ERR << "Error: Tip " << header << " not found in the tree" << endl;
          // LOG_ERR << "Error: Cannot find tip \"%s\" in the tree" << end;;
          return_val = false;
        }
    }
    for (mt_index_t j = 0; j < pll_tree->tip_count; ++j)
    {
      if (!tips_found[j])
      {
        return_val = false;
        LOG_ERR << "Error: Sequence " << tip_nodes[j]->label << " not found in the MSA" << endl;
      }
    }
    delete[] tips_found;

    if (!return_val)
    {
      mt_errno = MT_ERROR_IO_FORMAT;
      snprintf(mt_errmsg, 400,
               "Error: Tips in tree and MSA headers do not agree");
    }

    return return_val;
  }

  TreePll::TreePll (tree_type_t type,
                    string const& filename,
                    Msa &msa,
                    data_type_t datatype,
                    mt_size_t number_of_threads,
                    int random_seed)
    : Tree(type, filename, msa, number_of_threads, random_seed)
  {
    pll_utree_t * starting_tree = NULL;

    bl_optimized = false;
    pll_tree = (pll_utree_t **) Utils::c_allocate(number_of_threads, sizeof(pll_utree_t *));
    pll_start_tree = (pll_utree_t **) Utils::c_allocate(number_of_threads, sizeof(pll_utree_t *));

    if (ROOT)
    {
      if (filename.compare(""))
      {
        starting_tree = pll_utree_parse_newick (filename.c_str());
        if (!starting_tree)
        {
          /* save error from the unrooted tree parsing */
          snprintf(mt_errmsg, 400, "Error %d parsing user tree: %s", pll_errno, pll_errmsg);
          mt_errno = pll_errno;

          /* try rooted tree */
          pll_rtree_t * rtree = pll_rtree_parse_newick (filename.c_str());
          if (!rtree)
          {
            throw EXCEPTION_TREE_USER;
          }
          else
          {
            LOG_WARN << "WARNING: Input tree is a rooted topology. It will be unrooted" << endl;

            starting_tree = pll_rtree_unroot(rtree);
            pll_utree_reset_template_indices(starting_tree->nodes[0]->back, starting_tree->tip_count);
            pll_rtree_destroy(rtree, NULL);

          }
        }
        assert(starting_tree);
        pllmod_utree_set_length_recursive(starting_tree, 0.1, true);

        n_tips = starting_tree->tip_count;
      }
      else if (type == tree_random)
      {
        starting_tree = pllmod_utree_create_random(n_tips,
                                                   msa.get_headers(),
                                                   random_seed);
      }
      else
      {
        unsigned int * cost = new unsigned int[msa.get_n_patterns()];
        unsigned int states;
        const pll_state_t * map;
        if (datatype == dt_dna)
        {
          map = pll_map_nt;
          states = 4;
        }
        else if (datatype == dt_protein)
        {
          map = pll_map_aa;
          states = 20;
        }
        else
            assert(0);

        starting_tree = pllmod_utree_create_parsimony(n_tips,
                                                      msa.get_n_patterns(),
                                                      msa.get_headers(),
                                                      msa.get_sequences(),
                                                      msa.get_weights(),
                                                      map,
                                                      states,
                                                      0, /* attributes */
                                                      random_seed,
                                                      cost /* scores */
                                                    );
        delete[] cost;
      }

      if (!starting_tree)
      {
        cleanup();
        snprintf(mt_errmsg, 400, "Error %d creating starting tree: %s", pll_errno, pll_errmsg);
        throw EXCEPTION_TREE_MP;
      }
    }

    if (ROOT && !set_indices(starting_tree, msa))
    {
      cleanup();
      throw EXCEPTION_TREE_FORMAT;
    }

    #if (MPI_ENABLED)

    /* broadcast starting topology */
    pll_unode_t * serialized_tree;
    if (ROOT)
    {

      serialized_tree = pllmod_utree_serialize(starting_tree->nodes[0]->back,
                                               n_tips);
      if (!serialized_tree)
      {
        MPI_Abort(MPI_COMM_WORLD, MT_ERROR_IO);
      }

      MPI_Bcast(serialized_tree,
                sizeof(pll_unode_t) * (2*n_tips - 2),
                MPI_BYTE,
                0,
                master_mpi_comm );
    }
    else
    {
      serialized_tree = (pll_unode_t *) malloc(sizeof(pll_unode_t) * (2*n_tips - 2));
      MPI_Bcast(serialized_tree,
                sizeof(pll_unode_t) * (2*n_tips - 2),
                MPI_BYTE,
                0,
                master_mpi_comm );

      starting_tree = pllmod_utree_expand(serialized_tree, n_tips);

      update_names(starting_tree, &msa);
      free(serialized_tree);
    }
    #endif

    clone_tree( starting_tree );
    pll_utree_destroy(starting_tree, NULL);
  }

  TreePll::~TreePll ()
  {
    cleanup();
  }

  bool TreePll::set_branches(double length, mt_index_t thread_number)
  {
    assert(thread_number < number_of_threads);
    assert(length > 0);
    pllmod_utree_set_length_recursive(pll_tree[thread_number], length, false);

    return true;
  }

  bool TreePll::scale_branches(double factor, mt_index_t thread_number)
  {
    assert(factor > 0);
    assert(thread_number < number_of_threads);
    pllmod_utree_scale_branches(pll_tree[thread_number], factor);

    return true;
  }

  bool TreePll::reset_branches(mt_index_t thread_number)
  {
    assert(thread_number < number_of_threads);
    return false;
  }

  bool TreePll::test_tree(std::string const& tree_filename, mt_size_t *n_tips)
  {
    pll_utree_t * tree = pll_utree_parse_newick (tree_filename.c_str());
    if (!tree)
    {
      /* try rooted tree */
      pll_rtree_t * rtree = pll_rtree_parse_newick (tree_filename.c_str());
      if (rtree)
      {
        *n_tips = tree->tip_count;
        pll_rtree_destroy(rtree, NULL);
        return true;
      }
      *n_tips = 0;
      mt_errno = pll_errno;
      snprintf(mt_errmsg, 400, "PLL Error %d testing tree: %s", pll_errno, pll_errmsg);
      return false;
    }
    else
    {
      *n_tips = tree->tip_count;
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
        return pll_tree[thread_number]->nodes[index]->label;
  }

  string TreePll::newick(mt_index_t thread_number) const
  {
    assert(thread_number < number_of_threads);
    char *nw_cstr = pll_utree_export_newick(pll_tree[thread_number]->nodes[0]->back,
                                            NULL);
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
  }

  void TreePll::update_names( pll_utree_t * tree, Msa * msa )
  {
    if (msa)
    {
      pllmod_utree_traverse_apply(tree->nodes[0]->back,
                                  NULL,
                                  NULL,
                                  &cb_set_names,
                                  msa);
    }
    else
    {
      pllmod_utree_traverse_apply(tree->nodes[0]->back,
                                  NULL,
                                  NULL,
                                  &cb_set_missing_names,
                                  pll_start_tree[0]->nodes);
    }
  }

  void * TreePll::extract_tree ( mt_index_t thread_number) const
  {
    assert(thread_number < number_of_threads);
    pll_utree_t * new_tree = pll_utree_clone(pll_tree[thread_number]);
    return new_tree;
  }

  static double recurse_euclidean_distance(pll_unode_t * tree1, pll_unode_t * tree2)
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
    pll_unode_t *node1 = tree1->nodes[0],
                *node2 = tree2->nodes[0];

    assert(node1->node_index == node2->node_index);

    double sum_branches = recurse_euclidean_distance(node1, node2);

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
      }
      if(pll_start_tree) free(pll_start_tree);
      free(pll_tree);
    }
    pll_tree        = 0;
    pll_start_tree  = 0;
  }

  void TreePll::clone_tree( pll_utree_t * tree )
  {
    pllmod_utree_set_length_recursive(tree, 0.1, true);

    for (mt_index_t i=0; i<number_of_threads; i++)
    {
      pll_tree[i] = pll_utree_clone (tree);
      pll_start_tree[i] = pll_tree[i];
    }
  }
} /* namespace modeltest */
