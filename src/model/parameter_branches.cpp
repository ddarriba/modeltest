#include "parameter_branches.h"
#include "partition.h"

#define MIN_BL 1e-4
#define MAX_BL 5
#define SMOOTHINGS 4

using namespace std;

namespace modeltest
{

ParameterBranches::ParameterBranches( const ParameterBranches & other )
{
  n_branches = other.n_branches;
}

ParameterBranches::ParameterBranches( void )
{
  n_branches = 0;
}

ParameterBranches::~ParameterBranches( void )
{
}

bool ParameterBranches::initialize(mt_opt_params_t * params,
                                   Partition const& partition )
{
  UNUSED(params);
  UNUSED(partition);
  n_branches = partition.get_n_sequences()*2 - 3;
  return true;
}

double ParameterBranches::optimize(mt_opt_params_t * params,
                                double loglikelihood,
                                double tolerance,
                                bool first_guess)
{
  UNUSED(first_guess);
  double cur_logl;

  cur_logl = pll_optimize_branch_lengths_iterative (
             params->partition,
             params->tree,
             params->params_indices,
             MIN_BL, MAX_BL,
             tolerance, SMOOTHINGS, true);

  assert(!loglikelihood || (cur_logl - loglikelihood)/loglikelihood < 1e-10);

  return cur_logl;
}

void ParameterBranches::print(std::ostream  &out) const
{
  UNUSED(out);
}

mt_size_t ParameterBranches::get_n_free_parameters( void ) const
{
  return n_branches;
}

} /* namespace modeltest */
