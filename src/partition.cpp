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

#include "partition.h"
#include "utils.h"

#include <sstream>
#include <iostream>
#include <algorithm>

#include "genesis/logging.h"

using namespace std;

namespace modeltest
{

static bool build_models(const partition_descriptor_t &descriptor,
                         std::vector<mt_index_t> candidate_models,
                         mt_mask_t model_params,
                         vector<Model *> &c_models,
                         string const& ckp_filename)
{
  data_type_t datatype = descriptor.datatype;
  asc_bias_t asc_bias_corr = descriptor.asc_bias_corr;
  const mt_size_t *asc_weights = descriptor.asc_weights;

  mt_size_t n_matrices = (mt_size_t) candidate_models.size();
  mt_size_t n_models = 0;

  mt_mask_t freq_params = model_params & MOD_MASK_FREQ_PARAMS;
  mt_mask_t rate_params = model_params & MOD_MASK_RATE_PARAMS;

  n_models = Utils::number_of_models(n_matrices, model_params);

  if (!n_models)
  {
      mt_errno = MT_ERROR_MODELS;
       snprintf(mt_errmsg, ERR_MSG_SIZE, "Candidate models set is empty");
      return false;
  }

  c_models.reserve(n_models);

  for (mt_index_t i=MOD_PARAM_MIN_RPARAM; i<=MOD_PARAM_MAX_RPARAM; i<<=1)
  {
      mt_mask_t cur_rate_param = rate_params & i;
      if (cur_rate_param)
      {
          for (mt_index_t model_matrix : candidate_models)
          {
              if (datatype == dt_dna)
              {
                  if (freq_params & MOD_PARAM_FIXED_FREQ)
                      c_models.push_back(
                                  new DnaModel(model_matrix,
                                    cur_rate_param | MOD_PARAM_FIXED_FREQ,
                                    descriptor,
                                    asc_bias_corr,
                                    asc_weights)
                                  );
                  if (freq_params & MOD_PARAM_ESTIMATED_FREQ)
                      c_models.push_back(
                                  new DnaModel(model_matrix,
                                    cur_rate_param | MOD_PARAM_ESTIMATED_FREQ,
                                    descriptor,
                                    asc_bias_corr,
                                    asc_weights)
                                  );
              }
              else if (datatype == dt_protein)
              {
                  if (model_matrix == LG4M_INDEX)
                  {
                    if (cur_rate_param & (MOD_PARAM_GAMMA | MOD_PARAM_INV_GAMMA))
                        c_models.push_back(
                            new ProtModel(LG4M_INDEX,
                                        cur_rate_param | MOD_PARAM_FIXED_FREQ,
                                        descriptor,
                                        asc_bias_corr,
                                        asc_weights)
                            );
                  }
                  else if (model_matrix == LG4X_INDEX)
                  {
                    if (cur_rate_param & (MOD_PARAM_GAMMA | MOD_PARAM_INV_GAMMA))
                        c_models.push_back(
                                new ProtModel(LG4X_INDEX,
                                        cur_rate_param | MOD_PARAM_FIXED_FREQ,
                                        descriptor,
                                        asc_bias_corr,
                                        asc_weights)
                                );
                  }
                  else
                  {
                      if (freq_params & MOD_PARAM_FIXED_FREQ)
                          c_models.push_back(
                                      new ProtModel(model_matrix,
                                                    cur_rate_param | MOD_PARAM_FIXED_FREQ,
                                                    descriptor,
                                                    asc_bias_corr,
                                                    asc_weights)
                                      );
                      if (freq_params & MOD_PARAM_EMPIRICAL_FREQ)
                          c_models.push_back(
                                      new ProtModel(model_matrix,
                                                    cur_rate_param | MOD_PARAM_EMPIRICAL_FREQ,
                                                    descriptor,
                                                    asc_bias_corr,
                                                    asc_weights)
                                      );
                  }
              }
              else
                  assert(0);
          }
      }
  }
  assert(datatype == dt_protein || (c_models.size() == n_models));

  if (ckp_filename.compare(""))
  {
    LOG_DBG << "[DBG] Attempting to load partition from checkpoint " << ckp_filename << endl;
    for (Model * model : c_models)
    {
      if (model->input_bin(ckp_filename))
      {
        LOG_DBG << "[DBG] Loaded model from checkpoint: " << model->get_name() << endl;
      }
    }
  }

  return true;
}

Partition::Partition(partition_id_t _id,
                     Msa & _msa,
                     Tree & _tree,
                     partition_descriptor_t _descriptor,
                     std::vector<mt_index_t> candidate_models,
                     mt_mask_t model_params,
                     string const& ckp_filename) :
    id(_id),
    msa(_msa),
    tree(_tree),
    descriptor(_descriptor),
    model_params(model_params),
    ckp_filename(ckp_filename)
{
  assert(_descriptor.unique_id > 0 && _descriptor.unique_id < MAX_PARTITION_INDEX);

  switch(descriptor.datatype)
  {
  case dt_dna:
      emp_freqs.resize(N_DNA_STATES);
      emp_subst_rates.resize(N_DNA_SUBST_RATES);
      break;
  case dt_protein:
      emp_freqs.resize(N_PROT_STATES);
      break;
  }

  n_patterns = 0;
  n_sites = 0;
  const unsigned int * wgt = msa.get_weights();
  for (const partition_region_t & region : descriptor.regions)
  {
      n_patterns += (region.end - region.start + 1)/region.stride;
      for (mt_index_t s = region.start - 1; s < region.end; s+=region.stride)
      {
          n_sites += wgt[s];
      }
   }

  assert(n_patterns > 0);
  assert(n_sites >= n_patterns);

  if (!compute_empirical_frequencies( false ))
      throw EXCEPTION_PARTITION_EMP_FREQS;

  if (descriptor.datatype == dt_dna)
  {
      if (!compute_empirical_subst_rates())
          throw EXCEPTION_PARTITION_EMP_RATES;
  }

  if (model_params & (MOD_PARAM_INV | MOD_PARAM_INV_GAMMA))
      if (!compute_empirical_pinv())
          throw EXCEPTION_PARTITION_EMP_PINV;

  build_models(descriptor,
               candidate_models,
               model_params,
               c_models,
               ckp_filename);
}

Partition::~Partition()
{
    for (size_t i=0; i<c_models.size(); i++)
        delete c_models[i];
}

const partition_descriptor_t Partition::get_descriptor( void ) const
{
    return descriptor;
}

data_type_t Partition::get_datatype(void) const
{
    return descriptor.datatype;
}

const std::string Partition::get_name( void ) const
{
    return descriptor.partition_name;
}

const partition_id_t Partition::get_id( void ) const
{
    return id;
}
static bool sort_forwards(Model * m1, Model * m2)
{
    /* sort by matrices */
    mt_size_t p1 = m1->get_matrix_index() * 10;
    mt_size_t p2 = m2->get_matrix_index() * 10;
    if (m1->is_F())
        p1 += 5;
    if (m2->is_F())
        p2 += 5;
        if (m1->is_G())
            p1 += 2;
        if (m2->is_G())
            p2 += 2;
        if (m1->is_I())
            p1 += 1;
        if (m2->is_I())
            p2 += 1;
    return p1 < p2;
}

static bool sort_backwards(Model * m1, Model * m2)
{
    mt_size_t p1 = m1->get_n_free_variables();
    mt_size_t p2 = m2->get_n_free_variables();
    if (m1->is_G())
        p1 += 20;
    if (m2->is_G())
        p2 += 20;
    if (m1->is_I())
        p1 += 10;
    if (m2->is_I())
        p2 += 10;

    return p1 > p2;
}

void Partition::sort_models(bool forwards)
{
    bool (*fsort)(Model *, Model *);
    if (forwards)
        fsort = &sort_forwards;
    else
        fsort = &sort_backwards;
    sort(c_models.begin(),
         c_models.end(),
         fsort);
}

mt_index_t Partition::get_unique_id( void ) const
{
  return descriptor.unique_id;
}

mt_size_t Partition::get_number_of_models( void ) const
{
    return (mt_size_t) c_models.size();
}

mt_size_t Partition::get_n_sites( void ) const
{
    return n_sites;
}

mt_size_t Partition::get_n_sequences( void ) const
{
  return msa.get_n_sequences();
}

mt_size_t Partition::get_n_patterns( void ) const
{
    return n_patterns;
}

std::vector<Model *> const& Partition::get_models(void) const
{
    return c_models;
}

Model * Partition::get_model(mt_index_t index) const
{
    return c_models[index];
}

Model * Partition::get_model_by_matrix(mt_index_t matrix_id,
                                       mt_mask_t model_params) const
{
  for (Model * model : c_models)
  {
    if (model->get_matrix_index() == matrix_id &&
        model->get_model_params() == model_params)
      return model;
  }
  return NULL;
}

bool Partition::set_models(const std::vector<Model *> &models)
{
    /* validate */
    if (c_models.size() != models.size())
        return false;
    for (size_t i=0; i<models.size(); i++)
        if (models[i]->get_name().compare(c_models[i]->get_name()))
            return false;
    for (size_t i=0; i<models.size(); i++)
    {
        c_models[i]->clone(models[i]);
    }
    return true;
}

const char * Partition::get_sequence(mt_index_t idx) const
{
  const char * msa_seq = msa.get_sequence (idx);

  /* the number of regions and stride should be reduced to 1 during site reordering */
  assert(descriptor.regions.size() == 1 || descriptor.regions[0].stride == 1);

  return msa_seq + descriptor.regions[0].start - 1;
}

const mt_size_t * Partition::get_weights( void ) const
{
  //mt_size_t *wgt = new mt_size_t[n_patterns];
  const mt_size_t * msa_wgt = msa.get_weights();

  /* the number of regions and stride should be reduced to 1 during site reordering */
  assert(descriptor.regions.size() == 1 || descriptor.regions[0].stride == 1);
  return msa_wgt + descriptor.regions[0].start - 1;
}

vector<double> const& Partition::get_empirical_frequencies( void ) const
{
    return emp_freqs;
}

vector<double> const& Partition::get_empirical_subst_rates( void ) const
{
    return emp_subst_rates;
}

double Partition::get_empirical_pinv( void ) const
{
    return emp_pinv;
}

vector<Model *> Partition::update_model_set(DnaModel & model)
{
  vector<Model *> new_models;
  vector<mt_index_t> new_matrices;

  const int * symmetries = model.get_symmetries();
  mt_size_t n_subst_params = model.get_n_subst_params() + 1;

  new_matrices.reserve((n_subst_params*(n_subst_params+1))/2);

  for (int i=0; i<(int)n_subst_params; ++i)
  {
    for (int j=(i+1); j<(int)n_subst_params; ++j)
    {
      /* merge rates 'i' and 'j' */
      /* foreach rate 'r'>j, 'r'--*/
      int new_matrix[N_DNA_SUBST_RATES];

      /* build symmetries set */
      for (int k=0; k<N_DNA_SUBST_RATES; ++k)
      {
        new_matrix[k] = symmetries[k];
        if (new_matrix[k] == j)
          new_matrix[k] = i;
        else if (new_matrix[k] > j)
          --new_matrix[k];
      }

      /* add model */
      mt_index_t matrix_index = DnaModel::get_index_for_matrix(new_matrix);
      new_matrices.push_back(matrix_index);

      // mt_index_t _matrix_index,
      // mt_mask_t model_params,
      // const partition_descriptor_t &partition,
      // asc_bias_t asc_bias_corr,
      // const mt_size_t *asc_w
    }
  }

  build_models(descriptor,
               new_matrices,
               model_params,
               new_models,
               ckp_filename);

  c_models.reserve(c_models.size() + new_models.size());
  c_models.insert(c_models.end(), new_models.begin(), new_models.end());

  return new_models;
}

void Partition::output_log(std::ostream  &out) const
{
  UNUSED(out);
  assert(0);
}

void Partition::input_log(std::istream  &in)
{
  UNUSED(in);
  assert(0);
}

int Partition::output_bin(string const& bin_filename) const
{
  assert(ROOT);

  //TODO
  UNUSED(bin_filename);
  return false;
}

int Partition::input_bin(string const& bin_filename)
{
  assert(ROOT);

  //TODO
  UNUSED(bin_filename);
  return false;
}

/* private methods */

bool Partition::compute_empirical_frequencies(bool smooth)
{
    const mt_size_t * weights = msa.get_weights();
    mt_size_t states = descriptor.states;
    assert (states);

    for (mt_index_t i=0; i<states; i++)
        emp_freqs[i] = 0;

    mt_size_t cum_weights = n_sites;
    mt_size_t cum_abs_freq = 0;

    uint32_t existing_states = 0;
    const unsigned int * states_map = (descriptor.datatype == dt_dna)?pll_map_nt:pll_map_aa;
    for (mt_index_t i=0; i<msa.get_n_sequences(); ++i)
    {
        for (partition_region_t region : descriptor.regions)
        {
            for (mt_index_t j = region.start - 1; j < region.end; ++j)
            {
                mt_size_t sum_site = 0;
                mt_size_t ind = states_map[(int)msa.get_sequence(i)[j]];
                if (!ind)
                {
                    mt_errno = MT_ERROR_FREQUENCIES;
                    snprintf(mt_errmsg, ERR_MSG_SIZE, "MSA does not match the data type [%s]. State %d", descriptor.partition_name.c_str(), (int) msa.get_sequence(i)[j]);
                    return false;
                }
                for (unsigned int k=0; k<states; ++k)
                {
                    sum_site += ((ind & (1<<k)) > 0);
                }
                for (unsigned int k=0; k<states; ++k)
                    emp_freqs[k] += 1.0 * weights[j] * ((ind & (1<<k))>0) / sum_site;
                if (sum_site == 1)
                    existing_states |= ind;
            }
        }
    }
    // return false;

    for (mt_index_t i=0; i<states; i++)
    {
        cum_abs_freq += emp_freqs[i];
        emp_freqs[i] /= msa.get_n_sequences() * cum_weights;
    }

    /* validate */
    double checksum = 0.0;
    for (mt_index_t i=0; i<states; ++i)
    {
        checksum += emp_freqs[i];
    }

    if( (checksum != checksum) || (fabs(1-checksum) > 1e-10 ))
    {
        mt_errno = MT_ERROR_FREQUENCIES;
        snprintf(mt_errmsg, ERR_MSG_SIZE, "Empirical frequencies sum to %.4f instead of 1 [%s]", checksum, descriptor.partition_name.c_str());
        return false;
    }

    /* check missing states */
    mt_size_t missing_states = states - Utils::count_bits(existing_states);
    if (missing_states)
    {
        if (smooth)
        {
            LOG_WARN << "WARNING: Forced freq. smoothing" << endl;
            for (mt_index_t i=0; i<states; i++)
                emp_freqs[i] /= checksum + MT_MIN_SMOOTH_FREQ * missing_states;
        }
        else
        {
            mt_errno = MT_ERROR_FREQUENCIES;
            snprintf(mt_errmsg, ERR_MSG_SIZE, "There are [%d] missing states [%s]", missing_states, descriptor.partition_name.c_str());
            return false;
        }
    }

    mt_errno = 0;
    return true;
}

bool Partition::compute_empirical_subst_rates( void )
{
    mt_size_t states = descriptor.states;
    assert (states);

    const unsigned int * states_map = (descriptor.datatype == dt_dna)?pll_map_nt:pll_map_aa;
    const mt_size_t * weights = msa.get_weights();

    for (mt_index_t i=0; i<N_DNA_SUBST_RATES; ++i)
        emp_subst_rates[i] = 0;

    mt_index_t * pair_rates = new mt_index_t[states * states];
    mt_index_t * state_freq = new mt_index_t[states];

    memset (pair_rates , 0, sizeof(mt_index_t) * states * states);

    if (!(pair_rates && state_freq))
    {
        pll_errno = PLL_ERROR_MEM_ALLOC;
        snprintf (pll_errmsg, 200,
                  "Cannot allocate memory for empirical subst rates");
        if (pair_rates)
            delete[] pair_rates;
        if (state_freq)
            delete[] state_freq;
        return false;
    }

    mt_size_t undef_state = (unsigned int) (1<<states) - 1;
    for (mt_index_t n = 0; n < n_patterns; ++n)
    {
        memset (state_freq, 0, sizeof(mt_index_t) * states);
        for (mt_index_t i = 0; i < msa.get_n_sequences(); ++i)
        {
            mt_index_t state = states_map[(int)msa.get_sequence(i)[n]];
            if (state == undef_state)
                continue;
            for (mt_index_t k = 0; k < states; ++k)
            {
                if (state & 1)
                    state_freq[k]++;
                state >>= 1;
            }
        }

        for (mt_index_t i = 0; i < states; ++i)
        {
            if (state_freq[i] == 0)
                continue;
            for (mt_index_t j = i + 1; j < states; ++j)
            {
                pair_rates[i * states + j] += state_freq[i] * state_freq[j] * weights[n];
            }
        }
    }

    mt_index_t k = 0;
    double last_rate = pair_rates[(states - 2) * states + states - 1];
    if (last_rate < 1e-7)
        last_rate = 1;
    for (mt_index_t i = 0; i < states - 1; ++i)
    {
        for (mt_index_t j = i + 1; j < states; ++j)
        {
            emp_subst_rates[k] = pair_rates[i * states + j] / last_rate;
            if (emp_subst_rates[k] < 0.01)
                emp_subst_rates[k] = 0.01;
            if (emp_subst_rates[k] > 50.0)
                emp_subst_rates[k] = 50.0;
            ++k;
        }
    }

    emp_subst_rates[k - 1] = 1.0;

    delete[] pair_rates;
    delete[] state_freq;

    return true;
}

bool Partition::compute_empirical_pinv( void )
{
    const mt_size_t * weights = msa.get_weights();
    mt_size_t n_inv = 0;
    double sum_wgt = 0.0;
    mt_size_t gap_state = descriptor.datatype == dt_protein?((1<<20)-1):15;

    const unsigned int * charmap = descriptor.datatype == dt_protein?pll_map_aa:pll_map_nt;
    for (partition_region_t region : descriptor.regions)
    {
        for (mt_index_t j = region.start - 1; j < region.end; j += region.stride)
        {
            sum_wgt += weights[j];
            n_inv   += weights[j];
            mt_size_t state = gap_state;
            for (mt_index_t i=0; i<msa.get_n_sequences(); ++i)
            {
              state &= charmap[(mt_index_t) msa.get_sequence(i)[j]];
              if (!state)
                break;
            }

            if (__builtin_popcount(state) != 1)
            {
              n_inv -= weights[j];
            }
        }
    }
    emp_pinv = (double)1.0*n_inv/sum_wgt;

    return true;
}

} /* modeltest */
