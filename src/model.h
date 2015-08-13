#ifndef MODEL_H
#define MODEL_H

#include "model_defs.h"

#include <time.h>

namespace modeltest {

class Model
{
public:
    Model(int matrix_index,
          int model_params,
          std::string const& matrix="");

    Model(const Model & other);

    /**
     * @brief Clones all parameters from another model
     * @param other the other model
     */
    void clone(const Model *other);

    /**
     * @brief Gets the name of the model
     * @return the name of the model
     */
    std::string const& get_name() const;

    /**
     * @brief Checks whether the model is already optimized
     * @return true, if the model is optimized
     */
    bool is_optimized() const;

    /**
     * @brief Checks whether the model includes a proportion of invariant sites
     * @return true, if the model is +I
     */
    bool is_I() const;

    /**
     * @brief Checks whether the model contains gamma rate heterogeneity
     * @return true, if the model is +G
     */
    bool is_G() const;

    /**
     * @brief checks whether the model contains unequal frequencies
     * @return true, if state frequencies are unequal
     */
    bool is_F() const;

    /**
     * @brief Gets the matrix symmetries
     * @return the rate matrix symmetries
     */
    const int * get_symmetries( void ) const;

    /**
     * @brief Gets the model state frequencies
     * @return the state frequencies
     */
    const double * get_frequencies( void ) const;

    /**
     * @brief Sets the model state frequencies
     * @param[in] value the model state frequencies
     */
    void set_frequencies(const double value[]);

    /**
     * @brief Gets the substitution rates
     * @return the substitution rates
     */
    const double * get_subst_rates( void ) const;

    /**
     * @brief Sets the substitution rates
     * @param[in] value the new substitution rates
     * @param full_vector if false, contains only the rates
     *                    corresponding to non-equal rates in order
     */
    void set_subst_rates(const double value[],
                         bool full_vector=true);

    /**
     * @brief Gets the number of model substitution rate parameters
     * @return the number of model substitution rate parameters
     */
    int get_n_subst_params() const;

    /**
     * @brief Gets the number of model free parameters
     * @return the number of model free parameters
     */
    int get_n_free_variables() const;

    /**
     * @brief Gets the log-Likelihood score
     * @return the log-Likelihood score
     */
    double get_lnl() const;

    /**
     * @brief Sets the log-Likelihood score
     * @param l the log-Likelihood score
     */
    void set_lnl( double l );

    bool evaluate_criteria (int n_branches_params,
                            double sample_size );

    double get_bic() const;
    double get_aic() const;
    double get_aicc() const;
    double get_dt() const;

    double get_prop_inv() const;
    void set_prop_inv(double value);

    double get_alpha() const;
    void set_alpha(double value);

    time_t get_exec_time() const;
    void set_exec_time( time_t t);
private:
    std::string name;
    int matrix_symmetries[N_SUBST_RATES];

    bool optimize_pinv;
    bool optimize_gamma;
    bool optimize_freqs;

    double prop_inv;
    double alpha;
    double frequencies[N_STATES];
    double subst_rates[N_SUBST_RATES];

    int n_free_variables;

    double lnL;
    double bic;
    double aic;
    double aicc;
    double dt;

    time_t exec_time;
};

}
#endif // MODEL_H
