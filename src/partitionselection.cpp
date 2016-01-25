#include "partitionselection.h"

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
