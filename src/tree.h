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
    Tree (tree_type _type,
          std::string const& filename,
          mt_size_t _number_of_threads = 1,
          int _random_seed = 12345)
    : type(_type),
      tree_file(filename),
      n_tips(0),
      number_of_threads(_number_of_threads),
      random_seed(_random_seed)
    {
    }

    virtual ~Tree ();

    mt_size_t get_n_tips( void ) const { return n_tips; }
    virtual const std::string get_label( mt_index_t index, mt_index_t thread_number = 0 ) const = 0;
    virtual void print( mt_index_t thread_number = 0 ) const = 0;
    bool is_bl_optimized( void ) { return bl_optimized; }
    void set_bl_optimized( void ) { bl_optimized = true; }
  protected:
    tree_type type;
    const std::string tree_file;
    mt_size_t n_tips;
    mt_size_t number_of_threads;
    int random_seed;

    bool bl_optimized;
  };

} /* namespace modeltest */

#endif /* TREE_H_ */
