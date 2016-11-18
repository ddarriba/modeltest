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

#include "model/model.h"
#include "utils.h"

#include <cmath>
#include <iomanip>
#include <sstream>
#include <cstring>
#include <cassert>
#include <fstream>
#include <algorithm>

const double * prot_model_rates[N_PROT_MODEL_MATRICES] = {
   pll_aa_rates_dayhoff,
   pll_aa_rates_lg,
   pll_aa_rates_dcmut,
   pll_aa_rates_jtt,
   pll_aa_rates_mtrev,
   pll_aa_rates_wag,
   pll_aa_rates_rtrev,
   pll_aa_rates_cprev,
   pll_aa_rates_vt,
   pll_aa_rates_blosum62,
   pll_aa_rates_mtmam,
   pll_aa_rates_mtart,
   pll_aa_rates_mtzoa,
   pll_aa_rates_pmb,
   pll_aa_rates_hivb,
   pll_aa_rates_hivw,
   pll_aa_rates_jttdcmut,
   pll_aa_rates_flu,
   pll_aa_rates_stmtrev
 };

const double * prot_model_freqs[N_PROT_MODEL_MATRICES] = {
   pll_aa_freqs_dayhoff,
   pll_aa_freqs_lg,
   pll_aa_freqs_dcmut,
   pll_aa_freqs_jtt,
   pll_aa_freqs_mtrev,
   pll_aa_freqs_wag,
   pll_aa_freqs_rtrev,
   pll_aa_freqs_cprev,
   pll_aa_freqs_vt,
   pll_aa_freqs_blosum62,
   pll_aa_freqs_mtmam,
   pll_aa_freqs_mtart,
   pll_aa_freqs_mtzoa,
   pll_aa_freqs_pmb,
   pll_aa_freqs_hivb,
   pll_aa_freqs_hivw,
   pll_aa_freqs_jttdcmut,
   pll_aa_freqs_flu,
   pll_aa_freqs_stmtrev
};

using namespace std;

namespace modeltest {

static void set_subst_params(int * m_ind, string const& matrix)
 {
    for (mt_index_t i=0; i<N_DNA_SUBST_RATES; i++)
      m_ind[i] = (int)(matrix.at(i) - '0');
 }

Model::Model(mt_mask_t model_params,
             const partition_descriptor_t &partition,
             mt_size_t states,
             asc_bias_t asc_bias_corr)
    : model_params(model_params),
      optimize_pinv(model_params & (MOD_PARAM_INV | MOD_PARAM_INV_GAMMA)),
      optimize_gamma((model_params & (MOD_PARAM_GAMMA | MOD_PARAM_INV_GAMMA))
                     && !(model_params & MOD_PARAM_FREE_RATES)),
      optimize_freqs(model_params & MOD_PARAM_ESTIMATED_FREQ),
      empirical_freqs(model_params & MOD_PARAM_EMPIRICAL_FREQ),
      mixture(model_params & MOD_PARAM_MIXTURE),
      gap_aware(partition.gap_aware),
      asc_bias_corr(asc_bias_corr)
{
    matrix_index = 0;
    current_opt_parameter = 0;
    unique_id = 0;

    loglh  = 0.0;
    bic  = 0.0;
    aic  = 0.0;
    aicc = 0.0;
    dt   = 0.0;

    n_categories = 0;
    params_indices = 0;

    tree = 0;
    n_tips = 0;

    asc_weights = 0;

    param_gamma    = 0;
    param_pinv     = 0;
    param_branches = 0;
    param_freqs    = 0;
    param_substrates    = 0;

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
    }
    else
    {
      param_freqs = new ParameterFrequenciesFixed(
                          states,
                          model_params & MOD_PARAM_FIXED_FREQ);
    }
    parameters.push_back(param_freqs);
}

Model::Model( void )
    : optimize_pinv(false),
      optimize_gamma(false),
      optimize_freqs(false),
      empirical_freqs(false),
      mixture(false),
      gap_aware(false),
      asc_bias_corr(asc_none)
{
  model_params = (mt_mask_t) 0;
  matrix_index = 0;
  current_opt_parameter = 0;

  loglh  = 0.0;
  bic  = 0.0;
  aic  = 0.0;
  aicc = 0.0;
  dt   = 0.0;

  n_categories = 0;
  params_indices = 0;

  tree = 0;

  asc_weights = 0;
}

Model::~Model()
{
  if (asc_weights)
    delete[] asc_weights;
  if (params_indices)
    free(params_indices);
  if (tree)
  {
    pll_utree_destroy(tree->back);
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
  assert(0);
  return 0;
}

mt_size_t Model::get_n_categories() const
{
  return n_categories;
}

void Model::set_n_categories( mt_size_t ncat )
{
  n_categories = ncat;
  if (params_indices)
    free (params_indices);

  if (ncat > 0)
  {
    params_indices = (unsigned int *) Utils::c_allocate(
                                      n_categories, sizeof(unsigned int));
  }

  if (mixture)
  {
    assert(ncat == N_MIXTURE_CATS);
    for (mt_index_t i = 0; i<N_MIXTURE_CATS; ++i)
      params_indices[i] = i;
  }

  param_gamma->set_n_categories(ncat);
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
  return 0;
}

double Model::get_prop_inv() const
{
  if (!param_pinv)
    return 0.0;
  return param_pinv->get_pinv();
}

void Model::set_prop_inv(double value)
{
  param_pinv->set_pinv(value);
}
double Model::get_alpha() const
{
  return param_gamma->get_alpha();
}

void Model::set_alpha(double value)
{
  param_gamma->set_alpha(value);
}

mt_size_t Model::get_n_states( void ) const
{
  return n_frequencies;
}

mt_size_t Model::get_n_subst_rates( void ) const
{
  return n_subst_rates;
}

const double * Model::get_frequencies( void ) const
{
  return param_freqs->get_frequencies();
}

const double * Model::get_mixture_frequencies( mt_index_t matrix_idx ) const
{
  UNUSED(matrix_idx);
  return 0;
}

const unsigned int * Model::get_params_indices( void ) const
{
  assert(params_indices);
  return params_indices;
}

void Model::set_frequencies(const double value[])
{
  param_freqs->set_frequencies(value);
}

void Model::set_frequencies(const vector<double> & value)
{
  param_freqs->set_frequencies(value);
}

const double * Model::get_subst_rates( void ) const
{
  return param_substrates->get_subst_rates();
}

void Model::set_subst_rates(const double value[])
{
  param_substrates->set_subst_rates(value);
}

const double * Model::get_mixture_subst_rates( mt_index_t matrix_idx ) const
{
  UNUSED(matrix_idx);
  return 0;
}

const double * Model::get_mixture_weights( void ) const
{
  return 0;
}

const double * Model::get_mixture_rates( void ) const
{
  return 0;
}

bool Model::evaluate_criteria (mt_size_t n_branches_params,
                                double sample_size )
{
  if (!is_optimized())
      return false;

  mt_size_t n_params = n_free_variables + n_branches_params;

  aic = 2*n_params - 2*loglh;
  aicc = aic + 2*n_params*(n_params+1)/(n_params - sample_size - 1);
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

void Model::set_tree( pll_utree_t * _tree )
{
  if (tree)
  {
    pll_utree_destroy(tree->back);
  }
  tree = _tree;
}

mt_index_t Model::get_unique_id( void ) const
{
  return unique_id;
}

bool Model::optimize_init ( pll_partition_t * pll_partition,
                            pll_utree_t * pll_tree,
                            Partition const& partition )
{
  mt_opt_params_t params;
  params.partition = pll_partition;
  params.tree = pll_tree;
  params.params_indices = params_indices;

  bool result = true;
  for (AbstractParameter * parameter : parameters)
    result &= parameter->initialize(&params, partition);
  current_opt_parameter = 0;
  return result;
}

bool Model::optimize( pll_partition_t * partition, pll_utree_t * tree, double tolerance )
{
    mt_opt_params_t params;
    params.partition = partition;
    params.tree = tree;
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
                                   pll_utree_t * tree,
                                   double tolerance )
{
  assert(current_opt_parameter < parameters.size());
  mt_opt_params_t params;
  params.partition = partition;
  params.tree = tree;
  params.params_indices = params_indices;

  AbstractParameter * parameter = parameters[current_opt_parameter];
  loglh = parameter->optimize(&params, loglh, tolerance, true);
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

DnaModel::DnaModel(mt_index_t _matrix_index,
                   mt_mask_t model_params,
                   const partition_descriptor_t &partition,
                   asc_bias_t asc_bias_corr,
                   const mt_size_t *asc_w)
    : Model(model_params, partition, N_DNA_STATES, asc_bias_corr)
{
    stringstream ss_name;

    matrix_index = _matrix_index;

    assert(matrix_index < N_DNA_ALLMATRIX_COUNT);

    n_frequencies = gap_aware?(N_DNA_STATES+1):N_DNA_STATES;
    n_subst_rates = gap_aware?10:N_DNA_SUBST_RATES;

    matrix_symmetries = new int[n_subst_rates];
    set_subst_params(matrix_symmetries, dna_model_matrices[matrix_index]);

    param_gamma = new ParameterGamma(optimize_gamma?n_categories:1);
    parameters.push_back(param_gamma);

    param_substrates = new ParameterSubstRatesOpt(matrix_symmetries, n_subst_rates);
    parameters.push_back(param_substrates);

    mt_index_t standard_matrix_index = (mt_index_t) (find(dna_model_matrices_indices,
                                     dna_model_matrices_indices + N_DNA_MODEL_MATRICES,
                                     matrix_index) - dna_model_matrices_indices);
    if (standard_matrix_index < N_DNA_MODEL_MATRICES)
    {
        ss_name << dna_model_names[2 * standard_matrix_index + (is_F()?1:0)];
    }
    else
    {
        ss_name << dna_model_matrices[matrix_index];
        if (is_F())
            ss_name << "[F]";
    }

    if (optimize_pinv)
        ss_name << "+I";
    if (optimize_gamma)
        ss_name << "+G";
    name = ss_name.str();

    n_free_variables = param_substrates->get_n_free_parameters();
    n_free_variables += param_freqs->get_n_free_parameters();
    n_free_variables += param_gamma->get_n_free_parameters();
    n_free_variables += param_pinv?param_pinv->get_n_free_parameters():0;

    if (asc_bias_corr == asc_felsenstein)
    {
      assert(asc_w);
      asc_weights = new mt_size_t[n_frequencies];
      fill_n(asc_weights, n_frequencies, 0);
      asc_weights[0] = asc_w[0];
    }
    else if (asc_bias_corr == asc_stamatakis)
    {
      assert(asc_w);
      asc_weights = new mt_size_t[N_DNA_STATES];
      memcpy(asc_weights, asc_w, N_DNA_STATES * sizeof(mt_size_t));
    }

    unique_id = matrix_index * 8 + optimize_freqs + 2 * optimize_pinv + 4 * optimize_gamma;
}

DnaModel::DnaModel(const Model & other)
    : Model()
{
    matrix_symmetries = new int[other.get_n_subst_rates()];
    clone(&other);
}

DnaModel::~DnaModel( void )
{
  delete[] matrix_symmetries;
}

mt_index_t DnaModel::get_index_for_matrix(const int * matrix)
{
  string matrix_str = Utils::int_array_to_string(matrix, N_DNA_SUBST_RATES);
  mt_index_t matrix_index = (mt_index_t) (find(dna_model_matrices,
                                   dna_model_matrices + N_DNA_ALLMATRIX_COUNT,
                                   matrix_str) - dna_model_matrices);
  assert(matrix_index < N_DNA_ALLMATRIX_COUNT);
  return matrix_index;
}

void DnaModel::clone(const Model * other_model)
{
  const DnaModel * other = dynamic_cast<const DnaModel *>(other_model);
  matrix_index = other->matrix_index;
  name = other->name;
  memcpy(matrix_symmetries, other->matrix_symmetries, N_DNA_SUBST_RATES * sizeof(int));
  model_params    = other->model_params;
  optimize_pinv   = other->optimize_pinv;
  optimize_gamma  = other->optimize_gamma;
  optimize_freqs  = other->optimize_freqs;
  empirical_freqs = other->empirical_freqs;

  unique_id = other->unique_id;

  set_n_categories(other->n_categories);
  n_free_variables = other->n_free_variables;

  loglh  = other->loglh;
  bic  = other->bic;
  aic  = other->aic;
  aicc = other->aicc;
  dt   = other->dt;

  exec_time = other->exec_time;
  if (other->tree)
      tree = pll_utree_clone(other->tree);

  /* clone parameters */
  if (other->param_gamma)
  {
    param_gamma = new ParameterGamma(*dynamic_cast<ParameterGamma *>(other->param_gamma));
    parameters.push_back(param_gamma);
  }
  if (other->param_pinv)
  {
    param_pinv = new ParameterPinv(*(other->param_pinv));
    parameters.push_back(param_pinv);
  }
  if (other->param_substrates)
  {
    const ParameterSubstRatesOpt * other_rates =
      dynamic_cast<const ParameterSubstRatesOpt *>(other->param_substrates);
    param_substrates = new ParameterSubstRatesOpt(*other_rates);
    parameters.push_back(param_substrates);
  }
  if (other->param_freqs)
  {
    if (optimize_freqs)
      param_freqs = new ParameterFrequenciesOpt(
        *(dynamic_cast<const ParameterFrequenciesOpt *>(
          other->param_freqs)));
    else
      param_freqs = new ParameterFrequenciesFixed(
        *(dynamic_cast<const ParameterFrequenciesFixed *>(
          other->param_freqs)));
    parameters.push_back(param_freqs);
  }
  if (other->param_branches)
  {
    param_branches = new ParameterBranches(*(other->param_branches));
    parameters.push_back(param_branches);
  }
}

const int * DnaModel::get_symmetries( void ) const
{
  return matrix_symmetries;
}

mt_size_t DnaModel::get_n_subst_params() const
{
  return (mt_size_t) *max_element(matrix_symmetries,
                                  matrix_symmetries+N_DNA_SUBST_RATES);
}

static mt_mask_t asc_bias_attribute(asc_bias_t v)
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

pll_partition_t * DnaModel::build_partition(mt_size_t _n_tips,
                                            mt_size_t n_sites,
                                            mt_size_t n_cat_g)
{
    mt_mask_t attributes = PLL_ATTRIB_PATTERN_TIP;
    mt_size_t states = n_frequencies;

    assert(!n_tips && _n_tips);
    n_tips = _n_tips;

#ifdef HAVE_AVX
    attributes |= PLL_ATTRIB_ARCH_AVX;
#else
#ifdef HAVE_SSE
    attributes |= PLL_ATTRIB_ARCH_SSE;
#else
    attributes |= PLL_ATTRIB_ARCH_CPU;
#endif
#endif

    attributes |= asc_bias_attribute(asc_bias_corr);

    pll_partition_t * part = pll_partition_create (
                n_tips,                           /* tips */
                n_tips-2,                         /* clv buffers */
                states,                           /* states */
                n_sites,                          /* sites */
                1,                                /* rate matrices */
                2*n_tips-3,                       /* prob matrices */
                optimize_gamma ? n_cat_g : 1,     /* rate cats */
                n_tips-2,                         /* scale buffers */
                attributes                        /* attributes */
                );

    if (asc_weights)
      pll_set_asc_state_weights(part, asc_weights);

    return part;
}

void DnaModel::print(std::ostream  &out)
{
    const double * subst_rates = get_subst_rates();
    out << fixed << setprecision(MT_PRECISION_DIGITS);
    out << setw(PRINTMODEL_TABSIZE) << left << "Model:" << name << endl
        << setw(PRINTMODEL_TABSIZE) << left << "lnL:" << loglh << endl
        << setw(PRINTMODEL_TABSIZE) << left << "Frequencies:";
    param_freqs->print(out);
    out << endl;
    out << setw(PRINTMODEL_TABSIZE) << left << "Subst. Rates:";
    for (mt_index_t i=0; i<n_subst_rates; i++)
        out << subst_rates[i] << " ";
    out << endl;
    out << setw(PRINTMODEL_TABSIZE) << left << "Inv. sites prop:";
    if (is_I())
        out << get_prop_inv() << endl;
    else
        out << "-" << endl;
    out << setw(PRINTMODEL_TABSIZE) << left << "Gamma shape:";
    if (is_G())
        out << get_alpha() << endl;
    else
        out << "-" << endl;
}

void DnaModel::print_xml(std::ostream  &out)
{
    const double * subst_rates = get_subst_rates();
    out << fixed << setprecision(MT_PRECISION_DIGITS);
    out << "<model datatype=\"dna\" name=\"" << name
        << "\" lnl=\"" << loglh
        << "\">" << endl;
    out << "  <frequencies type=\"";
    if (is_F())
        out << "maximum-likelihood";
    else
        out << "fixed";
    out << "\">" << endl << "  ";
    param_freqs->print(out);
    out << endl << "  </frequencies>" << endl;
    out << "  <subst_rates>" << endl << "  ";
    for (mt_index_t i=0; i<n_subst_rates; i++)
        out << "  " << subst_rates[i];
    out << endl << "  </subst_rates>" << endl;
    out << "  <rate_params pinv=\"" << get_prop_inv() <<
           "\" alpha=\"" << get_alpha() << "\"/>" << endl;
    out << "</model>" << endl;
}

void DnaModel::output_log(std::ostream  &out) const
{
    const double * subst_rates = get_subst_rates();
    out << fixed << setprecision(MT_PRECISION_DIGITS);
    out << name << " ";
    out << matrix_index << " ";
    for (mt_index_t i=0; i<n_subst_rates; i++)
        out << matrix_symmetries[i] << " ";
    out << n_free_variables << " ";
    out << optimize_freqs << " " << empirical_freqs << " " << optimize_pinv << " " << optimize_gamma << " ";
    out << loglh << " ";
    param_freqs->print(out);
    for (mt_index_t i=0; i<n_subst_rates; i++)
        out << subst_rates[i] << " ";
    out << get_prop_inv() << " ";
    out << get_alpha() << " ";
    char *newick = pll_utree_export_newick(tree);
    out << strlen(newick) << " ";
    out << newick << " ";
    free(newick);
    out << endl;
}

static bool read_word(std::istream  &in, char *w, mt_size_t maxlen)
{
    char c = '\0';
    mt_index_t l = 0;
    while (in.get(c) && c!=' ' && l<maxlen)
    {
        *(w++) = c;
        l++;
    }
    *w = '\0';
    return c != '\0';
}

#define LOG_LEN 50
void DnaModel::input_log(std::istream  &in)
{
    char str[LOG_LEN];
    char *treestr;

    read_word(in, str, LOG_LEN);       name = str;
    read_word(in, str, LOG_LEN); matrix_index = Utils::parse_index(str);
    for (mt_index_t i=0; i<N_DNA_SUBST_RATES; i++)
    {
        read_word(in, str, LOG_LEN); matrix_symmetries[i] = atoi(str);
    }
    read_word(in, str, LOG_LEN); n_free_variables = Utils::parse_size(str);
    read_word(in, str, LOG_LEN); optimize_freqs = atoi(str);
    read_word(in, str, LOG_LEN); empirical_freqs = atoi(str);
    read_word(in, str, LOG_LEN); optimize_pinv = atoi(str);
    read_word(in, str, LOG_LEN); optimize_gamma = atoi(str);
    read_word(in, str, LOG_LEN); loglh = atof(str);

    double frequencies[n_frequencies];
    for (mt_index_t i=0; i<n_frequencies; i++)
    {
        read_word(in, str, LOG_LEN); frequencies[i] = atof(str);
    }
    param_freqs->set_frequencies(frequencies);

    double subst_rates[n_subst_rates];
    for (mt_index_t i=0; i<n_subst_rates; i++)
    {
       read_word(in, str, LOG_LEN); subst_rates[i] = atof(str);
    }
    param_substrates->set_subst_rates(subst_rates);

    read_word(in, str, LOG_LEN);
    if (optimize_pinv)
    {
      set_prop_inv(atof(str));
    }
    if (optimize_gamma)
    {
      read_word(in, str, LOG_LEN); set_alpha(atof(str));
    }
    read_word(in, str, LOG_LEN); mt_size_t treelen = Utils::parse_size(str);

    treestr = (char *) Utils::allocate(treelen + 1, 1);
    read_word(in, treestr, treelen);

    //TODO: USE BINARY MODULE
    mt_size_t n_tips;
    tree = pll_utree_parse_newick_string(treestr, &n_tips);
    pll_utree_t **nodes = (pll_utree_t **) malloc(n_tips * sizeof(pll_utree_t *));
    pll_utree_query_tipnodes(tree, nodes);
    tree = nodes[0];

    free(treestr);
}

#undef LOG_LEN

int DnaModel::output_bin(std::string const& bin_filename) const
{
  ckpdata_t ckp_data;
  pll_binary_header_t input_header;
  const double * subst_rates = get_subst_rates();
  const double * frequencies = get_frequencies();
  int write_ok;
  FILE * bin_file;

  bin_file = pllmod_binary_append_open(bin_filename.c_str(), &input_header);
  if (!bin_file)
    return false;

  ckp_data.matrix_index = matrix_index;
  ckp_data.optimize_freqs = optimize_freqs;
  ckp_data.empirical_freqs = empirical_freqs;
  ckp_data.optimize_pinv = optimize_pinv;
  ckp_data.optimize_gamma = optimize_gamma;
  ckp_data.loglh = loglh;
  ckp_data.bic = bic;
  ckp_data.aic = aic;
  ckp_data.aicc = aicc;
  ckp_data.dt = dt;
  ckp_data.n_tips = n_tips;

  memset(ckp_data.frequencies, 0, N_PROT_STATES * sizeof(double));
  memcpy(ckp_data.frequencies, frequencies, N_DNA_STATES * sizeof(double));
  memcpy(ckp_data.subst_rates, subst_rates, N_DNA_SUBST_RATES * sizeof(double));
  ckp_data.prop_invar = get_prop_inv();
  ckp_data.alpha = get_alpha();

  ckp_data.exec_time = exec_time;

  assert(bin_file);
  write_ok = pllmod_binary_custom_dump(bin_file,
                            unique_id,
                            &ckp_data,
                            sizeof(ckpdata_t),
                            PLLMOD_BIN_ATTRIB_UPDATE_MAP);

  assert(n_tips > 0);
  write_ok &= pllmod_binary_utree_dump(bin_file, 2000+unique_id, tree->back, n_tips,
                                       PLLMOD_BIN_ATTRIB_UPDATE_MAP);

  pllmod_binary_close(bin_file);

  if(!write_ok)
  {
    mt_errno = pll_errno;
    snprintf(mt_errmsg, ERR_MSG_SIZE, "Error dumping checkpoint");
  }

  return write_ok;
}

int DnaModel::input_bin(std::string const& bin_filename)
{
  ckpdata_t * ckp_data;
  pll_binary_header_t input_header;
  size_t size = 0;
  unsigned int type, attributes;
  int read_ok;
  FILE * bin_file;

  bin_file = pllmod_binary_open(bin_filename.c_str(), &input_header);
  if (!bin_file)
  {
    mt_errno = pll_errno;
    snprintf(mt_errmsg, ERR_MSG_SIZE, "Error opening file: %d %s",
             pll_errno, pll_errmsg);
    return false;
  }

  ckp_data = (ckpdata_t *) pllmod_binary_custom_load(bin_file,
                                             unique_id,
                                             &size,
                                             &type,
                                             &attributes,
                                             PLLMOD_BIN_ACCESS_SEEK);
  read_ok = (ckp_data != NULL);

  if (read_ok)
  {
    assert(ckp_data->matrix_index == matrix_index);
    assert(ckp_data->optimize_freqs == optimize_freqs);
    assert(ckp_data->empirical_freqs == empirical_freqs);
    assert(ckp_data->optimize_pinv == optimize_pinv);
    assert(ckp_data->optimize_gamma == optimize_gamma);
    set_loglh(ckp_data->loglh);
    bic  = ckp_data->bic;
    aic  = ckp_data->aic;
    aicc = ckp_data->aicc;
    dt   = ckp_data->dt;
    exec_time = ckp_data->exec_time;

    set_frequencies(ckp_data->frequencies);
    set_subst_rates(ckp_data->subst_rates);
    if (optimize_pinv)
      set_prop_inv(ckp_data->prop_invar);
    if (optimize_gamma)
      set_alpha(ckp_data->alpha);
    n_tips = ckp_data->n_tips;

    free(ckp_data);

    pll_errno = 0;
    tree = pllmod_binary_utree_load(bin_file,
                                 2000+unique_id,
                                 &attributes,
                                 PLLMOD_BIN_ACCESS_SEEK);

    if(!tree)
    {
      mt_errno = pll_errno;
      snprintf(mt_errmsg, ERR_MSG_SIZE, "Error loading tree from ckp file");
      read_ok = false;
    }
    else
      tree = tree->back;
  }
  else
  {
    mt_errno = pll_errno;
    snprintf(mt_errmsg, ERR_MSG_SIZE, "%s", pll_errmsg);
  }

  pllmod_binary_close(bin_file);

  return read_ok;
}

/* PROTEIN MODELS */

ProtModel::ProtModel(mt_index_t _matrix_index,
             mt_mask_t model_params,
             const partition_descriptor_t &partition,
             asc_bias_t asc_bias_corr,
             const mt_size_t *asc_w)
    : Model(model_params, partition, N_PROT_STATES, asc_bias_corr)
{
  stringstream ss_name;

  matrix_index = _matrix_index;
  assert(matrix_index < N_PROT_MODEL_ALL_MATRICES);
  mixture = (matrix_index == LG4M_INDEX || matrix_index == LG4X_INDEX);

  n_frequencies   = N_PROT_STATES;
  n_subst_rates   = N_PROT_SUBST_RATES;
  n_free_variables = 0;

  if (mixture)
  {
      assert (optimize_gamma && !empirical_freqs);
      if (matrix_index == LG4M_INDEX)
      {
          /* LG4M model */
          param_gamma = new ParameterGamma(n_categories);
          parameters.push_back(param_gamma);

          mixture_frequencies = pll_aa_freqs_lg4m;
          mixture_subst_rates = pll_aa_rates_lg4m;
      }
      else
      {
          /* LG4X model / free rates */
          param_gamma = new ParameterRateCats(4);
          parameters.push_back(param_gamma);

          optimize_gamma = false;
          n_free_variables += 6;
          mixture_frequencies = pll_aa_freqs_lg4x;
          mixture_subst_rates = pll_aa_rates_lg4x;
      }
  }
  else
  {
      param_gamma = new ParameterGamma(optimize_gamma?n_categories:1);
      parameters.push_back(param_gamma);

      fixed_subst_rates = prot_model_rates[matrix_index];
      if (!empirical_freqs)
        param_freqs->set_frequencies(prot_model_freqs[matrix_index]);
  }

  if (asc_bias_corr == asc_felsenstein)
  {
    assert(asc_w);
    asc_weights = new mt_size_t[N_PROT_STATES];
    fill_n(asc_weights, N_PROT_STATES, 0);
    asc_weights[0] = asc_w[0];
  }
  else if (asc_bias_corr == asc_stamatakis)
  {
    assert(asc_w);
    asc_weights = new mt_size_t[N_PROT_STATES];
    memcpy(asc_weights, asc_w, N_PROT_STATES * sizeof(mt_size_t));
  }

  ss_name << prot_model_names[matrix_index];
  if (optimize_pinv)
      ss_name << "+I";
  if (optimize_gamma && !mixture)
      ss_name << "+G";

  if (empirical_freqs)
  {
    ss_name << "+F";
  }
  name = ss_name.str();

  if (optimize_freqs || empirical_freqs)
      n_free_variables += N_PROT_STATES-1;

  if (optimize_pinv)
      n_free_variables ++;
  if (optimize_gamma)
      n_free_variables ++;

  unique_id = 8 * matrix_index +
              4 * optimize_gamma +
              2 * optimize_pinv +
              (optimize_freqs || empirical_freqs);
}

ProtModel::ProtModel(const Model & other)
    : Model()
{
    clone(&other);
}

ProtModel::~ProtModel()
{
}

void ProtModel::clone(const Model * other_model)
{
    const ProtModel * other = dynamic_cast<const ProtModel *>(other_model);
    matrix_index = other->matrix_index;
    name = other->name;
    model_params    = other->model_params;
    optimize_pinv   = other->optimize_pinv;
    optimize_gamma  = other->optimize_gamma;
    optimize_freqs  = other->optimize_freqs;
    empirical_freqs = other->empirical_freqs;

    unique_id = other->unique_id;
    //TODO: Check for LG4!
    // memcpy(frequencies, other->frequencies, N_PROT_STATES * sizeof(double));
    fixed_subst_rates = other->fixed_subst_rates;

    set_n_categories(other->n_categories);
    n_free_variables = other->n_free_variables;

    loglh  = other->loglh;
    bic  = other->bic;
    aic  = other->aic;
    aicc = other->aicc;
    dt   = other->dt;

    exec_time = other->exec_time;
    if (other->tree)
        tree = pll_utree_clone(other->tree);
}

mt_size_t ProtModel::get_n_subst_params() const
{
    /* substitution rates are fixed */
    return 0;
}

const double * ProtModel::get_mixture_frequencies( mt_index_t matrix_idx ) const
{
    return mixture_frequencies[matrix_idx];
}

const double * ProtModel::get_subst_rates( void ) const
{
    return fixed_subst_rates;
}

const double * ProtModel::get_mixture_subst_rates( mt_index_t matrix_idx ) const
{
    return mixture_subst_rates[matrix_idx];
}

const double * ProtModel::get_mixture_weights( void ) const
{
    return param_gamma->get_weights();
}

const double * ProtModel::get_mixture_rates( void ) const
{
    return param_gamma->get_rates();
}

pll_partition_t * ProtModel::build_partition(mt_size_t _n_tips,
                                            mt_size_t n_sites,
                                            mt_size_t n_cat_g)
{
    mt_mask_t attributes = PLL_ATTRIB_PATTERN_TIP;

    assert(!n_tips && _n_tips);
    n_tips = _n_tips;

#ifdef HAVE_AVX
    attributes |= PLL_ATTRIB_ARCH_AVX;
#else
#ifdef HAVE_SSE
    attributes |= PLL_ATTRIB_ARCH_SSE;
#else
    attributes |= PLL_ATTRIB_ARCH_CPU;
#endif
#endif
    attributes |= asc_bias_attribute(asc_bias_corr);

    mt_size_t n_cats = optimize_gamma?n_cat_g:1;
    if (mixture)
        n_cats = N_MIXTURE_CATS;

    pll_partition_t * part = pll_partition_create (
                n_tips,                           /* tips */
                n_tips-2,                         /* clv buffers */
                N_PROT_STATES,                    /* states */
                n_sites,                          /* sites */
                mixture?N_MIXTURE_CATS:1,         /* rate matrices */
                2*n_tips-3,                       /* prob matrices */
                n_cats,                           /* rate cats */
                n_tips-2,                         /* scale buffers */
                attributes                        /* attributes */
                );

    if (asc_weights)
      pll_set_asc_state_weights(part, asc_weights);

    return part;
}

void ProtModel::print(std::ostream  &out)
{
  double const *rates = param_gamma->get_rates();
  double const *rate_weights = param_gamma->get_weights();
  out << fixed << setprecision(MT_PRECISION_DIGITS);
  out << setw(PRINTMODEL_TABSIZE) << left << "Model:" << name << endl
      << setw(PRINTMODEL_TABSIZE) << left << "lnL:" << loglh << endl;
  if (mixture)
  {
      for (mt_index_t j=0; j<N_MIXTURE_CATS; j++)
      {
          for (mt_index_t i=0; i<N_PROT_STATES; i++)
          {
              out <<mixture_frequencies[j][i] << " ";
              if ((i+1)<N_PROT_STATES && !((i+1)%5))
              {
                  out << endl << setw(PRINTMODEL_TABSIZE) << " ";
              }
          }
          out << endl << endl;
          if (j <N_MIXTURE_CATS-1) out << setw(PRINTMODEL_TABSIZE) << " ";
      }
      if (matrix_index == LG4X_INDEX)
      {
        out << setw(PRINTMODEL_TABSIZE) << left << "Mixture weights:";
        for (mt_index_t j=0; j<N_MIXTURE_CATS; j++)
        {
           out << rate_weights[j] << " ";
        }
        out << endl;
        out << setw(PRINTMODEL_TABSIZE) << left << "Mixture rates:";
        for (mt_index_t j=0; j<N_MIXTURE_CATS; j++)
        {
           out << rates[j] << " ";
        }
        out << endl;
      }
  }
  else
  {
    out << setw(PRINTMODEL_TABSIZE) << left << "Frequencies:";
    param_freqs->print(out);
    out << endl;
  }
  out << setw(PRINTMODEL_TABSIZE) << left << "Inv. sites prop:";
  if (is_I())
      out << get_prop_inv() << endl;
  else
      out << "-" << endl;
  out << setw(PRINTMODEL_TABSIZE) << left << "Gamma shape:";
  if (is_G())
      out << get_alpha() << endl;
  else
      out << "-" << endl;
}

void ProtModel::print_xml(std::ostream  &out)
{
  out << fixed << setprecision(MT_PRECISION_DIGITS);
  out << "<model type=\"aa\" name=\"" << name
      << "\" lnl=\"" << loglh
      << "\">" << endl;
  out << "  <frequencies type=\"";
  if (is_F())
      out << "empirical";
  else
      out << "model";
  out << "\">" << endl << "  ";
  param_freqs->print(out);
  // for (mt_index_t i=0; i<N_PROT_STATES; i++)
  // {
  //     out << "  " << setprecision(MT_PRECISION_DIGITS) << frequencies[i];
  //     if ((i%5)==4) out << endl << "  ";
  // }
  out << "</frequencies>" << endl;
  out << "  <rate_params pinv=\"" << get_prop_inv() <<
         "\" alpha=\"" << get_alpha() << "\"/>" << endl;
  out << "</model>" << endl;
}

void ProtModel::output_log(std::ostream  &out) const
{
  out << fixed << setprecision(MT_PRECISION_DIGITS);
  out << get_name() << " ";
  out << get_matrix_index() << " ";
  out << is_F() << " " << is_I() << " " << is_G() << " ";
  out << get_loglh() << " ";
  param_freqs->print(out);
  // for (mt_index_t i=0; i<N_PROT_STATES; i++)
  //     out << setprecision(MT_PRECISION_DIGITS) << frequencies[i] << " ";
  if (is_I())
      out << get_prop_inv() << " ";
  else
      out << "- ";
  if (is_G())
      out << get_alpha() << " ";
  else
      out << "- ";
  char *newick = pll_utree_export_newick(tree);
  out << newick << " ";
  free(newick);
  out << endl;
}

void ProtModel::input_log(std::istream  &in)
{
  //TODO:
}

int ProtModel::output_bin(std::string const& bin_filename) const
{
  ckpdata_t ckp_data;
  pll_binary_header_t input_header;
  const double * frequencies = get_frequencies();
  int write_ok;
  FILE * bin_file;

  bin_file = pllmod_binary_append_open(bin_filename.c_str(), &input_header);
  if (!bin_file)
    return false;

  ckp_data.matrix_index    = matrix_index;
  ckp_data.optimize_freqs  = optimize_freqs;
  ckp_data.empirical_freqs = empirical_freqs;
  ckp_data.optimize_pinv   = optimize_pinv;
  ckp_data.optimize_gamma  = optimize_gamma;
  ckp_data.loglh = loglh;
  ckp_data.bic   = bic;
  ckp_data.aic   = aic;
  ckp_data.aicc  = aicc;
  ckp_data.dt    = dt;
  ckp_data.n_tips = n_tips;

  memcpy(ckp_data.frequencies, frequencies, N_PROT_STATES * sizeof(double));
  memset(ckp_data.subst_rates, 0, N_DNA_SUBST_RATES * sizeof(double));

  ckp_data.prop_invar = get_prop_inv();
  ckp_data.alpha = get_alpha();

  ckp_data.exec_time = exec_time;

  assert(bin_file);
  write_ok = pllmod_binary_custom_dump(bin_file,
                            unique_id,
                            &ckp_data,
                            sizeof(ckpdata_t),
                            PLLMOD_BIN_ATTRIB_UPDATE_MAP);

  assert(n_tips > 0);
  write_ok &= pllmod_binary_utree_dump(bin_file, 2000+unique_id, tree->back, n_tips,
                                       PLLMOD_BIN_ATTRIB_UPDATE_MAP);

  pllmod_binary_close(bin_file);

  if(!write_ok)
  {
    mt_errno = pll_errno;
    snprintf(mt_errmsg, ERR_MSG_SIZE, "Error dumping checkpoint");
  }

  return write_ok;
}

int ProtModel::input_bin(std::string const& bin_filename)
{
  ckpdata_t * ckp_data;
  pll_binary_header_t input_header;
  size_t size = 0;
  unsigned int type, attributes;
  int read_ok;
  FILE * bin_file;

  bin_file = pllmod_binary_open(bin_filename.c_str(), &input_header);
  if (!bin_file)
  {
    mt_errno = pll_errno;
    snprintf(mt_errmsg, ERR_MSG_SIZE, "Error opening file: %d %s",
             pll_errno, pll_errmsg);
    return false;
  }

  ckp_data = (ckpdata_t *) pllmod_binary_custom_load(bin_file,
                                             unique_id,
                                             &size,
                                             &type,
                                             &attributes,
                                             PLLMOD_BIN_ACCESS_SEEK);
  read_ok = (ckp_data != NULL);

  if (read_ok)
  {
    assert(ckp_data->matrix_index == matrix_index);
    assert(ckp_data->optimize_freqs == optimize_freqs);
    assert(ckp_data->empirical_freqs == empirical_freqs);
    assert(ckp_data->optimize_pinv == optimize_pinv);
    assert(ckp_data->optimize_gamma == optimize_gamma);
    set_loglh(ckp_data->loglh);
    bic  = ckp_data->bic;
    aic  = ckp_data->aic;
    aicc = ckp_data->aicc;
    dt   = ckp_data->dt;
    exec_time = ckp_data->exec_time;

    set_frequencies(ckp_data->frequencies);
    if (optimize_pinv)
      set_prop_inv(ckp_data->prop_invar);
    if (optimize_gamma)
      set_alpha(ckp_data->alpha);
    n_tips = ckp_data->n_tips;

    free(ckp_data);

    pll_errno = 0;
    tree = pllmod_binary_utree_load(bin_file,
                                 2000+unique_id,
                                 &attributes,
                                 PLLMOD_BIN_ACCESS_SEEK);

    if(!tree)
    {
      mt_errno = pll_errno;
      snprintf(mt_errmsg, ERR_MSG_SIZE, "Error loading tree from ckp file");
      read_ok = false;
    }
    else
      tree = tree->back;
  }
  else
  {
    mt_errno = pll_errno;
    snprintf(mt_errmsg, ERR_MSG_SIZE, "%s", pll_errmsg);
  }

  pllmod_binary_close(bin_file);

  return read_ok;
}

} /* namespace */
