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

namespace modeltest
{

  Tree::~Tree() {}

  TreePll::TreePll (tree_type type,
                    std::string const& filename,
                    mt_size_t number_of_threads,
                    int random_seed)
      : Tree(type, filename, number_of_threads, random_seed)
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
          for (mt_index_t i=0; i<number_of_threads; i++)
          {
              /*TODO: copy this for other tree types or move it outside */
              pll_tree[i] = pll_utree_parse_newick (filename.c_str(), &(n_tips));
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
                  cout << "Error " << pll_errno << " reading tree: " << pll_errmsg << endl;
                  free (pll_tree);
                  free (pll_start_tree);
                  free (pll_tip_nodes);
                  free (pll_inner_nodes);
                  pll_tree = 0;
                  pll_start_tree = 0;
                  pll_tip_nodes = 0;
                  pll_inner_nodes = 0;
                  errno = pll_errno;
                  return;
              }
          }
      }
          break;
      case tree_mp:
      {
          /* Temporary call to RAxML */
          cout << "Testing MP starting tree!" << endl;
          string mp_tree_filename = "mtTempMpTree";
          char command[500];
          sprintf(command, "scripts/makeParsimony.sh %s %s %d", filename.c_str(), mp_tree_filename.c_str(), random_seed);
          int retval = system(command);
          if (retval)
          {
              cout << "ERROR: Command failed: " << command << endl;
          }
          for (mt_index_t i=0; i<number_of_threads; i++)
          {
              pll_tree[i] = pll_utree_parse_newick (mp_tree_filename.c_str(), &(n_tips));
              pll_start_tree[i] = pll_tree[i];
              if (!pll_tree[i])
              {
                  cout << "Error " << pll_errno << " reading tree: " << pll_errmsg << endl;
                  errno = pll_errno;
                  return;
              }
          }
      }
          break;
      case tree_ml_gtr_fixed:
      {
          /* Temporary call to RAxML */
          cout << "Testing ML GTR starting tree!" << endl;
          string mp_tree_filename = "mtTempMpTree";
          char command[500];
          sprintf(command, "scripts/makeGtrML.sh %s %s %d", filename.c_str(), mp_tree_filename.c_str(), random_seed);
          int retval = system(command);
          if (retval)
          {
              cout << "ERROR: Command failed: " << command << endl;
          }
          for (mt_index_t i=0; i<number_of_threads; i++)
          {
              pll_tree[i] = pll_utree_parse_newick (mp_tree_filename.c_str(), &(n_tips));
              pll_start_tree[i] = pll_tree[i];
              if (!pll_tree[i])
              {
                  cout << "Error " << pll_errno << " reading tree: " << pll_errmsg << endl;
                  errno = pll_errno;
                  return;
              }
          }
      }
          break;
      case tree_ml_jc_fixed:
      {
          /* Temporary call to RAxML */
          cout << "Testing ML GTR starting tree!" << endl;
          string mp_tree_filename = "mtTempMpTree";
          char command[500];
          sprintf(command, "scripts/makeJcML.sh %s %s %d", filename.c_str(), mp_tree_filename.c_str(), random_seed);
          int retval = system(command);
          if (retval)
          {
              cout << "ERROR: Command failed: " << command << endl;
          }
          for (mt_index_t i=0; i<number_of_threads; i++)
          {
              pll_tree[i] = pll_utree_parse_newick (mp_tree_filename.c_str(), &(n_tips));
              pll_start_tree[i] = pll_tree[i];
              if (!pll_tree[i])
              {
                  cout << "Error " << pll_errno << " reading tree: " << pll_errmsg << endl;
                  errno = pll_errno;
                  return;
              }
          }
      }
          break;
      case tree_ml:
          /* unimplemented */
          cout << "Unimplemented" << endl;
          assert(0);
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
      assert(pll_inner_nodes);
      /* move to random node */
      int inner_index = rand () % n_inner;
//      int tip_index = rand () % n_tips;

      pll_tree[thread_number] = pll_inner_nodes[thread_number][inner_index];
//      pll_tree[thread_number] = pll_tip_nodes[thread_number][inner_index];
      assert(pll_tree[thread_number]);
  }

  bool TreePll::set_branches(double length, mt_index_t thread_number)
  {
      set_branch_length(pll_tree[thread_number], length, true);
      return true;
  }

  bool TreePll::reset_branches(mt_index_t thread_number)
  {
    return false;
  }

  bool TreePll::test_tree(std::string const& tree_filename, mt_size_t *n_tips)
  {
        pll_utree_t * tree = pll_utree_parse_newick (tree_filename.c_str(), n_tips);
        if (!tree)
        {
                errno = pll_errno;
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
      if (index >= n_tips)
          return "";
      else
          return pll_tip_nodes[thread_number][index]->label;
  }

  void TreePll::print(mt_index_t thread_number) const
  {
      char *newick = pll_utree_export_newick(pll_tree[thread_number]);
      cout << newick << endl;
      free (newick);
  }

} /* namespace modeltest */
