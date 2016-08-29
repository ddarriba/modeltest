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

#include "partitioning_scheme.h"

namespace modeltest
{

PartitioningScheme::PartitioningScheme()
{
}

PartitioningScheme::~PartitioningScheme()
{
    partitions.clear();
}

bool PartitioningScheme::delete_partitions( void )
{
    if (!partitions.size())
        return false;

    for (partitions_map_t::iterator it=partitions.begin(); it!=partitions.end(); ++it)
        delete it->second;
    partitions.clear();

    return true;
}

mt_index_t PartitioningScheme::get_size( void ) const
{
    return (mt_index_t) partitions.size();
}

Partition & PartitioningScheme::get_partition( partition_id_t const& part_id )
{
    return *(partitions[part_id]);
}

Partition & PartitioningScheme::get_partition( mt_index_t id )
{
    return *(partitions[partition_ids[id]]);
}

bool PartitioningScheme::add_partition( partition_id_t const& partition_id,
                                        Partition * partition)
{
    //TODO: Check existence and overlapping
    partition_ids.push_back(partition_id);
    partitions[partition_id] = partition;

    return true;
}

} /* modeltest */
