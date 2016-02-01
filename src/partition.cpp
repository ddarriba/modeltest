#include "partition.h"

#include <algorithm>
#include <iostream>

using namespace std;

namespace modeltest
{

static bool build_models(data_type datatype,
                         std::vector<mt_index_t> candidate_models,
                         mt_mask_t model_params,
                         vector<Model *> &c_models)
{
    mt_size_t n_matrices = (mt_size_t) candidate_models.size();
    mt_size_t n_models = 0;

    int freq_params = model_params & MOD_MASK_FREQ_PARAMS;
    int rate_params = model_params & MOD_MASK_RATE_PARAMS;
    if (!freq_params)
    {
        mt_errno = MT_ERROR_MODELS;
         snprintf(mt_errmsg, 200, "Model frequencies is empty");
        return false;
    }

    int it_model_params = rate_params;
    while (it_model_params)
    {
        if (it_model_params & 1) n_models += n_matrices;
        it_model_params >>= 1;
    }

    if (freq_params == 3)
        n_models *= 2;
    c_models.reserve(n_models);

    for (int i=1; i<64; i*=2)
    {
        int cur_rate_param = rate_params & i;
        if (cur_rate_param)
        {
            for (mt_index_t j=0; j<n_matrices; j++)
            {
                if (datatype == dt_dna)
                {
                    if (freq_params & MOD_PARAM_FIXED_FREQ)
                        c_models.push_back(
                                    new DnaModel(candidate_models[j], cur_rate_param | MOD_PARAM_FIXED_FREQ)
                                    );
                    if (freq_params & MOD_PARAM_ESTIMATED_FREQ)
                        c_models.push_back(
                                    new DnaModel(candidate_models[j], cur_rate_param | MOD_PARAM_ESTIMATED_FREQ)
                                    );
                }
                else if (datatype == dt_protein)
                {
                    if (freq_params & MOD_PARAM_FIXED_FREQ)
                        c_models.push_back(
                                    new ProtModel(candidate_models[j], cur_rate_param | MOD_PARAM_FIXED_FREQ)
                                    );
                    if (freq_params & MOD_PARAM_ESTIMATED_FREQ)
                        c_models.push_back(
                                    new ProtModel(candidate_models[j], cur_rate_param | MOD_PARAM_ESTIMATED_FREQ)
                                    );
                }
                else
                    assert(0);
            }
        }
    }
    assert(c_models.size() == n_models);

    return true;
}

Partition::Partition(partition_id_t id,
                     Msa * _msa,
                     Tree * _tree,
                     partition_t _descriptor,
                     std::vector<mt_index_t> candidate_models,
                     mt_mask_t model_params) :
    id(id), msa(_msa), tree(_tree),
    descriptor(_descriptor)
{
    switch(descriptor.datatype)
    {
    case dt_dna:
        emp_freqs.resize(N_DNA_STATES);
        break;
    case dt_protein:
        emp_freqs.resize(N_PROT_STATES);
        break;
    }

    build_models(descriptor.datatype, candidate_models, model_params, c_models);
}

Partition::~Partition()
{
    for (size_t i=0; i<c_models.size(); i++)
        delete c_models[i];
}

const partition_t Partition::get_descriptor( void ) const
{
    return descriptor;
}

data_type Partition::get_datatype(void) const
{
    return descriptor.datatype;
}

const std::string Partition::get_name( void ) const
{
    return descriptor.partition_name;
}

const partition_id_t Partition::get_id( void ) const
{
    return id;
}
static bool sort_forwards(Model * m1, Model * m2)
{
    /* sort by complexity */
//    mt_size_t p1 = m1->get_n_free_variables();
//    mt_size_t p2 = m2->get_n_free_variables();
//    if (m1->is_G())
//        p1 += 20;
//    if (m2->is_G())
//        p2 += 20;
//    if (m1->is_I())
//        p1 += 10;
//    if (m2->is_I())
//        p2 += 10;

    /* sort by matrices */
    mt_size_t p1 = m1->get_matrix_index() * 10;
    mt_size_t p2 = m2->get_matrix_index() * 10;
    if (m1->is_F())
        p1 += 5;
    if (m2->is_F())
        p2 += 5;
        if (m1->is_G())
            p1 += 2;
        if (m2->is_G())
            p2 += 2;
        if (m1->is_I())
            p1 += 1;
        if (m2->is_I())
            p2 += 1;
    return p1 < p2;
}

static bool sort_backwards(Model * m1, Model * m2)
{
    mt_size_t p1 = m1->get_n_free_variables();
    mt_size_t p2 = m2->get_n_free_variables();
    if (m1->is_G())
        p1 += 20;
    if (m2->is_G())
        p2 += 20;
    if (m1->is_I())
        p1 += 10;
    if (m2->is_I())
        p2 += 10;

    return p1 > p2;
}

void Partition::sort_models(bool forwards)
{
    bool (*fsort)(Model *, Model *);
    if (forwards)
        fsort = &sort_forwards;
    else
        fsort = &sort_backwards;
    sort(c_models.begin(),
         c_models.end(),
         fsort);
}

mt_size_t Partition::get_number_of_models( void ) const
{
    return c_models.size();
}

std::vector<Model *> const& Partition::get_models() const
{
    return c_models;
}

Model * Partition::get_model(mt_index_t index) const
{
    return c_models[index];
}

bool Partition::set_models(const std::vector<Model *> &models)
{
    /* validate */
    if (c_models.size() != models.size())
        return false;
    for (size_t i=0; i<models.size(); i++)
        if (models[i]->get_name().compare(c_models[i]->get_name()))
            return false;
    for (size_t i=0; i<models.size(); i++)
    {
        c_models[i]->clone(models[i]);
    }
    return true;
}

} /* modeltest */
