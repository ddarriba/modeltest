/*
 * treepll.h
 *
 *  Created on: Jun 10, 2015
 *      Author: diego
 */

#ifndef TREEPLL_H_
#define TREEPLL_H_

#include "tree.h"
#include "plldefs.h"

namespace modeltest
{

  class TreePll : public Tree
  {
  public:
    TreePll (tree_type type,
             std::string const& filename,
             mt_size_t number_of_threads = 1,
             int random_seed = 12345);
    virtual ~TreePll ();

    virtual const std::string get_label(mt_index_t index, mt_index_t thread_number = 0) const;
    virtual void reroot_random(mt_index_t thread_number = 0);
    virtual bool set_branches(double length, mt_index_t thread_number = 0);
    virtual bool reset_branches(mt_index_t thread_number = 0);
    virtual void print(mt_index_t thread_number = 0) const;
    virtual void * extract_tree ( mt_index_t thread_number = 0 ) const;

    /**
     * @brief get the starting tree pointer (before doing any rearrangement)
     * @param[in] thread_number the thread index
     * @return the starting pll unrooted tree
     */
    pll_utree_t *get_pll_start_tree( mt_index_t thread_number = 0)
    {
        return pll_start_tree[thread_number];
    }

    /**
     * @brief get a particular pll tree
     * @param[in] thread_number the thread index
     * @return the pll unrooted tree structure
     */
    pll_utree_t *get_pll_tree( mt_index_t thread_number = 0)
    {
        return pll_tree[thread_number];
    }

    /**
     * @brief test whether a tree is correct
     * @param[in] tree_filename the filename of the tree in NEWICK format
     * @param[out] n_tips the number of tips
     * @return true, if the topology is correect
     */
    static bool test_tree(std::string const& tree_filename, mt_size_t *n_tips);

    /**
     * @brief compute euclidean distance between two identical topologies
     *
     * The topologies of tree1 and tree2 must be the same. Additionally, both trees must point
     * to the same TIP node.
     *
     * @param[in] tree1 first pll unrooted tree
     * @param[in] tree2 second pll unrooted tree
     * @return the euclidean distance
     */
    static double compute_euclidean_distance(pll_utree_t * tree1, pll_utree_t * tree2);
  private:
    pll_utree_t **pll_tree;          //! pll tree structures for each thread
    pll_utree_t **pll_start_tree;    //! pll initial tree structures for each thread
    pll_utree_t ***pll_tip_nodes;    //! list of tip node pointers for each thread
    pll_utree_t ***pll_inner_nodes;  //! list of inner node pointers for each thread
  };

} /* namespace modeltest */

#endif /* TREEPLL_H_ */
