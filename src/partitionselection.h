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
