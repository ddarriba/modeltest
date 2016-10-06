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

#ifndef CHECKPOINT_H_
#define CHECKPOINT_H_

#include "plldefs.h"
#include "model/model.h"

typedef struct
{
  mt_index_t unique_id;
  double loglh;
  double bic;
  double aic;
  double aicc;
  double dt;

  double frequencies[N_PROT_STATES];
  double subst_rates[N_DNA_SUBST_RATES];
  double prop_invar;
  double alpha;

  time_t exec_time;
} model_ckpdata_t;

namespace modeltest
{

class Checkpoint
{
public:
  Checkpoint(std::string const& ckp_filename);
  ~Checkpoint( void );

  bool is_enabled( void ) const;
  bool ckp_save_model(Model const& model);
  bool ckp_load_model(Model & model);
private:
  const std::string ckp_filename;
  bool enabled;
  mt_size_t seq_count;
};

}

#endif
