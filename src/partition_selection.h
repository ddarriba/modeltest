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

#ifndef PARTITIONSELECTION_H
#define PARTITIONSELECTION_H

#include "global_defs.h"
#include "partitioning_scheme.h"
#include "model_selection.h"

#include <vector>

namespace modeltest
{

class PartitionSelection
{
public:
    PartitionSelection(PartitioningScheme & scheme,
                       ic_type type);

    double get_score();
    double get_weight();
    selection_model & get_model(mt_index_t part_idx);
private:
    std::vector<selection_model> best_models;
    double score;
    double weight;
};

}
#endif // PARTITIONSELECTION_H
