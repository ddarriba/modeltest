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

#include "partition_selection.h"

namespace modeltest
{

PartitionSelection::PartitionSelection(PartitioningScheme &scheme,
                                       ic_type type)
{
    best_models.resize(scheme.get_size());
    for (mt_index_t i=0; i<scheme.get_size(); ++i)
    {
        Partition partition = scheme.get_partition(i);
        ModelSelection selection(partition.get_models(), type);

        best_models[i] = selection.get_model(0);
        score += best_models[i].score;
        weight += best_models[i].weight;
    }
    weight /= scheme.get_size();
}

double PartitionSelection::get_score()
{
    return score;
}

double PartitionSelection::get_weight()
{
    return weight;
}

selection_model & PartitionSelection::get_model(mt_index_t part_idx)
{
    assert(part_idx < best_models.size());
    return best_models[part_idx];
}

}
