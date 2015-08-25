#ifndef PARTITION_H
#define PARTITION_H

#include "global_defs.h"
#include "model.h"
#include "msa.h"
#include "tree.h"

namespace modeltest
{

class Partition
{
public:
    Partition(Msa * _msa,
              Tree * _tree,
              partition_t _descriptor,
              std::vector<mt_index_t> candidate_models,
              mt_mask_t model_params);
    ~Partition();

    const partition_t get_descriptor( void ) const;

    void sort_models(bool forwards);
    std::vector<Model *> & get_models();
    bool set_models(const std::vector<Model *> &models);
private:
    Msa * msa;                       //! input MSA
    Tree * tree;                     //! user defined tree (optional)
    partition_t descriptor;          //! partition descriptor

    std::vector<Model *> c_models; //! candidate models
};

} /* modeltest */
#endif // PARTITION_H
