#ifndef PARTITIONING_SCHEME_H
#define PARTITIONING_SCHEME_H

#include "utils.h"
#include "partition.h"
#include "msa.h"
#include "tree.h"

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
