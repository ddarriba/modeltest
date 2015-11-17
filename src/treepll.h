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

    pll_utree_t *get_pll_start_tree( mt_index_t thread_number = 0)
    {
        return pll_start_tree[thread_number];
    }
    pll_utree_t *get_pll_tree( mt_index_t thread_number = 0)
    {
        return pll_tree[thread_number];
    }
    virtual const std::string get_label(mt_index_t index, mt_index_t thread_number = 0) const;

    virtual void reroot_random(mt_index_t thread_number = 0);
    virtual bool set_branches(double length, mt_index_t thread_number = 0);
    virtual bool reset_branches(mt_index_t thread_number = 0);

    virtual void print(mt_index_t thread_number = 0) const;
    static bool test_tree(std::string const& tree_filename, mt_size_t *n_tips);

  private:
    pll_utree_t **pll_tree;
    pll_utree_t **pll_start_tree;
    pll_utree_t ***pll_tip_nodes;
    pll_utree_t ***pll_inner_nodes;
  };

} /* namespace modeltest */

#endif /* TREEPLL_H_ */
