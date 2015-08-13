#include "model.h"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <cmath>

using namespace std;

namespace modeltest {

static void set_subst_params(string const& matrix, int * m_ind)
 {
    for (int i=0; i<N_SUBST_RATES; i++)
      m_ind[i] = (int)(matrix.at(i) - '0');
 }

Model::Model(mt_index_t matrix_index,
             int model_params,
             string const& matrix)
    : optimize_pinv(model_params & (MOD_PARAM_INV | MOD_PARAM_INV_GAMMA)),
      optimize_gamma(model_params & (MOD_PARAM_GAMMA | MOD_PARAM_INV_GAMMA)),
      optimize_freqs(model_params & MOD_PARAM_ML_FREQ)
{
    stringstream ss_name;

    set_subst_params(model_matrices[matrix_index], matrix_symmetries);

    mt_index_t standard_matrix_index = find(model_matrices_ind,
                                     model_matrices_ind + N_MODEL_MATRICES,
                                     matrix_index) - model_matrices_ind;
    if (standard_matrix_index < N_MODEL_MATRICES)
    {
        ss_name << model_names[2 * standard_matrix_index + (optimize_freqs?1:0)];
    }
    else
    {
        ss_name << model_matrices[matrix_index];
        if (optimize_freqs)
            ss_name << "[F]";
    }

    for (mt_index_t i=0; i<N_STATES; i++)
      frequencies[i] = 1.0/N_STATES;
    for (mt_index_t i=0; i<N_SUBST_RATES; i++)
      subst_rates[i] = 1.0;

    prop_inv = 0.0;
    alpha = 0.0;

    lnL  = 0.0;
    bic  = 0.0;
    aic  = 0.0;
    aicc = 0.0;
    dt   = 0.0;

    if (optimize_pinv)
        ss_name << "+I";
    if (optimize_gamma)
        ss_name << "+G";
    name = ss_name.str();

    n_free_variables = get_n_subst_params();
    if (optimize_freqs)
        n_free_variables += N_STATES-1;
    if (optimize_pinv)
        n_free_variables ++;
    if (optimize_gamma)
        n_free_variables ++;
}

Model::Model(const Model & other)
{
    clone(&other);
}

void Model::clone(const Model * other)
{
    name = other->name;
    memcpy(matrix_symmetries, other->matrix_symmetries, N_SUBST_RATES * sizeof(int));
    optimize_pinv  = other->optimize_pinv;
    optimize_gamma = other->optimize_gamma;
    optimize_freqs = other->optimize_freqs;

    prop_inv = other->prop_inv;
    alpha    = other->alpha;
    memcpy(frequencies, other->frequencies, N_STATES * sizeof(double));
    memcpy(subst_rates, other->subst_rates, N_SUBST_RATES * sizeof(double));

    n_free_variables = other->n_free_variables;

    lnL  = other->lnL;
    bic  = other->bic;
    aic  = other->aic;
    aicc = other->aicc;
    dt   = other->dt;

    exec_time = other->exec_time;
}

std::string const& Model::get_name() const
{
    return name;
}

bool Model::is_optimized() const
{
    return lnL < 0.0;
}

bool Model::is_I() const
{
    return optimize_pinv;
}

bool Model::is_G() const
{
    return optimize_gamma;
}

bool Model::is_F() const
{
    return optimize_freqs;
}

const int * Model::get_symmetries( void ) const
{
    return matrix_symmetries;
}

mt_size_t Model::get_n_free_variables() const
{
    return n_free_variables;
}

double Model::get_lnl() const
{
    return lnL;
}

void Model::set_lnl( double l )
{
    lnL = l;
}

mt_size_t Model::get_n_subst_params() const
{
    return *max_element(matrix_symmetries, matrix_symmetries+N_SUBST_RATES);
}

double Model::get_prop_inv() const
{
    return prop_inv;
}

void Model::set_prop_inv(double value)
{
    prop_inv = value;
}
double Model::get_alpha() const
{
    return alpha;
}

void Model::set_alpha(double value)
{
    alpha = value;
}

const double * Model::get_frequencies( void ) const
{
    return frequencies;
}

void Model::set_frequencies(const double value[])
{
    memcpy(frequencies, value, N_STATES * sizeof(double));
}

const double * Model::get_subst_rates( void ) const
{
    return subst_rates;
}

void Model::set_subst_rates(const double value[], bool full_vector)
{
    int n_subst_params = get_n_subst_params();
    if (full_vector)
    {
        /*TODO: validate symmetries */
        memcpy(subst_rates, value, N_SUBST_RATES * sizeof(double));
    }
    else
    {
        for (int i=0; i<N_SUBST_RATES; i++)
        {
            if (matrix_symmetries[i] == n_subst_params)
            {
                subst_rates[i] = 1.0;
            }
            else
            {
                subst_rates[i] = value[matrix_symmetries[i]];
            }
        }
    }
}

bool Model::evaluate_criteria (mt_size_t n_branches_params,
                                double sample_size )
{
    if (!lnL)
        return false;

    mt_size_t n_params = n_free_variables + n_branches_params;

    aic = 2*n_params - 2*lnL;
    aicc = aic + 2*n_params*(n_params+1)/(n_params - sample_size - 1);
    bic = -2*lnL + n_params * log(sample_size);

    return true;
}

double Model::get_bic() const
{
    return bic;
}

double Model::get_aic() const
{
    return aic;
}

double Model::get_aicc() const
{
    return aicc;
}

double Model::get_dt() const
{
    return dt;
}

time_t Model::get_exec_time() const
{
    return exec_time;
}

void Model::set_exec_time( time_t t)
{
    exec_time = t;
}

}
