#include "partition.h"
#include "utils.h"

#include <algorithm>
#include <iostream>
#include <sstream>

using namespace std;

namespace modeltest
{



static bool build_models(const partition_descriptor_t &descriptor,
                         std::vector<mt_index_t> candidate_models,
                         mt_mask_t model_params,
                         vector<Model *> &c_models)
{
    data_type_t datatype = descriptor.datatype;
    asc_bias_t asc_bias_corr = descriptor.asc_bias_corr;
    const mt_size_t *asc_weights = descriptor.asc_weights;

    mt_size_t n_matrices = (mt_size_t) candidate_models.size();
    mt_size_t n_models = 0;

    mt_mask_t freq_params = model_params & MOD_MASK_FREQ_PARAMS;
    mt_mask_t rate_params = model_params & MOD_MASK_RATE_PARAMS;

    n_models = Utils::number_of_models(n_matrices, model_params);

    if (!n_models)
    {
        mt_errno = MT_ERROR_MODELS;
         snprintf(mt_errmsg, ERR_MSG_SIZE, "Candidate models set is empty");
        return false;
    }

    c_models.reserve(n_models);

    for (mt_index_t i=MOD_PARAM_MIN_RPARAM; i<=MOD_PARAM_MAX_RPARAM; i<<=1)
    {
        mt_mask_t cur_rate_param = rate_params & i;
        if (cur_rate_param)
        {
            for (mt_index_t model_matrix : candidate_models)
            {
                if (datatype == dt_dna)
                {
                    if (freq_params & MOD_PARAM_FIXED_FREQ)
                        c_models.push_back(
                                    new DnaModel(model_matrix,
                                      cur_rate_param | MOD_PARAM_FIXED_FREQ,
                                      descriptor,
                                      asc_bias_corr,
                                      asc_weights)
                                    );
                    if (freq_params & MOD_PARAM_ESTIMATED_FREQ)
                        c_models.push_back(
                                    new DnaModel(model_matrix,
                                      cur_rate_param | MOD_PARAM_ESTIMATED_FREQ,
                                      descriptor,
                                      asc_bias_corr,
                                      asc_weights)
                                    );
                }
                else if (datatype == dt_protein)
                {
                    if (model_matrix == LG4M_INDEX)
                    {
                      if (cur_rate_param & (MOD_PARAM_GAMMA | MOD_PARAM_INV_GAMMA))
                          c_models.push_back(
                              new ProtModel(LG4M_INDEX,
                                          cur_rate_param | MOD_PARAM_FIXED_FREQ,
                                          descriptor,
                                          asc_bias_corr,
                                          asc_weights)
                              );
                    }
                    else if (model_matrix == LG4X_INDEX)
                    {
                      if (cur_rate_param & (MOD_PARAM_GAMMA | MOD_PARAM_INV_GAMMA))
                          c_models.push_back(
                                  new ProtModel(LG4X_INDEX,
                                          cur_rate_param | MOD_PARAM_FIXED_FREQ,
                                          descriptor,
                                          asc_bias_corr,
                                          asc_weights)
                                  );
                    }
                    else
                    {
                        if (freq_params & MOD_PARAM_FIXED_FREQ)
                            c_models.push_back(
                                        new ProtModel(model_matrix,
                                                      cur_rate_param | MOD_PARAM_FIXED_FREQ,
                                                      descriptor,
                                                      asc_bias_corr,
                                                      asc_weights)
                                        );
                        if (freq_params & MOD_PARAM_EMPIRICAL_FREQ)
                            c_models.push_back(
                                        new ProtModel(model_matrix,
                                                      cur_rate_param | MOD_PARAM_EMPIRICAL_FREQ,
                                                      descriptor,
                                                      asc_bias_corr,
                                                      asc_weights)
                                        );
                    }
                }
                else
                    assert(0);
            }
        }
    }
    assert(datatype == dt_protein || (c_models.size() == n_models));
    return true;
}

Partition::Partition(partition_id_t _id,
                     Msa & _msa,
                     Tree & _tree,
                     partition_descriptor_t _descriptor,
                     std::vector<mt_index_t> candidate_models,
                     mt_mask_t model_params) :
    id(_id), msa(_msa), tree(_tree),
    descriptor(_descriptor)
{
    switch(descriptor.datatype)
    {
    case dt_dna:
        emp_freqs.resize(N_DNA_STATES);
        emp_subst_rates.resize(N_DNA_SUBST_RATES);
        break;
    case dt_protein:
        emp_freqs.resize(N_PROT_STATES);
        break;
    }

    n_patterns = 0;
    n_sites = 0;
    const unsigned int * wgt = msa.get_weights();
    for (const partition_region_t & region : descriptor.regions)
    {
        n_patterns += (region.end - region.start + 1)/region.stride;
        for (mt_index_t s = region.start - 1; s < region.end; s+=region.stride)
        {
            n_sites += wgt[s];
        }
     }

    assert(n_patterns > 0);
    assert(n_sites >= n_patterns);

    if (!compute_empirical_frequencies( false ))
        throw EXCEPTION_PARTITION_EMP_FREQS;

    if (descriptor.datatype == dt_dna)
    {
        if (!compute_empirical_subst_rates())
            throw EXCEPTION_PARTITION_EMP_RATES;
    }

    if (model_params & (MOD_PARAM_INV | MOD_PARAM_INV_GAMMA))
        if (!compute_empirical_pinv())
            throw EXCEPTION_PARTITION_EMP_PINV;

    build_models(descriptor, candidate_models, model_params, c_models);
}

Partition::~Partition()
{
    for (size_t i=0; i<c_models.size(); i++)
        delete c_models[i];
}

const partition_descriptor_t Partition::get_descriptor( void ) const
{
    return descriptor;
}

data_type_t Partition::get_datatype(void) const
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
    return (mt_size_t) c_models.size();
}

mt_size_t Partition::get_n_sites( void ) const
{
    return n_sites;
}

mt_size_t Partition::get_n_sequences( void ) const
{
  return msa.get_n_sequences();
}

mt_size_t Partition::get_n_patterns( void ) const
{
    return n_patterns;
}

std::vector<Model *> const& Partition::get_models(void) const
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

const char * Partition::get_sequence(mt_index_t idx) const
{
    char *seq = new char[n_patterns];
    const char * msa_seq = msa.get_sequence (idx);

    if (descriptor.regions.size() > 1 || descriptor.regions[0].stride > 1)
    {
        /* complex partition */
        char *seq_ptr = seq;
        for (const partition_region_t & region : descriptor.regions)
        {
            mt_index_t region_start  = region.start-1;
            mt_index_t region_end    = region.end;
            mt_index_t region_stride = region.stride;
            mt_size_t region_patterns   = (region_end - region_start)/region_stride;
            if (region_stride == 1)
            {
                /* copy consecutive sites in sequence */
                memcpy(seq_ptr, msa_seq + region_start, region_patterns * sizeof(char));
            }
            else
            {
                /* copy strided sites in sequence */
                mt_index_t k = 0;
                for (mt_index_t s = region_start; s < region_end; s+=region.stride)
                {
                    seq_ptr[k++] = msa_seq[s];
                }
            }
            seq_ptr += region_patterns;
        }
    }
    else
    {
        /* simple partition */
        memcpy(seq, msa_seq + descriptor.regions[0].start - 1, n_patterns * sizeof(char));
    }

    return seq;
}

const mt_size_t * Partition::get_weights( void ) const
{
    mt_size_t *wgt = new mt_size_t[n_patterns];
    const mt_size_t * msa_wgt = msa.get_weights();

    if (descriptor.regions.size() > 1 || descriptor.regions[0].stride > 1)
    {
        /* complex partition */
        mt_size_t *wgt_ptr = wgt;
        for (const partition_region_t & region : descriptor.regions)
        {
            mt_index_t region_start  = region.start-1;
            mt_index_t region_end    = region.end;
            mt_index_t region_stride = region.stride;
            mt_size_t region_patterns   = (region_end - region_start)/region_stride;
            if (region_stride == 1)
            {
                /* copy consecutive sites in sequence */
                memcpy(wgt_ptr, msa_wgt + region_start, region_patterns * sizeof(mt_size_t));
            }
            else
            {
                /* copy strided sites in sequence */
                mt_index_t k = 0;
                for (mt_index_t s = region_start; s < region_end; s+=region.stride)
                {
                    wgt_ptr[k++] = msa_wgt[s];
                }
            }
            wgt_ptr += region_patterns;
        }
    }
    else
    {
        /* simple partition */
        memcpy(wgt, msa_wgt + descriptor.regions[0].start - 1, n_patterns * sizeof(mt_size_t));
    }

    return wgt;
}

vector<double> const& Partition::get_empirical_frequencies( void ) const
{
    return emp_freqs;
}

vector<double> const& Partition::get_empirical_subst_rates( void ) const
{
    return emp_subst_rates;
}

double Partition::get_empirical_pinv( void ) const
{
    return emp_pinv;
}

void Partition::output_log(std::ostream  &out)
{
    UNUSED(out);
    assert(0);
}

void Partition::input_log(std::istream  &in)
{
    UNUSED(in);
    assert(0);
}

/* private methods */

bool Partition::compute_empirical_frequencies(bool smooth)
{
    const mt_size_t * weights = msa.get_weights();
    mt_size_t states = descriptor.states;
    assert (states);

    for (mt_index_t i=0; i<states; i++)
        emp_freqs[i] = 0;

    mt_size_t cum_weights = n_sites;
    mt_size_t cum_abs_freq = 0;

    uint32_t existing_states = 0;
    const unsigned int * states_map = (descriptor.datatype == dt_dna)?pll_map_nt:pll_map_aa;
    for (mt_index_t i=0; i<msa.get_n_sequences(); ++i)
    {
        for (partition_region_t region : descriptor.regions)
        {
            for (mt_index_t j = region.start - 1; j < region.end; ++j)
            {
                mt_size_t sum_site = 0;
                mt_size_t ind = states_map[(int)msa.get_sequence(i)[j]];
                if (!ind)
                {
                    mt_errno = MT_ERROR_FREQUENCIES;
                    snprintf(mt_errmsg, ERR_MSG_SIZE, "MSA does not match the data type [%s]. State %d", descriptor.partition_name.c_str(), (int) msa.get_sequence(i)[j]);
                    return false;
                }
                for (unsigned int k=0; k<states; ++k)
                {
                    sum_site += ((ind & (1<<k)) > 0);
                }
                for (unsigned int k=0; k<states; ++k)
                    emp_freqs[k] += 1.0 * weights[j] * ((ind & (1<<k))>0) / sum_site;
                if (sum_site == 1)
                    existing_states |= ind;
            }
        }
    }

    for (mt_index_t i=0; i<states; i++)
    {
        cum_abs_freq += emp_freqs[i];
        emp_freqs[i] /= msa.get_n_sequences() * cum_weights;
    }

    /* validate */
    double checksum = 0.0;
    for (mt_index_t i=0; i<states; ++i)
    {
        checksum += emp_freqs[i];
    }

    if( (checksum != checksum) || (fabs(1-checksum) > 1e-10 ))
    {
        mt_errno = MT_ERROR_FREQUENCIES;
        snprintf(mt_errmsg, ERR_MSG_SIZE, "Empirical frequencies sum to %.4f instead of 1 [%s]", checksum, descriptor.partition_name.c_str());
        return false;
    }

    /* check missing states */
    mt_size_t missing_states = states - Utils::count_bits(existing_states);
    if (missing_states)
    {
        if (smooth)
        {
            std::cerr << "WARNING: Forced freq. smoothing" << std::endl;
            for (mt_index_t i=0; i<states; i++)
                emp_freqs[i] /= checksum + MT_MIN_SMOOTH_FREQ * missing_states;
        }
        else
        {
            mt_errno = MT_ERROR_FREQUENCIES;
            snprintf(mt_errmsg, ERR_MSG_SIZE, "There are [%d] missing states [%s]", missing_states, descriptor.partition_name.c_str());
            return false;
        }
    }

    mt_errno = 0;
    return true;
}

bool Partition::compute_empirical_subst_rates( void )
{
    mt_size_t states = descriptor.states;
    assert (states);

    const unsigned int * states_map = (descriptor.datatype == dt_dna)?pll_map_nt:pll_map_aa;
    const mt_size_t * weights = msa.get_weights();

    for (mt_index_t i=0; i<N_DNA_SUBST_RATES; ++i)
        emp_subst_rates[i] = 0;

    mt_index_t * pair_rates = new mt_index_t[states * states];
    mt_index_t * state_freq = new mt_index_t[states];

    memset (pair_rates , 0, sizeof(mt_index_t) * states * states);

    if (!(pair_rates && state_freq))
    {
        pll_errno = PLL_ERROR_MEM_ALLOC;
        snprintf (pll_errmsg, 200,
                  "Cannot allocate memory for empirical subst rates");
        if (pair_rates)
            delete[] pair_rates;
        if (state_freq)
            delete[] state_freq;
        return false;
    }

    mt_size_t undef_state = (unsigned int) (1<<states) - 1;
    for (mt_index_t n = 0; n < n_patterns; ++n)
    {
        memset (state_freq, 0, sizeof(mt_index_t) * states);
        for (mt_index_t i = 0; i < msa.get_n_sequences(); ++i)
        {
            mt_index_t state = states_map[(int)msa.get_sequence(i)[n]];
            if (state == undef_state)
                continue;
            for (mt_index_t k = 0; k < states; ++k)
            {
                if (state & 1)
                    state_freq[k]++;
                state >>= 1;
            }
        }

        for (mt_index_t i = 0; i < states; ++i)
        {
            if (state_freq[i] == 0)
                continue;
            for (mt_index_t j = i + 1; j < states; ++j)
            {
                pair_rates[i * states + j] += state_freq[i] * state_freq[j] * weights[n];
            }
        }
    }

    mt_index_t k = 0;
    double last_rate = pair_rates[(states - 2) * states + states - 1];
    if (last_rate < 1e-7)
        last_rate = 1;
    for (mt_index_t i = 0; i < states - 1; ++i)
    {
        for (mt_index_t j = i + 1; j < states; ++j)
        {
            emp_subst_rates[k] = pair_rates[i * states + j] / last_rate;
            if (emp_subst_rates[k] < 0.01)
                emp_subst_rates[k] = 0.01;
            if (emp_subst_rates[k] > 50.0)
                emp_subst_rates[k] = 50.0;
            ++k;
        }
    }

    emp_subst_rates[k - 1] = 1.0;

    delete[] pair_rates;
    delete[] state_freq;

    return true;
}

bool Partition::compute_empirical_pinv( void )
{
    const mt_size_t * weights = msa.get_weights();
    const char * sequence0 = msa.get_sequence(0);
    mt_size_t n_inv = 0;
    double sum_wgt = 0.0;
    for (partition_region_t region : descriptor.regions)
    {
        for (mt_index_t j = region.start - 1; j < region.end; j += region.stride)
        {
            sum_wgt += weights[j];
            n_inv   += weights[j];
            char state = sequence0[j];
            for (mt_index_t i=1; i<msa.get_n_sequences(); ++i)
            {
                if (msa.get_sequence(i)[j] != state)
                {
                    n_inv -= weights[j];
                    break;
                }
            }
        }
    }
    emp_pinv = (double)1.0*n_inv/sum_wgt;
    return true;
}

} /* modeltest */
