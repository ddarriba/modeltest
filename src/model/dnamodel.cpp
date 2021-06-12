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
    n_subst_rates = gap_aware?10:pllmod_util_subst_rate_count(N_DNA_STATES);

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
          ss_name << "+FO";
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

    if (have_avx2)
    {
      attributes |= PLL_ATTRIB_ARCH_AVX2;
    }
    else if (have_avx)
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
                (optimize_gamma | optimize_ratecats) ? n_categories : 1, /* rate cats */
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


} /* namespace */
