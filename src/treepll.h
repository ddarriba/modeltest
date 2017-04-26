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

#ifndef TREEPLL_H_
#define TREEPLL_H_

#include "tree.h"
#include "plldefs.h"

namespace modeltest
{

  class TreePll : public Tree
  {
  public:
    TreePll (tree_type_t _type,
             std::string const& filename,
             Msa &_msa,
             data_type_t data_type,
             mt_size_t _number_of_threads = 1,
             int _random_seed = 12345);
    virtual ~TreePll ();

    virtual const std::string get_label(mt_index_t index, mt_index_t thread_number = 0) const;
    virtual bool set_branches(double length, mt_index_t thread_number = 0);
    virtual bool scale_branches(double factor, mt_index_t thread_number = 0);
    virtual bool reset_branches(mt_index_t thread_number = 0);
    virtual std::string newick(mt_index_t thread_number = 0) const;
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
     * @brief sets a particular pll tree
     * @param[in] thread_number the thread index
     */
    void set_pll_tree( pll_utree_t * new_tree, mt_index_t thread_number = 0);

    /**
     * @brief set missing taxa names into topology
     * @param[in,out] tree tree to update
     */
    void update_names( pll_utree_t * tree, Msa * msa = 0 );

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
    void cleanup( void );
    void clone_tree( pll_utree_t * tree );
    pll_utree_t **pll_tree;          //! pll tree structures for each thread
    pll_utree_t **pll_start_tree;    //! pll initial tree structures for each thread
  };

} /* namespace modeltest */

#endif /* TREEPLL_H_ */
