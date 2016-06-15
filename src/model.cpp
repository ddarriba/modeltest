#include "model.h"
#include "utils.h"

#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <cmath>
#include <cassert>

#include <fstream>

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

Model::Model(mt_mask_t model_params, asc_bias_t asc_bias_corr)
    : optimize_pinv(model_params & (MOD_PARAM_INV | MOD_PARAM_INV_GAMMA)),
      optimize_gamma((model_params & (MOD_PARAM_GAMMA | MOD_PARAM_INV_GAMMA))
                     && !(model_params & MOD_PARAM_FREE_RATES)),
      optimize_freqs(model_params & MOD_PARAM_ESTIMATED_FREQ),
      mixture(model_params & MOD_PARAM_MIXTURE), asc_bias_corr(asc_bias_corr)
{
    matrix_index = 0;
    prop_inv = 0.0;
    alpha = 0.0;

    lnL  = 0.0;
    bic  = 0.0;
    aic  = 0.0;
    aicc = 0.0;
    dt   = 0.0;

    n_categories = 0;
    params_indices = 0;

    frequencies = 0;
    subst_rates = 0;

    tree = 0;

    asc_weights = 0;
}

Model::~Model()
{
  if (asc_weights)
    delete[] asc_weights;
  if (frequencies)
    delete[] frequencies;
  if (subst_rates)
    delete[] subst_rates;
  if (params_indices)
    free(params_indices);
  if (tree)
  {
      pll_utree_destroy(tree->back);
  }
}

mt_index_t Model::get_matrix_index() const
{
    return matrix_index;
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

bool Model::is_mixture( void ) const
{
    return mixture;
}

const int * Model::get_symmetries( void ) const
{
    assert(0);
    return 0;
}

mt_size_t Model::get_n_categories() const
{
    return n_categories;
}

void Model::set_n_categories( mt_size_t ncat )
{
    n_categories = ncat;
    if (params_indices)
      free (params_indices);

    if (ncat > 0)
    {
      params_indices = (unsigned int *) Utils::c_allocate(
                                        n_categories, sizeof(unsigned int));
    }

    if (mixture)
    {
      assert(ncat == N_MIXTURE_CATS);
      for (mt_index_t i = 0; i<N_MIXTURE_CATS; ++i)
        params_indices[i] = i;
    }
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

const double * Model::get_mixture_frequencies( mt_index_t matrix_idx ) const
{
    UNUSED(matrix_idx);
    return 0;
}

const unsigned int * Model::get_params_indices( void ) const
{
  assert(params_indices);
  return params_indices;
}

void Model::set_frequencies(const double value[])
{
    memcpy(frequencies, value, n_frequencies * sizeof(double));
}

void Model::set_frequencies(const vector<double> & value)
{
    memcpy(frequencies, &(value[0]), n_frequencies * sizeof(double));
}

const double * Model::get_subst_rates( void ) const
{
    return subst_rates;
}

const double * Model::get_mixture_subst_rates( mt_index_t matrix_idx ) const
{
    UNUSED(matrix_idx);
    return 0;
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

const double * Model::get_mixture_weights( void ) const
{
    return 0;
}

void Model::set_mixture_weights(const double value[])
{
    UNUSED(value);
    assert(0);
}

const double * Model::get_mixture_rates( void ) const
{
    return 0;
}

void Model::set_mixture_rates(const double value[])
{
    UNUSED(value);
    assert(0);
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

pll_utree_t * Model::get_tree( void ) const
{
    assert(is_optimized());
    return tree;
}

void Model::set_tree( pll_utree_t * _tree )
{
    if (tree)
        pll_utree_destroy(tree->back);
    tree = _tree;
}

bool Model::optimize( void )
{
//        mt_size_t iters_hard_limit = 200;
//        while (fabs (cur_logl - logl) > epsilon && cur_logl < logl)
//        {
//            double iter_logl;
//            if (!(n_iters % params_to_optimize.size()))
//              logl = cur_logl;

//            mt_parameter_t cur_parameter = params_to_optimize[cur_parameter_index];
//#if(CHECK_LOCAL_CONVERGENCE)
//            if (!(converged & cur_parameter))
//            {
//                test_logl = cur_logl;
//#endif
//                bool full_range_search = n_iters<params_to_optimize.size();

//                iter_logl = opt_single_parameter(cur_parameter, tolerance, full_range_search);

//                //printf(" iteration %3d %3d %.10f %.10f\n", cur_parameter, params_to_optimize.size(), iter_logl, cur_logl);

//                /* ensure we never get a worse likelihood score */
//                assert(iter_logl - cur_logl < 1e-5);
//                cur_logl = iter_logl;

//                // notify parameter optimization
//                opt_delta = cur_logl;
//                notify();

//#if(CHECK_LOCAL_CONVERGENCE)
//                if (fabs(test_logl - cur_logl) < tolerance)
//                    converged |= cur_parameter;
//            }
//#endif

//            cur_parameter_index++;
//            cur_parameter_index %= params_to_optimize.size();

//            n_iters++;
//            iters_hard_limit--;
//            if (!iters_hard_limit)
//                break;
//        }

//        /* TODO: if bl are reoptimized */
//        if (keep_branch_lengths)
//          tree->set_bl_optimized();


//    cur_logl *= -1;

//    time_t end_time = time(NULL);

//    if (_num_threads > 1)
//    {
//        /* finalize */
//        thread_job = JOB_FINALIZE;

//        /* join threads */
//        for (mt_index_t i=1; i<_num_threads; i++)
//          pthread_join (threads[i], NULL);

//        /* clean */
//        for (mt_index_t i=0; i<_num_threads; i++)
//          dealloc_partition_local(thread_data[i].partition);

//        free(result_buf);
//        free(threads);
//        free(thread_data);
//        free(partition_local);
//    }
    return true;
}

DnaModel::DnaModel(mt_index_t _matrix_index,
                   mt_mask_t model_params,
                   asc_bias_t asc_bias_corr,
                   mt_size_t *asc_w)
    : Model(model_params, asc_bias_corr)
{
    stringstream ss_name;

    matrix_index = _matrix_index;
    assert(matrix_index < N_DNA_ALLMATRIX_COUNT);

    n_frequencies = N_DNA_STATES;
    n_subst_rates = N_DNA_SUBST_RATES;

    frequencies = new double[N_DNA_STATES];
    subst_rates = new double[N_DNA_SUBST_RATES];

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

    if (asc_bias_corr == asc_felsenstein)
    {
      assert(asc_w);
      asc_weights = new mt_size_t[N_DNA_STATES];
      fill_n(asc_weights, N_DNA_STATES, 0);
      asc_weights[0] = asc_w[0];
    }
    else if (asc_bias_corr == asc_stamatakis)
    {
      assert(asc_w);
      asc_weights = new mt_size_t[N_DNA_STATES];
      memcpy(asc_weights, asc_w, N_DNA_STATES * sizeof(mt_size_t));
    }
}

DnaModel::DnaModel(const Model & other)
    : Model(0)
{
    frequencies = new double[N_DNA_STATES];
    subst_rates = new double[N_DNA_SUBST_RATES];
    clone(&other);
}

void DnaModel::clone(const Model * other_model)
{
    const DnaModel * other = dynamic_cast<const DnaModel *>(other_model);
    matrix_index = other->matrix_index;
    name = other->name;
    memcpy(matrix_symmetries, other->matrix_symmetries, N_DNA_SUBST_RATES * sizeof(int));
    optimize_pinv  = other->optimize_pinv;
    optimize_gamma = other->optimize_gamma;
    optimize_freqs = other->optimize_freqs;

    prop_inv = other->prop_inv;
    alpha    = other->alpha;
    memcpy(frequencies, other->frequencies, N_DNA_STATES * sizeof(double));
    memcpy(subst_rates, other->subst_rates, N_DNA_SUBST_RATES * sizeof(double));

    set_n_categories(other->n_categories);
    n_free_variables = other->n_free_variables;

    lnL  = other->lnL;
    bic  = other->bic;
    aic  = other->aic;
    aicc = other->aicc;
    dt   = other->dt;

    exec_time = other->exec_time;
    if (other->tree)
        tree = pll_utree_clone(other->tree);
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

static mt_mask_t asc_bias_attribute(asc_bias_t v)
{
  mt_mask_t attr = 0;
  if (v != asc_none)
  {
    attr = PLL_ATTRIB_ASC_BIAS_FLAG;
    if (v == asc_lewis)
      attr |= PLL_ATTRIB_ASC_BIAS_LEWIS;
    else if (v == asc_felsenstein)
      attr |= PLL_ATTRIB_ASC_BIAS_FELSENSTEIN;
    else if (v == asc_stamatakis)
      attr |= PLL_ATTRIB_ASC_BIAS_STAMATAKIS;
  }
  return attr;
}

pll_partition_t * DnaModel::build_partition(mt_size_t n_tips,
                                            mt_size_t n_sites,
                                            mt_size_t n_cat_g) const
{
    mt_mask_t attributes = PLL_ATTRIB_PATTERN_TIP;
#ifdef HAVE_AVX
    attributes |= PLL_ATTRIB_ARCH_AVX;
#else
#ifdef HAVE_SSE
    attributes |= PLL_ATTRIB_ARCH_SSE;
#else
    attributes |= PLL_ATTRIB_ARCH_CPU;
#endif
#endif
    attributes |= asc_bias_attribute(asc_bias_corr);

    pll_partition_t * part = pll_partition_create (
                n_tips,                           /* tips */
                n_tips-2,                         /* clv buffers */
                N_DNA_STATES,                     /* states */
                n_sites,                          /* sites */
                1,                                /* rate matrices */
                2*n_tips-3,                       /* prob matrices */
                optimize_gamma ? n_cat_g : 1,     /* rate cats */
                n_tips-2,                         /* scale buffers */
                attributes                        /* attributes */
                );

    if (asc_weights)
      pll_set_asc_state_weights(part, asc_weights);

    return part;
}

void DnaModel::print(std::ostream  &out)
{
    out << setw(PRINTMODEL_TABSIZE) << left << "Model:" << name << endl
        << setw(PRINTMODEL_TABSIZE) << left << "lnL:" << lnL << endl
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

void DnaModel::print_xml(std::ostream  &out)
{
    out << "<model datatype=\"dna\" name=\"" << name
        << "\" lnl=\"" << setprecision(MT_PRECISION_DIGITS) << lnL
        << "\">" << endl;
    out << "  <frequencies type=\"";
    if (is_F())
        out << "maximum-likelihood";
    else
        out << "fixed";
    out << "\">" << endl << "  ";
    for (mt_index_t i=0; i<N_DNA_STATES; i++)
        out << "  " << setprecision(MT_PRECISION_DIGITS) << frequencies[i];
    out << endl << "  </frequencies>" << endl;
    out << "  <subst_rates>" << endl << "  ";
    for (mt_index_t i=0; i<N_DNA_SUBST_RATES; i++)
        out << "  " << setprecision(MT_PRECISION_DIGITS) << subst_rates[i];
    out << endl << "  </subst_rates>" << endl;
    out << "  <rate_params pinv=\"" << setprecision(MT_PRECISION_DIGITS) << prop_inv <<
           "\" alpha=\"" << alpha << "\"/>" << endl;
    out << "</model>" << endl;
}

void DnaModel::output_log(std::ostream  &out)
{
    out << name << " ";
    out << matrix_index << " ";
    for (mt_index_t i=0; i<N_DNA_SUBST_RATES; i++)
        out << matrix_symmetries[i] << " ";
    out << n_free_variables << " ";
    out << optimize_freqs << " " << optimize_pinv << " " << optimize_gamma << " ";
    out << lnL << " ";
    for (mt_index_t i=0; i<N_DNA_STATES; i++)
        out << setprecision(MT_PRECISION_DIGITS) << frequencies[i] << " ";
    for (mt_index_t i=0; i<N_DNA_SUBST_RATES; i++)
        out << setprecision(MT_PRECISION_DIGITS) << subst_rates[i] << " ";
    out << setprecision(MT_PRECISION_DIGITS) << prop_inv << " ";
    out << setprecision(MT_PRECISION_DIGITS) << alpha << " ";
    char *newick = pll_utree_export_newick(tree);
    out << strlen(newick) << " ";
    out << newick << " ";
    free(newick);
    out << endl;
}

static bool read_word(std::istream  &in, char *w, mt_size_t maxlen)
{
    char c = '\0';
    mt_index_t l = 0;
    while (in.get(c) && c!=' ' && l<maxlen)
    {
        *(w++) = c;
        l++;
    }
    *w = '\0';
    return c != '\0';
}

#define LOG_LEN 50
void DnaModel::input_log(std::istream  &in)
{
    char str[LOG_LEN];
    char *treestr;

    read_word(in, str, LOG_LEN);       name = str;
    read_word(in, str, LOG_LEN); matrix_index = Utils::parse_index(str);
    for (mt_index_t i=0; i<N_DNA_SUBST_RATES; i++)
    {
        read_word(in, str, LOG_LEN); matrix_symmetries[i] = atoi(str);
    }
    read_word(in, str, LOG_LEN); n_free_variables = Utils::parse_size(str);
    read_word(in, str, LOG_LEN); optimize_freqs = atoi(str);
    read_word(in, str, LOG_LEN); optimize_pinv = atoi(str);
    read_word(in, str, LOG_LEN); optimize_gamma = atoi(str);
    read_word(in, str, LOG_LEN); lnL = atof(str);
    for (mt_index_t i=0; i<N_DNA_STATES; i++)
    {
        read_word(in, str, LOG_LEN); frequencies[i] = atof(str);
    }
    for (mt_index_t i=0; i<N_DNA_SUBST_RATES; i++)
    {
        read_word(in, str, LOG_LEN); subst_rates[i] = atof(str);
    }
    read_word(in, str, LOG_LEN); prop_inv = atof(str);
    read_word(in, str, LOG_LEN); alpha    = atof(str);
    read_word(in, str, LOG_LEN); mt_size_t treelen = Utils::parse_size(str);

    treestr = (char *) Utils::allocate(treelen + 1, 1);
    read_word(in, treestr, treelen);
    //TODO: TEMPORARY DUMP TO FILE
    char fname[16] = "temptree.XXXXXX";
    int f_desc = mkstemp(fname);
    if (f_desc == -1)
    {
        cerr << "ERROR: mkstemp returned error " << errno << endl;
        assert(0);
        return;
    }
    ofstream f;
    f.open(fname);
    f << treestr << endl;
    f.close();
    mt_size_t n_tips;
    tree = pll_utree_parse_newick(fname, &n_tips);
    free(treestr);

    //unlink(fname);
}

#undef LOG_LEN

/* PROTEIN MODELS */

ProtModel::ProtModel(mt_index_t _matrix_index,
             mt_mask_t model_params,
             asc_bias_t asc_bias_corr,
             mt_size_t *asc_w)
    : Model(model_params, asc_bias_corr)
{
    stringstream ss_name;

    matrix_index = _matrix_index;
    assert(matrix_index < N_PROT_MODEL_ALL_MATRICES);
    mixture = (matrix_index == LG4M_INDEX || matrix_index == LG4X_INDEX);

    n_frequencies   = N_PROT_STATES;
    n_subst_rates   = N_PROT_SUBST_RATES;
   n_free_variables = 0;

    if (mixture)
    {
        assert (optimize_gamma && !optimize_freqs);
        if (matrix_index == LG4M_INDEX)
        {
            /* LG4M model */
            mixture_frequencies = pll_aa_freqs_lg4m;
            mixture_subst_rates = pll_aa_rates_lg4m;
        }
        else
        {
            /* LG4X model / free rates */
            optimize_gamma = false;
            n_free_variables += 6;
            mixture_frequencies = pll_aa_freqs_lg4x;
            mixture_subst_rates = pll_aa_rates_lg4x;
        }
    }
    else
    {
        frequencies = new double[N_PROT_STATES];
        memcpy(frequencies, prot_model_freqs[matrix_index], N_PROT_STATES * sizeof(double));
        fixed_subst_rates = prot_model_rates[matrix_index];
    }

    if (asc_bias_corr == asc_felsenstein)
    {
      assert(asc_w);
      asc_weights = new mt_size_t[N_PROT_STATES];
      fill_n(asc_weights, N_PROT_STATES, 0);
      asc_weights[0] = asc_w[0];
    }
    else if (asc_bias_corr == asc_stamatakis)
    {
      assert(asc_w);
      asc_weights = new mt_size_t[N_PROT_STATES];
      memcpy(asc_weights, asc_w, N_PROT_STATES * sizeof(mt_size_t));
    }

    ss_name << prot_model_names[matrix_index];
    if (optimize_pinv)
        ss_name << "+I";
    if (optimize_gamma && !mixture)
        ss_name << "+G";
    if (optimize_freqs)
        ss_name << "+F";
    name = ss_name.str();

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
    frequencies = new double[N_PROT_STATES];
    clone(&other);
}

ProtModel::~ProtModel()
{
    delete[] frequencies;
    frequencies = 0;
    subst_rates = 0;
}

void ProtModel::clone(const Model * other_model)
{
    const ProtModel * other = dynamic_cast<const ProtModel *>(other_model);
    matrix_index = other->matrix_index;
    name = other->name;
    optimize_pinv  = other->optimize_pinv;
    optimize_gamma = other->optimize_gamma;
    optimize_freqs = other->optimize_freqs;

    prop_inv = other->prop_inv;
    alpha    = other->alpha;
    //TODO: Check for LG4!
    memcpy(frequencies, other->frequencies, N_PROT_STATES * sizeof(double));
    fixed_subst_rates = other->fixed_subst_rates;

    set_n_categories(other->n_categories);
    n_free_variables = other->n_free_variables;

    lnL  = other->lnL;
    bic  = other->bic;
    aic  = other->aic;
    aicc = other->aicc;
    dt   = other->dt;

    exec_time = other->exec_time;
    if (other->tree)
        tree = pll_utree_clone(other->tree);
}

mt_size_t ProtModel::get_n_subst_params() const
{
    /* substitution rates are fixed */
    return 0;
}

const double * ProtModel::get_mixture_frequencies( mt_index_t matrix_idx ) const
{
    return mixture_frequencies[matrix_idx];
}

const double * ProtModel::get_subst_rates( void ) const
{
    return fixed_subst_rates;
}

const double * ProtModel::get_mixture_subst_rates( mt_index_t matrix_idx ) const
{
    return mixture_subst_rates[matrix_idx];
}

void ProtModel::set_subst_rates(const double value[], bool full_vector)
{
    UNUSED(value);
    UNUSED(full_vector);

    Utils::exit_with_error("INTERNAL ERROR: Protein substitution rates must be fixed");
}

const double * ProtModel::get_mixture_weights( void ) const
{
    assert(mixture);
    return mixture_weights;
}

void ProtModel::set_mixture_weights(const double value[])
{
    assert(mixture);
    memcpy(mixture_weights, value, N_MIXTURE_CATS * sizeof(double));
}

const double * ProtModel::get_mixture_rates( void ) const
{
    assert(mixture);
    return mixture_rates;
}

void ProtModel::set_mixture_rates(const double value[])
{
    assert(mixture);
    memcpy(mixture_rates, value, N_MIXTURE_CATS * sizeof(double));
}


pll_partition_t * ProtModel::build_partition(mt_size_t n_tips,
                                            mt_size_t n_sites,
                                            mt_size_t n_cat_g) const
{
    mt_mask_t attributes = PLL_ATTRIB_PATTERN_TIP;
#ifdef HAVE_AVX
    attributes |= PLL_ATTRIB_ARCH_AVX;
#else
#ifdef HAVE_SSE
    attributes |= PLL_ATTRIB_ARCH_SSE;
#else
    attributes |= PLL_ATTRIB_ARCH_CPU;
#endif
#endif
    attributes |= asc_bias_attribute(asc_bias_corr);

    mt_size_t n_cats = optimize_gamma?n_cat_g:1;
    if (mixture)
        n_cats = N_MIXTURE_CATS;

    pll_partition_t * part = pll_partition_create (
                n_tips,                           /* tips */
                n_tips-2,                         /* clv buffers */
                N_PROT_STATES,                    /* states */
                n_sites,                          /* sites */
                mixture?N_MIXTURE_CATS:1,         /* rate matrices */
                2*n_tips-3,                       /* prob matrices */
                n_cats,                           /* rate cats */
                n_tips-2,                         /* scale buffers */
                attributes                        /* attributes */
                );

    if (asc_weights)
      pll_set_asc_state_weights(part, asc_weights);

    return part;
}

void ProtModel::print(std::ostream  &out)
{
    out << setw(PRINTMODEL_TABSIZE) << left << "Model:" << name << endl
        << setw(PRINTMODEL_TABSIZE) << left << "lnL:" << lnL << endl
        << setw(PRINTMODEL_TABSIZE) << left << "Frequencies:";
    if (mixture)
    {
        for (mt_index_t j=0; j<N_MIXTURE_CATS; j++)
        {
            for (mt_index_t i=0; i<N_PROT_STATES; i++)
            {
                out << setprecision(MT_PRECISION_DIGITS) << mixture_frequencies[j][i] << " ";
                if ((i+1)<N_PROT_STATES && !((i+1)%5))
                {
                    out << endl << setw(PRINTMODEL_TABSIZE) << " ";
                }
            }
            out << endl << endl;
            if (j <N_MIXTURE_CATS-1) out << setw(PRINTMODEL_TABSIZE) << " ";
        }
        out << setw(PRINTMODEL_TABSIZE) << left << "Mixture weights:";
        for (mt_index_t j=0; j<N_MIXTURE_CATS; j++)
        {
           out << setprecision(MT_PRECISION_DIGITS) << mixture_weights[j] << " ";
        }
        out << endl;
        out << setw(PRINTMODEL_TABSIZE) << left << "Mixture rates:";
        for (mt_index_t j=0; j<N_MIXTURE_CATS; j++)
        {
           out << setprecision(MT_PRECISION_DIGITS) << mixture_rates[j] << " ";
        }
        out << endl;
    }
    else
    {
        for (mt_index_t i=0; i<N_PROT_STATES; i++)
        {
            out << setprecision(MT_PRECISION_DIGITS) << frequencies[i] << " ";
            if ((i+1)<N_PROT_STATES && !((i+1)%5))
            {
                out << endl << setw(PRINTMODEL_TABSIZE) << " ";
            }
        }
        out << endl;
    }
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

void ProtModel::print_xml(std::ostream  &out)
{
    out << "<model type=\"aa\" name=\"" << name
        << "\" lnl=\"" << setprecision(MT_PRECISION_DIGITS) << lnL
        << "\">" << endl;
    out << "  <frequencies type=\"";
    if (is_F())
        out << "empirical";
    else
        out << "model";
    out << "\">" << endl << "  ";
    for (mt_index_t i=0; i<N_PROT_STATES; i++)
    {
        out << "  " << setprecision(MT_PRECISION_DIGITS) << frequencies[i];
        if ((i%5)==4) out << endl << "  ";
    }
    out << "</frequencies>" << endl;
    out << "  <rate_params pinv=\"" << setprecision(MT_PRECISION_DIGITS) << prop_inv <<
           "\" alpha=\"" << alpha << "\"/>" << endl;
    out << "</model>" << endl;
}

void ProtModel::output_log(std::ostream  &out)
{
    out << get_name() << " ";
    out << get_matrix_index() << " ";
    out << is_F() << " " << is_I() << " " << is_G() << " ";
    out << get_lnl() << " ";
    for (mt_index_t i=0; i<N_PROT_STATES; i++)
        out << setprecision(MT_PRECISION_DIGITS) << frequencies[i] << " ";
    if (is_I())
        out << setprecision(MT_PRECISION_DIGITS) << prop_inv << " ";
    else
        out << "- ";
    if (is_G())
        out << setprecision(MT_PRECISION_DIGITS) << alpha << " ";
    else
        out << "- ";
    char *newick = pll_utree_export_newick(tree);
    out << newick << " ";
    free(newick);
    out << endl;
}

void ProtModel::input_log(std::istream  &in)
{
    //TODO:
}

}
