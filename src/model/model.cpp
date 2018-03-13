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

#include "model.h"
#include "../genesis/logging.h"
#include "../utils.h"
#include "../model_defs.h"

#include <cmath>
#include <iomanip>
#include <sstream>
#include <cstring>
#include <cassert>
#include <fstream>
#include <algorithm>

using namespace std;

namespace modeltest {

// b(branches) f(freqs) g(gamma) i(pinv)
// r(ratecats) s(substrates)
static char parameter_order[N_PARAMETERS+1] = "bifgrs";

static bool sort_parameters(AbstractParameter * p1, AbstractParameter * p2)
{
    /* sort parameters */
  for (int i=0; i<N_PARAMETERS; ++i)
  {
    if (parameter_order[i] == p1->get_char_id())
      return 1;
    else if (parameter_order[i] == p2->get_char_id())
      return 0;
  }
  assert(0);
  return 0;
}

Model::Model(mt_mask_t model_params,
             const partition_descriptor_t &partition,
             mt_size_t states,
             mt_size_t _n_categories,
             asc_bias_t asc_bias_corr)
    : model_params(model_params),
      optimize_pinv(model_params & (MOD_PARAM_INV | MOD_PARAM_INV_GAMMA)),
      optimize_gamma((model_params & (MOD_PARAM_GAMMA | MOD_PARAM_INV_GAMMA))
                     && !(model_params & MOD_PARAM_FREE_RATES)),
      optimize_freqs(model_params & MOD_PARAM_ESTIMATED_FREQ),
      empirical_freqs(model_params & MOD_PARAM_EMPIRICAL_FREQ),
      optimize_ratecats(model_params & MOD_PARAM_FREE_RATES),
      mixture(model_params & MOD_PARAM_MIXTURE),
      gap_aware(partition.gap_aware),
      asc_bias_corr(asc_bias_corr)
{
  restored_from_ckp = false;
  matrix_index = (mt_mask_t) 0;
  current_opt_parameter = 0;
  unique_id = 0;

  loglh = 0.0;
  bic = 0.0;
  aic = 0.0;
  aicc = 0.0;
  dt = 0.0;

  exec_time = 0;

  params_indices = 0;

  n_free_variables = 0;
  n_frequencies = 0;
  n_subst_rates = 0;
  n_categories = (optimize_gamma | optimize_ratecats)
      ?_n_categories
      :1;

  tree = 0;
  n_tips = 0;

  asc_weights = 0;

  param_gamma = 0;
  param_pinv = 0;
  param_branches = 0;
  param_freqs = 0;
  param_substrates = 0;

  if (mixture && n_categories != N_MIXTURE_CATS)
  {
    LOG_WARN << "Warning: Categories for mixture model set to " << N_MIXTURE_CATS << endl;
    n_categories = N_MIXTURE_CATS;
  }

  param_branches = new ParameterBranches();
  parameters.push_back(param_branches);

  param_gamma = 0;

  if (optimize_pinv)
  {
    param_pinv = new ParameterPinv();
    parameters.push_back(param_pinv);
  }

  if (optimize_freqs)
  {
    param_freqs = new ParameterFrequenciesOpt(states);
    parameters.push_back(param_freqs);
  }

  assert(optimize_gamma || optimize_ratecats || n_categories == 1);

  if (optimize_ratecats)
    param_gamma = new ParameterRateCats(n_categories);
  else
    param_gamma = new ParameterGamma(n_categories);

  parameters.push_back(param_gamma);

  params_indices = (unsigned int *) Utils::c_allocate(
                                      n_categories, sizeof(unsigned int));

  if (mixture)
  {
    assert(n_categories == N_MIXTURE_CATS);
    for (mt_index_t i = 0; i < N_MIXTURE_CATS; ++i)
      params_indices[i] = i;
  }
}

Model::Model( void )
    : optimize_pinv(false),
      optimize_gamma(false),
      optimize_freqs(false),
      empirical_freqs(false),
      optimize_ratecats(false),
      mixture(false),
      gap_aware(false),
      asc_bias_corr(asc_none)
{
  restored_from_ckp = false;
  model_params = (mt_mask_t) 0;
  matrix_index = 0;
  current_opt_parameter = 0;
  unique_id = 0;

  loglh  = 0.0;
  bic  = 0.0;
  aic  = 0.0;
  aicc = 0.0;
  dt   = 0.0;

  exec_time = 0;

  n_categories = 0;
  params_indices = 0;

  n_free_variables = 0;
  n_frequencies = 0;
  n_subst_rates = 0;

  tree = 0;
  n_tips = 0;

  asc_weights = 0;

  param_gamma = 0;
  param_pinv = 0;
  param_branches = 0;
  param_freqs = 0;
  param_substrates = 0;
}

Model::~Model()
{
  if (asc_weights)
    delete[] asc_weights;
  if (params_indices)
    free(params_indices);
  if (tree)
  {
    pll_utree_destroy(tree, NULL);
  }
  for (AbstractParameter * parameter : parameters)
    delete parameter;
}

mt_index_t Model::get_matrix_index() const
{
  return matrix_index;
}

std::string const& Model::get_name() const
{
  return name;
}

bool Model::is_optimized() const
{
  return loglh < 0.0;
}

mt_mask_t Model::get_model_params( void ) const
{
  return model_params;
}

bool Model::is_I() const
{
  return optimize_pinv;
}

bool Model::is_G() const
{
  return optimize_gamma;
}

bool Model::is_F() const
{
  return optimize_freqs || empirical_freqs;
}

bool Model::is_mixture( void ) const
{
  return mixture;
}

bool Model::is_gap_aware( void ) const
{
  return gap_aware;
}

const int * Model::get_symmetries( void ) const
{
  return 0;
}

mt_size_t Model::get_n_categories() const
{
  return n_categories;
}

mt_size_t Model::get_n_free_variables() const
{
  return n_free_variables;
}

double Model::get_loglh() const
{
  return loglh;
}

void Model::set_loglh( double l )
{
  loglh = l;
}

mt_size_t Model::get_n_subst_params() const
{
  assert(param_substrates);
  return param_substrates->get_n_free_parameters();
}

double Model::get_prop_inv() const
{
  if (!param_pinv)
    return 0.0;
  return param_pinv->get_pinv();
}

void Model::set_prop_inv(double value)
{
  assert(param_pinv);
  param_pinv->set_pinv(value);
}

double Model::get_alpha() const
{
  assert(param_gamma);
  return param_gamma->get_alpha();
}

void Model::set_alpha(double value)
{
  assert(param_gamma);
  param_gamma->set_alpha(value);
}

const mt_size_t * Model::get_asc_weights( void ) const
{
  return asc_weights;
}

asc_bias_t Model::get_asc_bias_corr( void ) const
{
  return asc_bias_corr;
}

mt_size_t Model::get_n_states( void ) const
{
  return n_frequencies;
}

mt_size_t Model::get_n_subst_rates( void ) const
{
  return n_subst_rates;
}

const double * Model::get_frequencies( mt_index_t freqs_idx ) const
{
  return param_freqs->get_frequencies(freqs_idx);
}

const unsigned int * Model::get_params_indices( void ) const
{
  assert(params_indices);
  return params_indices;
}

void Model::set_frequencies(const double value[])
{
  assert(param_freqs);
  param_freqs->set_frequencies(value);
}

void Model::set_frequencies(const vector<double> & value)
{
  assert(param_freqs);
  param_freqs->set_frequencies(value);
}

const double * Model::get_subst_rates( mt_index_t matrix_idx ) const
{
  assert(param_substrates);
  return param_substrates->get_subst_rates(matrix_idx);
}

void Model::set_subst_rates(const double value[], mt_index_t matrix_idx)
{
  assert(param_substrates);
  param_substrates->set_subst_rates(value, matrix_idx);
}

const double * Model::get_mixture_weights( void ) const
{
  return 0;
}

void Model::set_mixture_weights( const double * weights )
{
  /* this applies only to mixture models */
  UNUSED(weights);
  assert(0);
}

const double * Model::get_mixture_rates( void ) const
{
  return 0;
}

void Model::set_mixture_rates( const double * rates )
{
  /* this applies only to mixture models */
  UNUSED(rates);
  assert(0);
}

bool Model::evaluate_criteria (mt_size_t n_branches_params,
                                double sample_size )
{
  if (!is_optimized())
      return false;

  mt_size_t n_params = n_free_variables + n_branches_params;

  aic = 2*n_params - 2*loglh;
  aicc = aic + 2*n_params*(n_params+1)/(sample_size - n_params - 1);
  bic = -2*loglh + n_params * log(sample_size);

  return true;
}

double Model::get_bic() const
{
  return bic;
}

void Model::set_bic(double value)
{
  assert(is_optimized());
  bic = value;
}

double Model::get_aic() const
{
  return aic;
}

void Model::set_aic(double value)
{
  assert(is_optimized());
  aic = value;
}

double Model::get_aicc() const
{
  return aicc;
}

void Model::set_aicc(double value)
{
  assert(is_optimized());
  aicc = value;
}

double Model::get_dt() const
{
  return dt;
}

void Model::set_dt(double value)
{
  assert(is_optimized());
  dt = value;
}

time_t Model::get_exec_time() const
{
  return exec_time;
}

void Model::set_exec_time( time_t t)
{
    exec_time = t;
}

pll_utree_t * Model::get_tree( void ) const
{
    assert(is_optimized());
    return tree;
}

pll_unode_t * Model::get_tree_graph( void ) const
{
  assert(is_optimized());
  return tree->nodes[0];
}

void Model::set_tree( pll_unode_t * _tree, int _n_tips )
{
  /* set node zero */
  while (_tree->node_index > 0)
    if (_tree->next)
      _tree = _tree->next->back;
    else
     _tree = _tree->back;

  assert(n_tips || n_tips > 0);

  if (_n_tips > 0)
    n_tips = _n_tips;

  assert(!_tree->next && _tree->back->next);

  if (tree)
  {
    pll_utree_destroy(tree, NULL);
  }

  tree = pll_utree_wraptree(_tree->back, n_tips);
}

void Model::set_tree( pll_utree_t * _tree )
{
  if (n_tips > 0)
    assert(n_tips == _tree->tip_count);
  else
    n_tips = _tree->tip_count;

  tree = _tree;
}

mt_index_t Model::get_unique_id( void ) const
{
  return unique_id;
}

bool Model::optimize_init ( pll_partition_t * pll_partition,
                            pllmod_treeinfo_t * tree_info,
                            Partition const& partition )
{
  assert(pll_partition);
  mt_opt_params_t params;
  params.partition = pll_partition;
  params.tree_info = tree_info;
  params.params_indices = params_indices;

  /* reorder parameters */
  sort(parameters.begin(),
       parameters.end(),
       &sort_parameters);

  bool result = true;
  for (AbstractParameter * parameter : parameters)
    result &= parameter->initialize(&params, partition);
  current_opt_parameter = 0;
  return result;
}

bool Model::optimize( pll_partition_t * partition,
                      pllmod_treeinfo_t * tree_info,
                      double tolerance )
{
    mt_opt_params_t params;
    params.partition = partition;
    params.tree_info = tree_info;
    params.params_indices = params_indices;

    if (optimize_gamma)
    {
      if (optimize_pinv && alpha_inv_guess > 0)
        param_gamma->set_alpha(alpha_inv_guess);
      else if (alpha_guess > 0)
        param_gamma->set_alpha(alpha_guess);
    }

    for (AbstractParameter * parameter : parameters)
      parameter->optimize(&params, loglh, tolerance, true);
    //lnL = parameters[0]->optimize(&params, lnL, tolerance, true);

    if (optimize_gamma)
    {
      // alpha = param_gamma->get_alpha();
      if (optimize_pinv)
        alpha_inv_guess = param_gamma->get_alpha();
      else
        alpha_guess = param_gamma->get_alpha();
    }

    return true;
}

bool Model::optimize_oneparameter( pll_partition_t * partition,
                                   pllmod_treeinfo_t * tree_info,
                                   double tolerance )
{
  assert(partition);
  assert(tree_info);
  assert(tree_info->root);

  assert(current_opt_parameter < parameters.size());
  mt_opt_params_t params;
  params.partition = partition;
  params.tree_info = tree_info;
  params.params_indices = params_indices;

  AbstractParameter * parameter = parameters[current_opt_parameter];

  loglh = parameter->optimize(&params, loglh, tolerance, true);
  LOG_DBG << "[dbg] optimize " << parameter->get_name() << ": " << loglh << endl;
  ++current_opt_parameter;

  if (current_opt_parameter >= parameters.size())
  {
    current_opt_parameter = 0;
    return true;
  }
  else
  {
    return false;
  }
}

void Model::print_inline(int index,
                         int n_models,
                         time_t ini_time,
                         time_t global_ini_time,
                         ostream &out)
{
  out << fixed << setprecision(MT_PRECISION_DIGITS);
  out << setw(5) << right << index << "/"
      << setw(5) << left << n_models
      << setw(15) << left << get_name()
      << setw(11) << Utils::format_time(time(NULL) - ini_time);
  if (global_ini_time)
      out << setw(11) << Utils::format_time(time(NULL) - global_ini_time);
  out << setw(18) << right << fixed
      << get_loglh();
  if (is_G())
    out << setw(8) << get_alpha();
  else
    out << setw(8) << "-";
  if (is_I())
    out << setw(8) << get_prop_inv();
  else
    out << setw(8) << "-";
  out << endl;
}

mt_mask_t Model::asc_bias_attribute(asc_bias_t v)
{
  mt_mask_t attr = 0;
  if (v != asc_none)
  {
    attr = PLL_ATTRIB_AB_FLAG;
    if (v == asc_lewis)
      attr |= PLL_ATTRIB_AB_LEWIS;
    else if (v == asc_felsenstein)
      attr |= PLL_ATTRIB_AB_FELSENSTEIN;
    else if (v == asc_stamatakis)
      attr |= PLL_ATTRIB_AB_STAMATAKIS;
  }
  return attr;
}

} /* namespace */
