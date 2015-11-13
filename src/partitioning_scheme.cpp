#include "partitioning_scheme.h"

namespace modeltest
{

PartitioningScheme::PartitioningScheme(const partitioning_scheme_t &scheme_def,
                                       Msa * msa,
                                       Tree * tree,
                                       std::vector<mt_index_t> candidate_models,
                                       mt_mask_t model_params)
{
    mt_index_t cur_part_id = 0;
    partition_ids.resize(scheme_def.size());
    for (const partition_t & partition : scheme_def)
    {
        partition_id_t part_id(1);
        part_id[0] = cur_part_id;
        Partition * new_part;
        new_part = new Partition(msa,
                                 tree,
                                 partition,
                                 candidate_models,
                                 model_params);

        /*
         * sort candidate models by
         * estimated computational needs
         */
        new_part->sort_models(false);

        partitions[part_id] = new_part;
        partition_ids[cur_part_id] = part_id;
        cur_part_id++;
    }
}

PartitioningScheme::~PartitioningScheme()
{
    for (partitions_map_t::iterator it=partitions.begin(); it!=partitions.end(); ++it)
        delete it->second;
    partitions.clear();
}

mt_index_t PartitioningScheme::get_size( void ) const
{
    return (mt_index_t) partitions.size();
}

Partition & PartitioningScheme::get_partition( mt_index_t id )
{
    return *(partitions[partition_ids[id]]);
}

} /* modeltest */
