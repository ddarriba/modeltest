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

#ifndef PARTITIONING_SCHEME_H
#define PARTITIONING_SCHEME_H

#include "msa.h"
#include "tree.h"
#include "utils.h"
#include "partition.h"

#include <map>
#include <vector>

namespace modeltest
{

typedef std::map<partition_id_t, Partition *> partitions_map_t;

class PartitioningScheme
{
public:
    PartitioningScheme();
    ~PartitioningScheme();

    bool add_partition(const partition_id_t &partition_id,
                       Partition * partition);

    bool delete_partitions( void );

    mt_index_t get_size( void ) const;

    Partition & get_partition( partition_id_t const& part_id );
    Partition & get_partition( mt_index_t id );
private:
    partitions_map_t partitions;
    std::vector<partition_id_t> partition_ids;
};

} /* modeltest */
#endif // PARTITIONING_SCHEME_H
