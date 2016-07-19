#include "abstract_parameter.h"

namespace modeltest
{
  double AbstractParameter::cb_compute_negative_loglikelihood(void *p)
  {
    mt_opt_params_t * params = (mt_opt_params_t *) p;
    double score = -1
          * pll_compute_edge_loglikelihood (
              params->partition,
              params->tree->clv_index,
              params->tree->scaler_index,
              params->tree->back->clv_index,
              params->tree->back->scaler_index,
              params->tree->pmatrix_index,
              params->params_indices,
              NULL);

    return score;
  }
}
