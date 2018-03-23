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

#include "protmodel.h"
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

ProtModel::ProtModel(mt_index_t _matrix_index,
             mt_mask_t model_params,
             const partition_descriptor_t &partition,
             asc_bias_t asc_bias_corr,
             const mt_size_t *asc_w)
    : Model(model_params,
            partition,
            N_PROT_STATES,
            (_matrix_index == LG4M_INDEX || _matrix_index == LG4X_INDEX)
              ? N_MIXTURE_CATS
              : partition.n_categories,
            asc_bias_corr)
{
  stringstream ss_name;

  matrix_index = _matrix_index;
  assert(matrix_index < N_PROT_MODEL_ALL_MATRICES);
  assert(mixture == (matrix_index == LG4M_INDEX || matrix_index == LG4X_INDEX));

  n_frequencies   = N_PROT_STATES;
  n_subst_rates   = N_PROT_SUBST_RATES;

  if (mixture)
  {
      assert (!empirical_freqs);
      assert(!param_freqs);
      param_freqs = new ParameterFrequenciesFixed(N_PROT_STATES,
                                                  freqs_predef,
                                                  4);

      param_substrates = new ParameterSubstRatesFixed(n_subst_rates, 4);
      if (matrix_index == LG4M_INDEX)
      {
        assert(optimize_gamma);
          /* LG4M model */
          //mixture_subst_rates = pll_aa_rates_lg4m;

          for (int i=0; i<4; ++i)
          {
            param_freqs->set_frequencies(pll_aa_freqs_lg4m[i], i);
            param_substrates->set_subst_rates(pll_aa_rates_lg4m[i], i);
          }
      }
      else
      {
        assert(optimize_ratecats);
          /* LG4X model / free rates */
          optimize_gamma = false;
          //mixture_subst_rates = pll_aa_rates_lg4x;
          for (int i=0; i<4; ++i)
          {
            param_freqs->set_frequencies(pll_aa_freqs_lg4x[i], i);
            param_substrates->set_subst_rates(pll_aa_rates_lg4x[i], i);
          }
      }
      parameters.push_back(param_freqs);
      parameters.push_back(param_substrates);
  }
  else
  {
      if (matrix_index == PROT_GTR_INDEX)
      {
        int *matrix_symmetries = new int[n_subst_rates];
        for (mt_index_t i=0; i<n_subst_rates; ++i)
        {
          matrix_symmetries[i] = i;
        }

        param_substrates = new ParameterSubstRatesOpt(matrix_symmetries, n_subst_rates);
        parameters.push_back(param_substrates);
      }
      else
      {
        param_substrates = new ParameterSubstRatesFixed(prot_model_rates[matrix_index], n_subst_rates);
        parameters.push_back(param_substrates);
      }

      assert(!param_freqs);
      param_freqs = new ParameterFrequenciesFixed(N_PROT_STATES,
                                                  empirical_freqs?freqs_empirical:freqs_predef);
      if (!empirical_freqs)
        param_freqs->set_frequencies(prot_model_freqs[matrix_index]);
      parameters.push_back(param_freqs);
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

  if (!mixture)
  {
    if (optimize_gamma)
        ss_name << "+G" << n_categories;
    else if (optimize_ratecats)
    {
      ss_name << "+R" << n_categories;
    }
  }

  if (empirical_freqs)
  {
    ss_name << "+F";
  }
  name = ss_name.str();

  n_free_variables = 0;
  for (AbstractParameter * parameter : parameters)
    if (parameter->get_char_id() != 'b')
      n_free_variables += parameter->get_n_free_parameters();

  assert(partition.unique_id > 0 && partition.unique_id < MAX_PARTITION_INDEX);

  unique_id = (partition.unique_id << (31-NBIT_PARTITION_INDEX)) +
               16 * matrix_index +
               8 * optimize_gamma +
               4 * optimize_pinv +
               2 * (optimize_freqs || empirical_freqs);
}

ProtModel::~ProtModel()
{
}

mt_size_t ProtModel::get_n_subst_params() const
{
    /* substitution rates are fixed */
    return 0;
}

pll_partition_t * ProtModel::build_partition(mt_size_t _n_tips,
                                            mt_size_t n_sites)
{
    mt_mask_t attributes;

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
                n_tips,                           /* tips */
                n_tips-2,                         /* clv buffers */
                N_PROT_STATES,                    /* states */
                n_sites,                          /* sites */
                mixture?N_MIXTURE_CATS:1,         /* rate matrices */
                2*n_tips-3,                       /* prob matrices */
                n_categories,                           /* rate cats */
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
  out << setw(PRINTMODEL_TABSIZE) << left << "Frequencies:";
      param_freqs->print(out, true, false, PRINTMODEL_TABSIZE);
      out << endl;

  if (is_R())
  {
      assert(rates && rate_weights);
      if (matrix_index == LG4X_INDEX)
      {
        out << setw(PRINTMODEL_TABSIZE) << left << "Mixture weights:";
        for (mt_index_t j=0; j<n_categories; j++)
        {
           out << rate_weights[j] << " ";
        }
        out << endl;
        out << setw(PRINTMODEL_TABSIZE) << left << "Mixture rates:";
        for (mt_index_t j=0; j<n_categories; j++)
        {
           out << rates[j] << " ";
        }
        out << endl;
      }
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
  char *newick = pll_utree_export_newick(tree->nodes[2*n_tips - 3], NULL);
  out << newick << " ";
  free(newick);
  out << endl;
}

void ProtModel::input_log(std::istream  &in)
{
  UNUSED(in);
  //TODO:
}

} /* namespace */
