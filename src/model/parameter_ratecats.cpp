#include "parameter_ratecats.h"

#define MIN_RATE 0.02
#define MAX_RATE 2

/* epsilon for Weights/Rates optimization */
#define WR_EPSILON 0.9

using namespace std;

namespace modeltest
{

ParameterRateCats::ParameterRateCats(mt_size_t n_cats)
  : n_cats(n_cats)
{
}

ParameterRateCats::ParameterRateCats( const ParameterRateCats & other )
{
  n_cats = other.n_cats;
}

ParameterRateCats::~ParameterRateCats( void )
{

}

bool ParameterRateCats::initialize(Partition const& partition)
{
  return true;
}

double ParameterRateCats::optimize(mt_opt_params_t * params,
                                double loglikelihood,
                                double tolerance,
                                bool first_guess)
{
  UNUSED(first_guess);
  double cur_logl;

  cur_logl = pllmod_algo_opt_rates_weights(params->partition,
                                           params->tree,
                                           params->params_indices,
                                           0.01, 10,
                                           tolerance);

  return cur_logl;

}

void ParameterRateCats::print(std::ostream  &out) const
{

}

mt_size_t ParameterRateCats::get_n_free_parameters( void ) const
{
  return (2*n_cats - 1);
}

} /* namespace modeltest */
