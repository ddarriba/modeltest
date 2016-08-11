/*
 * treepll.cpp
 *
 *  Created on: Jun 10, 2015
 *      Author: diego
 */

#include "treepll.h"
#include "utils.h"

#include <iostream>
#include <cerrno>

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

static void set_branch_length_recursive (pll_utree_t * tree,
                                         double length,
                                         bool reset)
{
  if (tree)
  {
    /* set branch length to default if not set */
    if (tree->length < DOUBLE_EPSILON || reset)
      tree->length = length;

    if (tree->next)
    {
      if (tree->next->length < DOUBLE_EPSILON || reset)
        tree->next->length = length;

      if (tree->next->next->length < DOUBLE_EPSILON || reset)
        tree->next->next->length = length;

      set_branch_length_recursive (tree->next->back, length, reset);
      set_branch_length_recursive (tree->next->next->back, length, reset);
    }
  }
}

/* branch lengths not present in the newick file get a value of 0.000001 */
static void set_branch_length (pll_utree_t * tree,
                               double length,
                               bool reset)
{
  set_branch_length_recursive (tree, length, reset);
  set_branch_length_recursive (tree->back, length, reset);
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

  TreePll::TreePll (tree_type_t type,
                    Msa &msa,
                    mt_size_t number_of_threads,
                    int random_seed)
    : Tree(type, msa, number_of_threads, random_seed)
  {
      bl_optimized = false;
      pll_tree = (pll_utree_t **) Utils::allocate(number_of_threads, sizeof(pll_utree_t *));
      pll_start_tree = (pll_utree_t **) Utils::allocate(number_of_threads, sizeof(pll_utree_t *));
      pll_tip_nodes = (pll_utree_t ***) Utils::c_allocate(number_of_threads, sizeof(pll_utree_t **));
      pll_inner_nodes = (pll_utree_t ***) Utils::c_allocate(number_of_threads, sizeof(pll_utree_t **));

      switch(type)
      {
      case tree_user_fixed:
      {
          assert(0);
      }
      case tree_random:
      {
          pll_utree_t * random_tree = pllmod_utree_create_random(n_tips, msa.get_headers());
          pllmod_utree_traverse_apply(random_tree,
                                   NULL,
                                   &cb_set_missing_branches,
                                   NULL);
          for (mt_index_t i=0; i<number_of_threads; i++)
          {
              pll_tree[i] = pll_utree_clone(random_tree);
              pll_start_tree[i] = pll_tree[i];
              if (pll_tree[i])
              {
                  pll_tip_nodes[i] = (pll_utree_t **) Utils::c_allocate(n_tips, sizeof(pll_utree_t *));
                  pll_utree_query_tipnodes(pll_tree[i], pll_tip_nodes[i]);
                  pll_inner_nodes[i] = (pll_utree_t **) Utils::c_allocate(n_tips-2, sizeof(pll_utree_t *));
                  pll_utree_query_innernodes(pll_tree[i], pll_inner_nodes[i]);
              }
              else
              {
                  free (pll_tree);
                  free (pll_start_tree);
                  free (pll_tip_nodes);
                  free (pll_inner_nodes);
                  pll_tree = 0;
                  pll_start_tree = 0;
                  pll_tip_nodes = 0;
                  pll_inner_nodes = 0;
                  mt_errno = pll_errno;
                  snprintf(mt_errmsg, 400, "Error %d creating random tree: %s", pll_errno, pll_errmsg);
                  throw EXCEPTION_TREE_MP;
              }
          }
      }
          break;
      case tree_mp:
      case tree_ml_gtr_fixed:
      case tree_ml_jc_fixed:
      case tree_ml:
      {
          /*TODO: WARNING: Temporary use a RANDOM tree */
          cout << "[****WARNING****] Constructing random starting tree! (temporary)" << endl;
          pll_utree_t * random_tree = pllmod_utree_create_random(n_tips, msa.get_headers());
          pllmod_utree_traverse_apply(random_tree,
                                   NULL,
                                   &cb_set_missing_branches,
                                   NULL);
          for (mt_index_t i=0; i<number_of_threads; i++)
          {
              pll_tree[i] = pll_utree_clone(random_tree);
              pll_start_tree[i] = pll_tree[i];
              if (pll_tree[i])
              {
                  pll_tip_nodes[i] = (pll_utree_t **) Utils::c_allocate(n_tips, sizeof(pll_utree_t *));
                  pll_utree_query_tipnodes(pll_tree[i], pll_tip_nodes[i]);
                  pll_inner_nodes[i] = (pll_utree_t **) Utils::c_allocate(n_tips-2, sizeof(pll_utree_t *));
                  pll_utree_query_innernodes(pll_tree[i], pll_inner_nodes[i]);
              }
              else
              {
                  free (pll_tree);
                  free (pll_start_tree);
                  free (pll_tip_nodes);
                  free (pll_inner_nodes);
                  pll_tree = 0;
                  pll_start_tree = 0;
                  pll_tip_nodes = 0;
                  pll_inner_nodes = 0;
                  mt_errno = pll_errno;
                  snprintf(mt_errmsg, 400, "Error %d creating random tree: %s", pll_errno, pll_errmsg);
                  throw EXCEPTION_TREE_MP;
              }
          }
          pll_utree_destroy(random_tree);
      }
          break;
      }

      n_inner = n_tips-2;
  }

  TreePll::TreePll (tree_type_t type,
                    std::string const& filename,
                    mt_size_t number_of_threads,
                    int random_seed)
      : Tree(type, filename, number_of_threads, random_seed)
  {
      assert(type == tree_user_fixed);

      bl_optimized = false;
      pll_tree = (pll_utree_t **) Utils::allocate(number_of_threads, sizeof(pll_utree_t *));
      pll_start_tree = (pll_utree_t **) Utils::allocate(number_of_threads, sizeof(pll_utree_t *));
      pll_tip_nodes = (pll_utree_t ***) Utils::c_allocate(number_of_threads, sizeof(pll_utree_t **));
      pll_inner_nodes = (pll_utree_t ***) Utils::c_allocate(number_of_threads, sizeof(pll_utree_t **));

      switch(type)
      {
          case tree_user_fixed:
            {
              for (mt_index_t i=0; i<number_of_threads; i++)
              {
                  /*TODO: copy this for other tree types or move it outside */
                  pll_tree[i] = pll_utree_parse_newick (filename.c_str(), &(n_tips));
                  pllmod_utree_traverse_apply(pll_tree[i],
                                           NULL,
                                           &cb_set_missing_branches,
                                           NULL);
                  pll_start_tree[i] = pll_tree[i];
                  if (pll_tree[i])
                  {
                      pll_tip_nodes[i] = (pll_utree_t **) Utils::c_allocate(n_tips, sizeof(pll_utree_t *));
                      pll_utree_query_tipnodes(pll_tree[i], pll_tip_nodes[i]);
                      pll_inner_nodes[i] = (pll_utree_t **) Utils::c_allocate(n_tips-2, sizeof(pll_utree_t *));
                      pll_utree_query_innernodes(pll_tree[i], pll_inner_nodes[i]);
                  }
                  else
                  {
                      free (pll_tree);
                      free (pll_start_tree);
                      free (pll_tip_nodes);
                      free (pll_inner_nodes);
                      pll_tree = 0;
                      pll_start_tree = 0;
                      pll_tip_nodes = 0;
                      pll_inner_nodes = 0;
                      mt_errno = pll_errno;
                      snprintf(mt_errmsg, 400, "PLL Error %d parsing user tree: %s", pll_errno, pll_errmsg);
                      throw EXCEPTION_TREE_USER;
                  }
              }
            }
            break;
          case tree_mp:
          case tree_ml_gtr_fixed:
          case tree_ml_jc_fixed:
          case tree_ml:
          case tree_random:
            {
                assert(0);
            }
            break;
      }

      n_inner = n_tips-2;
      /* fix all missing branch lengths to 0.00001 */
      for (mt_index_t i=0; i<number_of_threads; i++)
      {
          set_branch_length (pll_tree[i], 0.00001, false);
      }
  }

  TreePll::~TreePll ()
  {
    if(pll_start_tree) free(pll_start_tree);
    if (pll_tree)
    {
      for (mt_index_t i=0; i<number_of_threads; i++)
      {
        if (pll_tree[i])
          pll_utree_destroy(pll_tree[i]);
        if (pll_tip_nodes[i])
          free(pll_tip_nodes[i]);
        if (pll_inner_nodes[i])
          free(pll_inner_nodes[i]);
      }
      free( pll_tree );
      free(pll_tip_nodes);
      free(pll_inner_nodes);
    }
  }

  void TreePll::reroot_random(mt_index_t thread_number)
  {
      if (!pll_inner_nodes)
          cerr << "INER NODES FAIL" << endl;
      assert(pll_inner_nodes);
      /* move to random node */
      int inner_index = rand () % (int) n_inner;
//      int tip_index = rand () % n_tips;

      if (!pll_inner_nodes[thread_number])
          cerr << "INER NODES THREAD FAIL " << thread_number << " " << number_of_threads << endl;
      pll_tree[thread_number] = pll_inner_nodes[thread_number][inner_index];
//      pll_tree[thread_number] = pll_tip_nodes[thread_number][inner_index];
      assert(pll_tree[thread_number]);
  }

  bool TreePll::set_branches(double length, mt_index_t thread_number)
  {
      assert(thread_number < number_of_threads);
      assert(length > 0);
      set_branch_length(pll_tree[thread_number], length, true);
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
            pll_utree_destroy(tree);
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

  void * TreePll::extract_tree ( mt_index_t thread_number) const
  {
      assert(thread_number < number_of_threads);
      pll_utree_t * new_tree = pll_utree_clone(pll_tip_nodes[thread_number][0]);
      return new_tree;
  }

  static double recurse_euclidean_distance(pll_utree_t * tree1, pll_utree_t * tree2)
  {
      double sum_branches = (tree1->length - tree2->length) * (tree1->length - tree2->length);

      if (tree1->back->next)
      {
          sum_branches += recurse_euclidean_distance(tree1->back->next, tree1->back->next);
          sum_branches += recurse_euclidean_distance(tree1->back->next->next, tree2->back->next->next);
      }
      return sum_branches;
  }

  double TreePll::compute_euclidean_distance(pll_utree_t * tree1, pll_utree_t * tree2)
  {
      assert(!(tree1->next || tree2->next) && !strcmp(tree1->label, tree2->label));

      double sum_branches = recurse_euclidean_distance(tree1, tree2);

      return sqrt(sum_branches);
  }
} /* namespace modeltest */
