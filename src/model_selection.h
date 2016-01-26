#ifndef MODEL_SELECTION_H
#define MODEL_SELECTION_H

#include "model.h"
#include <vector>
#include <iostream>

namespace modeltest {

//! type of criteria to perform the selection
enum ic_type {
    ic_lnl,   //! log-Likelihood score
    ic_bic,   //! Bayesian Information Criterion
    ic_aic,   //! Akaike Information Criterion
    ic_aicc,  //! Corrected Akaike Information Criterion
    ic_dt     //! Decision Theory
};

//! wrapper for model selection
typedef struct
{
    Model * model;  //! the model
    double score;   //! the criterion score
    double delta;   //! the score difference to the best model
    double weight;  //! the weight among the candidates
} selection_model;

class ModelSelection
{
public:
    ModelSelection(const std::vector<Model *> &c_models,
                   ic_type type);

    /**
     * @brief Get the number of models in the selection
     * @return the number of models in the selection
     */
    size_t size() const;

    /**
     * @brief Get one single selection model
     * @param[in] i the position
     * @return the i-th selection model
     */
    const selection_model & get_model(size_t i) const;

    /**
     * @brief Print out the selection results
     * @param[in] out the output stream to print to
     * @param[in] limit the number of models to print
     */
    void print(std::ostream  &out = std::cout, mt_size_t limit = 0);

    /**
     * @brief Print out the best model
     * @param[in] out the output stream to print to
     */
    void print_best_model(std::ostream &out = std::cout);

    static void print_inline_header(std::ostream &out = std::cout);
    static void print_inline_best_model(ic_type type, selection_model &model, std::ostream &out = std::cout);

    /**
     * @brief Print out the parameter importances
     * @param[in] out the output stream to print to
     */
    void print_importances(std::ostream  &out = std::cout);

    /**
     * @brief Get the importance of +I parameter
     * @return the importance of +I parameter
     */
    double get_importance_inv() const
    {
        return importance_inv;
    }

    /**
     * @brief Get the importance of +G parameter
     * @return the importance of +G parameter
     */
    double get_importance_gamma() const
    {
        return importance_gamma;
    }

    /**
     * @brief Get the importance of +I+G parameter combination
     * @return the importance of +I+G parameter
     */
    double get_importance_gamma_inv() const
    {
        return importance_gamma_inv;
    }

    /**
     * @brief Get the importance of frequencies free parameters
     * @return the importance of frequencies free parameters
     */
    double get_importance_freqs() const
    {
        return importance_freqs;
    }
private:
    std::vector<selection_model> models; //! selection wrappers
    std::string ic_name;                 //! criterion name
    double importance_inv;               //! importance of +I parameter
    double importance_gamma;             //! importance of +G parameter
    double importance_gamma_inv;         //! importance of +I+G parameters
    double importance_freqs;             //! importance of +F parameter
};

}

#endif // MODEL_SELECTION_H
