#ifndef ABSTRACT_PARAMETER_H
#define ABSTRACT_PARAMETER_H

#include "global_defs.h"
#include "plldefs.h"
#include <iostream>

namespace modeltest
{

class Partition;

typedef struct
{
    pll_partition_t * partition;
    pll_utree_t * tree;
    unsigned int * params_indices;
} mt_opt_params_t;

class AbstractParameter
{
public:
  virtual ~AbstractParameter() {}
  virtual bool initialize(mt_opt_params_t * params,
                          Partition const& partition) = 0;
  virtual double optimize(mt_opt_params_t * params,
                          double loglikelihood,
                          double tolerance = DEFAULT_PARAM_EPSILON,
                          bool first_guess = false) = 0;
  virtual void print(std::ostream  &out = std::cout) const = 0;
  virtual mt_size_t get_n_free_parameters( void ) const = 0;
protected:
  static double cb_compute_negative_loglikelihood(void *p);
};

} /* namespace modeltest */
#endif
