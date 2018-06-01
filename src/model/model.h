/*
  Copyright (C) 2016 Diego Darriba

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU Affero General Public License as
  published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Affero General Public License for more details.

  You should have received a copy of the GNU Affero General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  Contact: Diego Darriba <Diego.Darriba@h-its.org>,
  Heidelberg Institute for Theoretical Studies,
  Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

#ifndef MODEL_H
#define MODEL_H

#include "../loggable.h"
#include "../model_defs.h"
#include "parameter_pinv.h"
#include "parameter_gamma.h"
#include "parameter_branches.h"
#include "parameter_substrates.h"
#include "parameter_frequencies.h"

#include <time.h>

#define PRINTMODEL_TABSIZE 20

namespace modeltest {

class Partition;

typedef struct
{
  mt_index_t matrix_index;
  mt_size_t n_tips;
  mt_size_t n_branches;
  mt_size_t sample_size;
  double loglh;

  double frequencies[N_PROT_STATES];
  double subst_rates[N_DNA_SUBST_RATES];
  double mixture_weights[MT_MAX_CATEGORIES];
  double mixture_rates[MT_MAX_CATEGORIES];

  double prop_invar;
  double alpha;

  int should_save_tree;

  time_t exec_time;
} ckpdata_t;

class Model : public Loggable
{
public:
    /**
     * @brief Creates a new Model
     * @param model_params model parameters mask
     * @param partition the partition descriptor
     * @param asc_bias_corr ascertainment bias correction
     */
    Model(mt_mask_t model_params,
          const partition_descriptor_t &partition,
          mt_size_t states,
          mt_size_t n_categories,
          asc_bias_t asc_bias_corr = asc_none);
    virtual ~Model();

    virtual data_type_t get_datatype( void ) const = 0;

    mt_index_t get_matrix_index( void ) const;

    /**
     * @brief Gets the name of the model
     * @return the name of the model
     */
    std::string const& get_name( void ) const;

    /**
     * @brief Checks whether the model is already optimized
     * @return true, if the model is optimized
     */
    bool is_optimized( void ) const;

    /**
     * @brief Gets model paramaters mask
     * @return the model paramaters mask
     */
    mt_mask_t get_model_params( void ) const;

    /**
     * @brief Checks whether the model includes a proportion of invariant sites
     * @return true, if the model is +I
     */
    bool is_I( void ) const;

    /**
     * @brief Checks whether the model contains gamma rate heterogeneity
     * @return true, if the model is +G
     */
    bool is_G( void ) const;

    /**
     * @brief checks whether the model contains free rates
     * @return true, if the model is +R
     */
    bool is_R( void ) const;

    /**
     * @brief checks whether the model contains unequal frequencies
     * @return true, if state frequencies are unequal
     */
    bool is_F( void ) const;

    /**
     * @brief checks whether the model is a mixture model
     * @return true, if it is a mixture model
     */
    bool is_mixture( void ) const;

    bool is_gap_aware( void ) const;

    /**
     * @brief Gets the matrix symmetries
     * @return the rate matrix symmetries
     */
    virtual const int * get_symmetries( void ) const;

    /**
     * @brief Gets the number of states
     * @return the number of states
     */
    mt_size_t get_n_states( void ) const;

    /**
     * @brief Gets the number of categories
     * @return the number of categories
     */
    mt_size_t get_n_categories( void ) const;

    /**
     * @brief Gets the number of substitution rates
     * @return the number of substitution rates
     */
    mt_size_t get_n_subst_rates( void ) const;

    /**
     * @brief Gets the model state frequencies
     * @param[in] freqs_idx the index of frequencies set
     * @return the state frequencies
     */
    const double * get_frequencies( mt_index_t freqs_idx = 0 ) const;

    /**
     * @brief Sets the model state frequencies
     * @param[in] value the model state frequencies
     */
    void set_frequencies(const double value[]);
    void set_frequencies(const std::vector<double> & value);

    /**
     * @brief Gets the substitution rates
     * @return the substitution rates
     */
    virtual const double * get_subst_rates( mt_index_t matrix_idx = 0 ) const;
    void set_subst_rates(const double value[], mt_index_t matrix_idx = 0);

    const unsigned int * get_params_indices( void ) const;

    virtual const double * get_mixture_weights( void ) const;

    virtual void set_mixture_weights( const double * weights );

    virtual const double * get_mixture_rates( void ) const;

    virtual void set_mixture_rates( const double * rates );

    /**
     * @brief Gets the number of model substitution rate free parameters
     *
     * For example, JC (000000) has 0 parameters, while
     * GTR (012345) has 5.
     *
     * @return the number of model substitution rate parameters
     */
    virtual mt_size_t get_n_subst_params( void ) const;

    /**
     * @brief Gets the number of model free parameters
     * @return the number of model free parameters
     */
    mt_size_t get_n_free_variables( void ) const;

    /**
     * @brief Gets the log-Likelihood score
     * @return the log-Likelihood score
     */
    double get_loglh( void ) const;

    /**
     * @brief Sets the log-Likelihood score
     * @param l the log-Likelihood score
     */
    void set_loglh( double l );

    bool optimize_init ( pll_partition_t * pll_partition,
                         pllmod_treeinfo_t * tree_info,
                         Partition const& partition );

    bool optimize( pll_partition_t * partition,
                   pllmod_treeinfo_t * tree_info,
                   double tolerance );

    bool optimize_oneparameter( pll_partition_t * partition,
                                pllmod_treeinfo_t * tree_info,
                                double tolerance );
    /**
     * @brief Prints out the model in a human readable way
     * @param[in] out the output stream to print to
     */
    virtual void print(std::ostream  &out = std::cout) = 0;

    virtual void print_inline(int index,
                              int n_models,
                              time_t ini_time,
                              time_t global_ini_time,
                              std::ostream &out = std::cout);

    /**
     * @brief Prints out the model in XML format
     * @param[in] out the output stream to print to
     */
    virtual void print_xml(std::ostream  &out = std::cout) = 0;

    /**
     * @brief Prints out the model for logging
     * @param[in] out the output stream to print log to
     */
    virtual void output_log(std::ostream  &out) const = 0;

    /**
     * @brief Load the model from a logging stream
     * @param[in] in the input stream for reading the log
     */
    virtual void input_log(std::istream  &in) = 0;

    /**
     * @brief Export object in binary format
     * @param  bin_filename binary file
     * @return true, if success
     */
    virtual int output_bin(std::string const& bin_filename) const;

    /**
     * @brief Import object in binary format
     * @param  bin_filename binary file
     * @return true, if success
     */
    virtual int input_bin(std::string const& bin_filename);

    bool evaluate_criteria (mt_size_t n_branches_params,
                            double sample_size );

    unsigned int * params_indices;

    double get_bic( void ) const;
    void set_bic(double value);

    double get_aic( void ) const;
    void set_aic(double value);

    double get_aicc( void ) const;
    void set_aicc(double value);

    double get_dt( void ) const;
    void set_dt(double value);

    double get_prop_inv( void ) const;
    void set_prop_inv(double value);

    double get_alpha( void ) const;
    void set_alpha(double value);

    const mt_size_t * get_asc_weights( void ) const;
    asc_bias_t get_asc_bias_corr( void ) const;

    time_t get_exec_time() const;
    void set_exec_time( time_t t);

    virtual pll_partition_t * build_partition( mt_size_t n_tips,
                                               mt_size_t n_sites) = 0;
    pll_utree_t * get_tree( void ) const;
    pll_unode_t * get_tree_graph( void ) const;

    void set_tree( pll_unode_t * tree, int n_tips = 0 );
    void set_tree( pll_utree_t * tree );

    mt_index_t get_unique_id( void ) const;

protected:

    static mt_mask_t asc_bias_attribute(asc_bias_t v);

    bool restored_from_ckp;

    mt_index_t matrix_index;
    mt_index_t current_opt_parameter;
    mt_index_t unique_id;
    std::string name;

    /* model parameters */
    /* NOTE: If model is mixture and not gamma, rates are free */
    mt_mask_t model_params;
    bool optimize_pinv;
    bool optimize_gamma;
    bool optimize_freqs;
    bool empirical_freqs;
    bool optimize_ratecats;

    bool mixture;

    bool gap_aware;

    mt_size_t *asc_weights;
    asc_bias_t asc_bias_corr;

    mt_size_t n_free_variables;

    double loglh;
    double bic;
    double aic;
    double aicc;
    double dt;

    time_t exec_time;

    mt_size_t n_categories;
    mt_size_t n_frequencies;
    mt_size_t n_subst_rates;

    pll_utree_t *tree;
    mt_size_t n_tips;
    mt_size_t n_branches;
    mt_size_t sample_size;

    std::vector<AbstractParameter *> parameters;
    ParameterRateCats * param_gamma;
    ParameterPinv * param_pinv;
    ParameterSubstRates * param_substrates;
    ParameterFrequencies * param_freqs;
    ParameterBranches * param_branches;
};

}
#endif // MODEL_H
