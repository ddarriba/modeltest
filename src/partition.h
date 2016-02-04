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
    Partition(partition_id_t id,
              Msa * _msa,
              Tree * _tree,
              partition_descriptor_t _descriptor,
              std::vector<mt_index_t> candidate_models,
              mt_mask_t model_params);
    ~Partition();

    const partition_descriptor_t get_descriptor( void ) const;
    const std::string get_name( void ) const;

    void sort_models(bool forwards);

    mt_size_t get_number_of_models( void ) const;
    std::vector<Model *> const& get_models() const;
    Model * get_model(mt_index_t index) const;
    data_type_t get_datatype( void ) const;
    const partition_id_t get_id( void ) const;

    bool set_models(const std::vector<Model *> &models);
private:
    partition_id_t id;               //! id of the partition
    Msa * msa;                       //! input MSA
    Tree * tree;                     //! user defined tree (optional)
    partition_descriptor_t descriptor;          //! partition descriptor
    std::vector<double> emp_freqs;   //! empirical frequencies

    std::vector<Model *> c_models; //! candidate models
};

} /* modeltest */
#endif // PARTITION_H
