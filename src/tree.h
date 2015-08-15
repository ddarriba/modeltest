/*
 * tree.h
 *
 *  Created on: Jun 9, 2015
 *      Author: diego
 */

#ifndef TREE_H_
#define TREE_H_

#include "global_defs.h"
#include <string>

namespace modeltest
{

  class Tree
  {
  public:
    Tree (tree_type type,
          std::string const& filename,
          mt_size_t number_of_threads = 1,
          int random_seed = 12345)
    : type(type),
      tree_file(filename),
      n_tips(0),
      number_of_threads(number_of_threads),
      random_seed(random_seed)
    {
    }

    virtual ~Tree ()
    {
    }

    mt_size_t get_n_tips( void ) const { return n_tips; }
    virtual void print(mt_index_t thread_number = 0) = 0;

  protected:
    tree_type type;
    const std::string tree_file;
    mt_size_t n_tips;
    mt_size_t number_of_threads;
    int random_seed;
  };

} /* namespace modeltest */

#endif /* TREE_H_ */
