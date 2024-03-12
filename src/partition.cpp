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
#include "thread/parallel_context.h"

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
                  if (freq_params & MOD_PARAM_EMPIRICAL_FREQ)
                      c_models.push_back(
                                  new DnaModel(model_matrix,
                                    cur_rate_param | MOD_PARAM_EMPIRICAL_FREQ,
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
                                        cur_rate_param | MOD_PARAM_FIXED_FREQ | MOD_PARAM_MIXTURE,
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
                                        cur_rate_param | MOD_PARAM_FIXED_FREQ | MOD_PARAM_MIXTURE | MOD_PARAM_FREE_RATES,
                                        descriptor,
                                        asc_bias_corr,
                                        asc_weights)
                                );
                  }
                  else
                  {
                      if ((freq_params & MOD_PARAM_FIXED_FREQ) && model_matrix != PROT_GTR_INDEX)
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

  if (ckp_filename.compare("") && Utils::file_exists(ckp_filename))
  {
    LOG_DBG << "Attempting to load partition from checkpoint " << ckp_filename << endl;
    for (Model * model : c_models)
    {
      if (model->input_bin(ckp_filename))
      {
        LOG_DBG << "Loaded model from checkpoint: " << model->get_name() << endl;
      }
    }
  }

  return true;
}

Partition::Partition(partition_id_t _id,
                     Msa & _msa,
                     partition_descriptor_t _descriptor,
                     std::vector<mt_index_t> candidate_models,
                     mt_mask_t model_params,
                     string const& ckp_filename) :
    id(_id),
    msa(_msa),
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
      emp_subst_rates.resize(N_PROT_SUBST_RATES);
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

  for (mt_index_t i=0; i<_descriptor.states; i++)
     emp_freqs[i] = msa.get_stats(_descriptor.unique_id-1).freqs[i];

  for (mt_index_t i=0; i<emp_subst_rates.size(); i++)
     emp_subst_rates[i] = msa.get_stats(_descriptor.unique_id-1).subst_rates[i];

  if (model_params & (MOD_PARAM_INV | MOD_PARAM_INV_GAMMA))
  {
    emp_pinv = msa.get_stats(_descriptor.unique_id-1).inv_prop;
    if (emp_pinv < 0.002)
    {
      LOG_WARN << "Empirical invariant sites proportion for partition " << get_name() << " is " << emp_pinv << endl;
      LOG_WARN << "You should consider excluding +I and +I+G models from the analysis" << endl << endl;
    }
  }

  if (!build_models(descriptor,
               candidate_models,
               model_params,
               c_models,
               ckp_filename))
  {
    throw EXCEPTION_BUILD_MODELS;
  }
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

  if (!build_models(descriptor,
               new_matrices,
               model_params,
               new_models,
               ckp_filename))
 {
   throw EXCEPTION_BUILD_MODELS;
 }

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
  assert(ParallelContext::master());

  //TODO
  UNUSED(bin_filename);
  return false;
}

int Partition::input_bin(string const& bin_filename)
{
  assert(ParallelContext::master());

  //TODO
  UNUSED(bin_filename);
  return false;
}

} /* modeltest */
