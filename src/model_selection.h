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
     * @brief Gets the number of models in the selection
     * @return the number of models in the selection
     */
    size_t size() const;

    /**
     * @brief Gets one single selection model
     * @param[in] i the position
     * @return the i-th selection model
     */
    const selection_model & get_model(size_t i) const;

    /**
     * @brief Prints the selection results
     * @param[in] out the output stream to print to
     */
    void print(std::ostream  &out = std::cout, mt_size_t limit = 0);

private:
    std::vector<selection_model> models; //! selection wrappers
    std::string ic_name;
};

}

#endif // MODEL_SELECTION_H
