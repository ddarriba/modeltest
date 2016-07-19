#include "parameter_frequencies.h"

#define MIN_FREQ_RATE 0.02
#define MAX_FREQ_RATE 100

using namespace std;

namespace modeltest
{

struct custom_params {
  mt_opt_params_t * common_params;
  mt_index_t highest_freq_state;
};

static double target_func(void *p, double *x)
{
  struct custom_params * params = (struct custom_params *) p;
  mt_opt_params_t * mt_params = (mt_opt_params_t *) params->common_params;
  pll_partition_t * partition = mt_params->partition;
  mt_index_t highest_freq_state = params->highest_freq_state;
  mt_size_t states = partition->states;

  mt_index_t i;
  mt_index_t cur_index;

  double sum_ratios = 1.0;

  /* update frequencies */
  double *freqs = new double[states];

  for (i = 0; i < (states - 1); ++i)
  {
    assert(x[i] == x[i]);
    sum_ratios += x[i];
  }
  cur_index = 0;
  for (i = 0; i < states; ++i)
  {
    if (i != highest_freq_state)
    {
      freqs[i] = x[cur_index] / sum_ratios;
      cur_index++;
    }
  }
  freqs[highest_freq_state] = 1.0 / sum_ratios;
  pll_set_frequencies (partition,
                       0,
                       freqs);
  delete[] freqs;

  /* compute negative score */
  double score = -1 *
                 pll_utree_compute_lk(mt_params->partition,
                                      mt_params->tree,
                                      mt_params->params_indices,
                                      1,   /* update pmatrices */
                                      1);  /* update partials */
  return score;
}

ParameterFrequencies::ParameterFrequencies( void )
{
}

ParameterFrequencies::~ParameterFrequencies( void )
{

}

bool ParameterFrequencies::initialize(const partition_descriptor_t & partition_desc)
{

  return true;
}

double ParameterFrequencies::optimize(mt_opt_params_t * params,
                                double loglikelihood,
                                double tolerance,
                                bool first_guess)
{
  UNUSED(first_guess);
  double cur_logl;

  int bound_type[6] = {PLL_LBFGSB_BOUND_BOTH,PLL_LBFGSB_BOUND_BOTH,PLL_LBFGSB_BOUND_BOTH,
                       PLL_LBFGSB_BOUND_BOTH,PLL_LBFGSB_BOUND_BOTH,PLL_LBFGSB_BOUND_BOTH};

  struct custom_params opt_params;
  opt_params.common_params = params;

  unsigned int states = params->partition->states;
  unsigned int n_freqs_free_params = states - 1;
  unsigned int cur_index;
  mt_index_t params_index = 0;

  double * frequencies =
      params->partition->frequencies[params_index];

  /* find highest frequency */
  opt_params.highest_freq_state = 0;
  for (mt_index_t i = 1; i < states; i++)
          if (frequencies[i] > frequencies[opt_params.highest_freq_state])
            opt_params.highest_freq_state = i;

  cur_index = 0;
  for (mt_index_t i = 0; i < states; i++)
  {
    if (i != opt_params.highest_freq_state)
    {
      x[cur_index] = frequencies[i]
          / frequencies[opt_params.highest_freq_state];
      lower[cur_index] = PLL_OPT_MIN_FREQ;
      upper[cur_index] = PLL_OPT_MAX_FREQ;
      cur_index++;
    }
  }

  cur_logl = pll_minimize_lbfgsb(x, lower, upper, bound_type,
                                 n_freqs_free_params,
                                 1e7, tolerance,
                                 (void *) &opt_params,
                                 &target_func);

  /* update frequencies */
  target_func((void *)params, x);

  return cur_logl;
}

void ParameterFrequencies::print(std::ostream  &out)
{

}

} /* namespace modeltest */
