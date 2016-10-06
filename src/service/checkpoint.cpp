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
 Exelixis Lab, Heidelberg Instutute for Theoretical Studies
 Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
 */

#include "checkpoint.h"

#include "utils.h"
#include "genesis/logging.h"

using namespace std;

namespace modeltest
{

Checkpoint::Checkpoint(std::string const& ckp_filename) :
  ckp_filename(ckp_filename)
{
  FILE * bin_file;

  enabled = true;
  if (!Utils::file_exists(ckp_filename))
  {
    LOG_INFO << "Creating checkpoint file: " << ckp_filename << endl;
    pll_binary_header_t bin_header;
    bin_file = pllmod_binary_create(ckp_filename.c_str(),
                                    &bin_header,
                                    PLLMOD_BIN_ACCESS_RANDOM,
                                    2048);
    if (!bin_file)
    {
      LOG_ERR << "Cannot create binary file: " << ckp_filename << endl;
      enabled = false;
    }
    else
    {
      pllmod_binary_close(bin_file);
    }
  }
}

Checkpoint::~Checkpoint( void )
{
    /* do nothing */
}

bool Checkpoint::is_enabled( void ) const
{
  return enabled;
}

bool Checkpoint::ckp_save_model(Model const& model)
{
  model_ckpdata_t ckp_data;
  pll_binary_header_t input_header;
  const double * subst_rates = model.get_subst_rates();
  const double * frequencies = model.get_frequencies();
  pll_utree_t * tree = model.get_tree();
  unsigned int unique_id = model.get_unique_id();
  int write_ok;
  FILE * bin_file;

  if (!enabled)
    return false;

  bin_file = pllmod_binary_append_open(ckp_filename.c_str(), &input_header);
  if (!bin_file)
    return false;

  ckp_data.loglh = model.get_loglh();
  ckp_data.bic = model.get_bic();
  ckp_data.aic = model.get_aic();
  ckp_data.aicc = model.get_aicc();
  ckp_data.dt = model.get_dt();

  memset(ckp_data.frequencies, 0, N_PROT_STATES * sizeof(double));
  memset(ckp_data.subst_rates, 0, N_DNA_SUBST_RATES * sizeof(double));
  memcpy(ckp_data.frequencies,
         frequencies,
         model.get_n_states() * sizeof(double));

  if (model.get_datatype() == dt_dna)
  {
    memcpy(ckp_data.subst_rates,
           subst_rates,
           N_DNA_SUBST_RATES * sizeof(double));
  }

  ckp_data.prop_invar = model.get_prop_inv();
  ckp_data.alpha = model.get_alpha();
  ckp_data.exec_time = model.get_exec_time();

  assert(bin_file);
  write_ok = pllmod_binary_custom_dump(bin_file,
                            unique_id,
                            &ckp_data,
                            sizeof(model_ckpdata_t),
                            PLLMOD_BIN_ATTRIB_UPDATE_MAP);

  assert(tree && !tree->next && tree->back->next);
  write_ok &= pllmod_binary_utree_dump(bin_file,
                                       2000+unique_id,
                                       tree->back,
                                       seq_count,
                                       PLLMOD_BIN_ATTRIB_UPDATE_MAP);

  pllmod_binary_close(bin_file);

  if(!write_ok)
  {
    mt_errno = pll_errno;
    snprintf(mt_errmsg, ERR_MSG_SIZE, "Error dumping checkpoint");
  }

  return write_ok;
  return true;
}

bool Checkpoint::ckp_load_model(Model & model)
{
  model_ckpdata_t * ckp_data;
  pll_binary_header_t input_header;
  size_t size = 0;
  unsigned int type, attributes;
  int read_ok;
  FILE * bin_file;
  pll_utree_t * tree;
  mt_index_t unique_id = model.get_unique_id();

  bin_file = pllmod_binary_open(ckp_filename.c_str(), &input_header);
  if (!bin_file)
  {
    mt_errno = pll_errno;
    snprintf(mt_errmsg, ERR_MSG_SIZE, "Error opening file: %d %s",
             pll_errno, pll_errmsg);
    return false;
  }

  ckp_data = (model_ckpdata_t *) pllmod_binary_custom_load(bin_file,
                                             unique_id,
                                             &size,
                                             &type,
                                             &attributes,
                                             PLLMOD_BIN_ACCESS_SEEK);
  read_ok = (ckp_data != NULL);

  if (read_ok)
  {
    assert(ckp_data->unique_id == unique_id);
    model.set_loglh(ckp_data->loglh);
    model.set_bic(ckp_data->bic);
    model.set_aic(ckp_data->aic);
    model.set_aicc(ckp_data->aicc);
    model.set_dt(ckp_data->dt);
    model.set_exec_time(ckp_data->exec_time);

    model.set_frequencies(ckp_data->frequencies);

    if (model.get_datatype() == dt_dna)
    {
      model.set_subst_rates(ckp_data->subst_rates);
    }

    if (model.is_I())
      model.set_prop_inv(ckp_data->prop_invar);
    if (model.is_G())
      model.set_alpha(ckp_data->alpha);

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
      model.set_tree(tree->back);
  }
  else
  {
    mt_errno = pll_errno;
    snprintf(mt_errmsg, ERR_MSG_SIZE, "%s", pll_errmsg);
  }

  pllmod_binary_close(bin_file);

  return read_ok;
}

}
