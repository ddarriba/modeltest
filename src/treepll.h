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
             int number_of_threads = 1,
             int random_seed = 12345);
    virtual ~TreePll ();

    pll_utree_t *get_pll_tree( int thread_number = 0) { return pll_tree[thread_number]; }
    virtual void print(int thread_number = 0);

    static bool test_tree(std::string const& tree_filename, int *n_tips);

  private:
    pll_utree_t **pll_tree;
  };

} /* namespace modeltest */

#endif /* TREEPLL_H_ */
