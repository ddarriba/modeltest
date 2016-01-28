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
