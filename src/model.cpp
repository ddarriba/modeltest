#include "model.h"
#include "utils.h"

#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <cmath>
#include <cassert>

const double * prot_model_rates[N_PROT_MODEL_MATRICES] = {
   pll_aa_rates_dayhoff,
   pll_aa_rates_lg,
   pll_aa_rates_dcmut,
   pll_aa_rates_jtt,
   pll_aa_rates_mtrev,
   pll_aa_rates_wag,
   pll_aa_rates_rtrev,
   pll_aa_rates_cprev,
   pll_aa_rates_vt,
   pll_aa_rates_blosum62,
   pll_aa_rates_mtmam,
   pll_aa_rates_mtart,
   pll_aa_rates_mtzoa,
   pll_aa_rates_pmb,
   pll_aa_rates_hivb,
   pll_aa_rates_hivw,
   pll_aa_rates_jttdcmut,
   pll_aa_rates_flu,
   pll_aa_rates_stmtrev
 };

const double * prot_model_freqs[N_PROT_MODEL_MATRICES] = {
   pll_aa_freqs_dayhoff,
   pll_aa_freqs_lg,
   pll_aa_freqs_dcmut,
   pll_aa_freqs_jtt,
   pll_aa_freqs_mtrev,
   pll_aa_freqs_wag,
   pll_aa_freqs_rtrev,
   pll_aa_freqs_cprev,
   pll_aa_freqs_vt,
   pll_aa_freqs_blosum62,
   pll_aa_freqs_mtmam,
   pll_aa_freqs_mtart,
   pll_aa_freqs_mtzoa,
   pll_aa_freqs_pmb,
   pll_aa_freqs_hivb,
   pll_aa_freqs_hivw,
   pll_aa_freqs_jttdcmut,
   pll_aa_freqs_flu,
   pll_aa_freqs_stmtrev
};

using namespace std;

namespace modeltest {

static void set_subst_params(int * m_ind, string const& matrix)
 {
    for (mt_index_t i=0; i<N_DNA_SUBST_RATES; i++)
      m_ind[i] = (int)(matrix.at(i) - '0');
 }

Model::Model(int model_params)
    : optimize_pinv(model_params & (MOD_PARAM_INV | MOD_PARAM_INV_GAMMA)),
      optimize_gamma(model_params & (MOD_PARAM_GAMMA | MOD_PARAM_INV_GAMMA)),
      optimize_freqs(model_params & MOD_PARAM_ESTIMATED_FREQ)
{
    prop_inv = 0.0;
    alpha = 0.0;

    lnL  = 0.0;
    bic  = 0.0;
    aic  = 0.0;
    aicc = 0.0;
    dt   = 0.0;

    frequencies = 0;
    subst_rates = 0;
}

Model::~Model()
{
    if (frequencies)
        free(frequencies);
    if (subst_rates)
        free(subst_rates);
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
    assert(0);
    return 0;
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
    return 0;
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

mt_size_t Model::get_n_states( void ) const
{
    return n_frequencies;
}

mt_size_t Model::get_n_subst_rates( void ) const
{
    return n_subst_rates;
}

const double * Model::get_frequencies( void ) const
{
    return frequencies;
}

void Model::set_frequencies(const double value[])
{
    memcpy(frequencies, value, n_frequencies * sizeof(double));
}

const double * Model::get_subst_rates( void ) const
{
    return subst_rates;
}

void Model::set_subst_rates(const double value[], bool full_vector)
{
    if (full_vector)
    {
        memcpy(subst_rates, value, n_subst_rates * sizeof(double));
    }
    else
    {
        assert(0);
    }
}

bool Model::evaluate_criteria (mt_size_t n_branches_params,
                                double sample_size )
{
    if (!is_optimized())
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

DnaModel::DnaModel(mt_index_t matrix_index,
             int model_params)
    : Model(model_params)
{
    stringstream ss_name;

    assert(matrix_index < N_DNA_ALLMATRIX_COUNT);
    n_frequencies = N_DNA_STATES;
    n_subst_rates = N_DNA_SUBST_RATES;

    frequencies = (double *) Utils::allocate(N_DNA_STATES, sizeof(double));
    subst_rates = (double *) Utils::allocate(N_DNA_SUBST_RATES, sizeof(double));

    set_subst_params(matrix_symmetries, dna_model_matrices[matrix_index]);

    mt_index_t standard_matrix_index = (mt_index_t) (find(dna_model_matrices_indices,
                                     dna_model_matrices_indices + N_DNA_MODEL_MATRICES,
                                     matrix_index) - dna_model_matrices_indices);
    if (standard_matrix_index < N_DNA_MODEL_MATRICES)
    {
        ss_name << dna_model_names[2 * standard_matrix_index + (optimize_freqs?1:0)];
    }
    else
    {
        ss_name << dna_model_matrices[matrix_index];
        if (optimize_freqs)
            ss_name << "[F]";
    }

    for (mt_index_t i=0; i<N_DNA_STATES; i++)
      frequencies[i] = 1.0/N_DNA_STATES;
    for (mt_index_t i=0; i<N_DNA_SUBST_RATES; i++)
      subst_rates[i] = 1.0;

    if (optimize_pinv)
        ss_name << "+I";
    if (optimize_gamma)
        ss_name << "+G";
    name = ss_name.str();

    n_free_variables = get_n_subst_params();
    if (optimize_freqs)
        n_free_variables += N_DNA_STATES-1;
    if (optimize_pinv)
        n_free_variables ++;
    if (optimize_gamma)
        n_free_variables ++;
}

DnaModel::DnaModel(const Model & other)
    : Model(0)
{
    frequencies = (double *) Utils::allocate(N_DNA_STATES, sizeof(double));
    subst_rates = (double *) Utils::allocate(N_DNA_SUBST_RATES, sizeof(double));
    clone(&other);
}

void DnaModel::clone(const Model * other_model)
{
    const DnaModel * other = dynamic_cast<const DnaModel *>(other_model);
    name = other->name;
    memcpy(matrix_symmetries, other->matrix_symmetries, N_DNA_SUBST_RATES * sizeof(int));
    optimize_pinv  = other->optimize_pinv;
    optimize_gamma = other->optimize_gamma;
    optimize_freqs = other->optimize_freqs;

    prop_inv = other->prop_inv;
    alpha    = other->alpha;
    memcpy(frequencies, other->frequencies, N_DNA_STATES * sizeof(double));
    memcpy(subst_rates, other->subst_rates, N_DNA_SUBST_RATES * sizeof(double));

    n_free_variables = other->n_free_variables;

    lnL  = other->lnL;
    bic  = other->bic;
    aic  = other->aic;
    aicc = other->aicc;
    dt   = other->dt;

    exec_time = other->exec_time;
}

const int * DnaModel::get_symmetries( void ) const
{
    return matrix_symmetries;
}

mt_size_t DnaModel::get_n_subst_params() const
{
    return (mt_size_t) *max_element(matrix_symmetries,
                                    matrix_symmetries+N_DNA_SUBST_RATES);
}

void DnaModel::set_subst_rates(const double value[], bool full_vector)
{
    mt_size_t n_subst_params = get_n_subst_params();
    if (full_vector)
    {
        /*TODO: validate symmetries */
        memcpy(subst_rates, value, n_subst_rates * sizeof(double));
    }
    else
    {
        for (mt_index_t i=0; i<n_subst_rates; i++)
        {
            if (((mt_size_t)matrix_symmetries[i]) == n_subst_params)
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

void DnaModel::print(std::ostream  &out)
{
    out << setw(PRINTMODEL_TABSIZE) << left << "Model:" << get_name() << endl
        << setw(PRINTMODEL_TABSIZE) << left << "lnL:" << get_lnl() << endl
        << setw(PRINTMODEL_TABSIZE) << left << "Frequencies:";
    for (mt_index_t i=0; i<N_DNA_STATES; i++)
        out << setprecision(MT_PRECISION_DIGITS) << frequencies[i] << " ";
    out << endl;
    out << setw(PRINTMODEL_TABSIZE) << left << "Subst. Rates:";
    for (mt_index_t i=0; i<N_DNA_SUBST_RATES; i++)
        out << setprecision(MT_PRECISION_DIGITS) << subst_rates[i] << " ";
    out << endl;
    out << setw(PRINTMODEL_TABSIZE) << left << "Inv. sites prop:";
    if (is_I())
        out << prop_inv << endl;
    else
        out << "-" << endl;
    out << setw(PRINTMODEL_TABSIZE) << left << "Gamma shape:";
    if (is_G())
        out << alpha << endl;
    else
        out << "-" << endl;
}


/* PROTEIN MODELS */

ProtModel::ProtModel(mt_index_t matrix_index,
             int model_params)
    : Model(model_params)
{
    stringstream ss_name;

    assert(matrix_index < N_PROT_MODEL_MATRICES);
    n_frequencies = N_PROT_STATES;
    n_subst_rates = N_PROT_SUBST_RATES;

    frequencies = (double *) Utils::allocate(N_PROT_STATES, sizeof(double));
    memcpy(frequencies, prot_model_freqs[matrix_index], N_PROT_STATES * sizeof(double));
    fixed_subst_rates = prot_model_rates[matrix_index];

    ss_name << prot_model_names[matrix_index];
    if (optimize_pinv)
        ss_name << "+I";
    if (optimize_gamma)
        ss_name << "+G";
    if (optimize_freqs)
        ss_name << "+F";
    name = ss_name.str();

    n_free_variables = 0;
    if (optimize_freqs)
        n_free_variables += N_PROT_STATES-1;
    if (optimize_pinv)
        n_free_variables ++;
    if (optimize_gamma)
        n_free_variables ++;
}

ProtModel::ProtModel(const Model & other)
    : Model(0)
{
    frequencies = (double *) Utils::allocate(N_PROT_STATES, sizeof(double));
    clone(&other);
}

ProtModel::~ProtModel()
{
    free(frequencies);
    frequencies = 0;
    subst_rates = 0;
}

void ProtModel::clone(const Model * other_model)
{
    const ProtModel * other = dynamic_cast<const ProtModel *>(other_model);
    name = other->name;
    optimize_pinv  = other->optimize_pinv;
    optimize_gamma = other->optimize_gamma;
    optimize_freqs = other->optimize_freqs;

    prop_inv = other->prop_inv;
    alpha    = other->alpha;
    memcpy(frequencies, other->frequencies, N_PROT_STATES * sizeof(double));
    fixed_subst_rates = other->fixed_subst_rates;

    n_free_variables = other->n_free_variables;

    lnL  = other->lnL;
    bic  = other->bic;
    aic  = other->aic;
    aicc = other->aicc;
    dt   = other->dt;

    exec_time = other->exec_time;
}

mt_size_t ProtModel::get_n_subst_params() const
{
    /* substitution rates are fixed */
    return 0;
}

const double * ProtModel::get_subst_rates( void ) const
{
    return fixed_subst_rates;
}

void ProtModel::set_subst_rates(const double value[], bool full_vector)
{
    UNUSED(value);
    UNUSED(full_vector);

    assert(0);
    cerr << "INTERNAL ERROR: Protein substitution rates must be fixed" << endl;
    exit(EXIT_FAILURE);
}

void ProtModel::print(std::ostream  &out)
{
    out << setw(PRINTMODEL_TABSIZE) << left << "Model:" << get_name() << endl
        << setw(PRINTMODEL_TABSIZE) << left << "lnL:" << get_lnl() << endl
        << setw(PRINTMODEL_TABSIZE) << left << "Frequencies:";
    for (mt_index_t i=0; i<N_PROT_STATES; i++)
    {
        out << setprecision(MT_PRECISION_DIGITS) << frequencies[i] << " ";
        if ((i+1)<N_PROT_STATES && !((i+1)%5))
        {
            out << endl << setw(PRINTMODEL_TABSIZE) << " ";
        }
    }
    out << endl;
    out << setw(PRINTMODEL_TABSIZE) << left << "Inv. sites prop:";
    if (is_I())
        out << prop_inv << endl;
    else
        out << "-" << endl;
    out << setw(PRINTMODEL_TABSIZE) << left << "Gamma shape:";
    if (is_G())
        out << alpha << endl;
    else
        out << "-" << endl;
}
}
