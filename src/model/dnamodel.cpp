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

#include "dnamodel.h"
#include "../genesis/logging.h"
#include "../utils.h"

#include <cmath>
#include <iomanip>
#include <sstream>
#include <cstring>
#include <cassert>
#include <fstream>
#include <algorithm>

using namespace std;

namespace modeltest {

static void set_subst_params(int * m_ind, string const& matrix)
{
  for (mt_index_t i=0; i<N_DNA_SUBST_RATES; i++)
    m_ind[i] = (int)(matrix.at(i) - '0');
}

DnaModel::DnaModel(mt_index_t _matrix_index,
                   mt_mask_t model_params,
                   const partition_descriptor_t &partition,
                   asc_bias_t asc_bias_corr,
                   const mt_size_t *asc_w)
    : Model(model_params,
            partition,
            N_DNA_STATES,
            partition.n_categories,
            asc_bias_corr)
{
    stringstream ss_name;

    matrix_index = _matrix_index;

    assert(matrix_index < N_DNA_ALLMATRIX_COUNT);

    n_frequencies = gap_aware?(N_DNA_STATES+1):N_DNA_STATES;
    n_subst_rates = gap_aware?10:N_DNA_SUBST_RATES;

    if (!optimize_freqs)
    {
      assert(!param_freqs);
      param_freqs = new ParameterFrequenciesFixed(N_DNA_STATES,
                                                  freqs_equal);
      parameters.push_back(param_freqs);
    }

    matrix_symmetries = new int[n_subst_rates];
    set_subst_params(matrix_symmetries, dna_model_matrices[matrix_index]);

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
        {
          ss_name << "[F]";
        }
    }

    if (optimize_pinv)
    {
      ss_name << "+I";
    }
    if (optimize_gamma)
    {
      ss_name << "+G" << n_categories;
    }
    else if (optimize_ratecats)
    {
      ss_name << "+R" << n_categories;
    }

    name = ss_name.str();

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

    n_free_variables = 0;
    for (AbstractParameter * parameter : parameters)
      if (parameter->get_char_id() != 'b')
        n_free_variables += parameter->get_n_free_parameters();

    assert(partition.unique_id > 0 && partition.unique_id < MAX_PARTITION_INDEX);

    unique_id = (partition.unique_id << (31-NBIT_PARTITION_INDEX)) +
                16 * matrix_index +
                8 * optimize_freqs +
                4 * optimize_pinv +
                2 * optimize_gamma;
                /* reserve id+1 for tree */
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
  optimize_ratecats = other->optimize_ratecats;

  unique_id = other->unique_id;

  n_categories = other->n_categories;
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

pll_partition_t * DnaModel::build_partition(mt_size_t _n_tips,
                                            mt_size_t n_sites)
{
    mt_size_t states = n_frequencies;
    mt_mask_t attributes;

#ifndef PLL_ATTRIB_SITE_REPEATS
    /* safety check */
    assert(disable_repeats);
    #define PLL_ATTRIB_SITE_REPEATS 0
#endif

    if (disable_repeats)
    {
      attributes = PLL_ATTRIB_PATTERN_TIP;
    }
    else
    {
      attributes = PLL_ATTRIB_SITE_REPEATS;
    }

    assert(!n_tips && _n_tips);
    n_tips = _n_tips;

    if (have_avx)
    {
      attributes |= PLL_ATTRIB_ARCH_AVX;
    }
    else if (have_sse3)
    {
      attributes |= PLL_ATTRIB_ARCH_SSE;
    }
    else
    {
      attributes |= PLL_ATTRIB_ARCH_CPU;
    }

    attributes |= Model::asc_bias_attribute(asc_bias_corr);

    pll_partition_t * part = pll_partition_create (
                n_tips,                            /* tips */
                n_tips-2,                          /* clv buffers */
                states,                            /* states */
                n_sites,                           /* sites */
                1,                                 /* rate matrices */
                2*n_tips-3,                        /* prob matrices */
                optimize_gamma | optimize_ratecats ? n_categories : 1, /* rate cats */
                n_tips-2,                          /* scale buffers */
                attributes                         /* attributes */
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
    char *newick = pll_utree_export_newick(tree->nodes[2*n_tips - 3], NULL);
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
    tree = pll_utree_parse_newick_string(treestr);

    free(treestr);
}

#undef LOG_LEN

int DnaModel::output_bin(std::string const& bin_filename) const
{
  assert(ROOT);

  if (restored_from_ckp)
    return true;

  ckpdata_t ckp_data;
  pll_binary_header_t input_header;
  const double * subst_rates = get_subst_rates();
  const double * frequencies = get_frequencies();
  int write_ok;
  FILE * bin_file;

  std::unique_lock<std::mutex> lock(model_mutex);

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
  assert(tree);
  assert(tree->nodes);
  write_ok &= pllmod_binary_utree_dump(bin_file,
                                       unique_id + 1,
                                       tree->nodes[0]->back,
                                       n_tips,
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
    pll_unode_t * loaded_tree = pllmod_binary_utree_load(bin_file,
                                                       unique_id + 1,
                                                       &attributes,
                                                       PLLMOD_BIN_ACCESS_SEEK);

    if(!loaded_tree)
    {
      mt_errno = pll_errno;
      snprintf(mt_errmsg, ERR_MSG_SIZE,
               "Error loading tree from ckp file: %s",
               pll_errmsg);
      read_ok = false;
    }
    else
    {
      tree = pll_utree_wraptree(loaded_tree, n_tips);
    }
  }
  else
  {
    mt_errno = pll_errno;
    snprintf(mt_errmsg, ERR_MSG_SIZE, "%s", pll_errmsg);
  }

  pllmod_binary_close(bin_file);

  restored_from_ckp = read_ok;
  return read_ok;
}

} /* namespace */
