#include "model_selection.h"
#include "treepll.h"

#include <cassert>
#include <cmath>
#include <algorithm>
#include <iomanip>

namespace modeltest {

using namespace std;

static bool compare_score(const selection_model &m1, const selection_model &m2)
{
    return m1.score < m2.score;
}

ModelSelection::ModelSelection(const vector<Model *> &c_models,
                               ic_type type)
{
    double sum_exp = 0.0;
    models.resize(c_models.size());
    for (size_t i=0; i<c_models.size(); i++)
    {
        models[i].model = c_models[i];
        switch(type)
        {
        case ic_lnl:
            ic_name = "lnL";
            models[i].score = models[i].model->get_lnl();
            break;
        case ic_bic:
            ic_name = "BIC";
            models[i].score = models[i].model->get_bic();
            break;
        case ic_aic:
            ic_name = "AIC";
            models[i].score = models[i].model->get_aic();
            break;
        case ic_aicc:
            ic_name = "AICc";
            models[i].score = models[i].model->get_aicc();
            break;
        case ic_dt:
            ic_name = "DT";
            models[i].score = models[i].model->get_dt();
            break;
        }
    }

    if (type == ic_dt)
    {
        /* exactly as in DT-ModSel.pl */
        double * bicLike = new double[c_models.size()];
        double minBIC = c_models[0]->get_bic();
        double minDT;
        double denom = 0.0;
        double sum, sumReciprocal;

        /* get minimum BIC */
        for (size_t i=0; i<c_models.size(); i++)
        {
            if (c_models[i]->get_bic() < minBIC)
                minBIC = c_models[i]->get_bic();
        }

        for (size_t i=0; i<c_models.size(); i++)
        {
            bicLike[i] = exp(-0.5*(c_models[i]->get_bic() - minBIC));
            denom += bicLike[i];
        }

        /* TODO: set to DOUBLE_MAX */
        minDT = 999999999999;
        for (size_t i=0; i<c_models.size(); i++)
        {
            Model * model1 = models[i].model;
            sum = 0.0;
            for (size_t j=0; j<c_models.size(); j++)
            {
                Model * model2 = models[j].model;

                double distance = TreePll::compute_euclidean_distance(model1->get_tree(), model2->get_tree());
                /* TODO: compute distance between trees in a table*/
                // distance = distances.getDistance(model1.getTree(), model2.getTree());

                assert(distance >= 0);
                sum += distance * bicLike[j];
                //sum += exp(log(distance) - c_models[j]->get_bic() + minBIC);
            }
//sum(exp(log(dij) − bic[j] + min_bic))
            models[i].score = sum / denom;
            if (models[i].score < minDT)
            {
                minDT = models[i].score;
            }
        }

        // Calculate DT differences
        sumReciprocal = sum = 0;
        for (size_t i=0; i<c_models.size(); i++)
        {
            models[i].delta = models[i].score - minDT;
            sumReciprocal += 1.0 / models[i].score;
        }

        // DP we need to do it in a different way?: i think so...
        for (size_t i=0; i<c_models.size(); i++)
        {
            if (models[i].delta > 1000)
                models[i].weight = 0.0;
            else
                models[i].weight = (1.0 / models[i].score) / sumReciprocal;
        }

        delete[] bicLike;
    }
    else
    {

    }
    sort(models.begin(), models.end(), compare_score);

    for (size_t i=0; i<models.size(); i++)
    {
        models[i].delta = models[i].score - models[0].score;
        sum_exp += exp(-0.5 * models[i].delta);
    }

    importance_freqs = 0.0;
    importance_inv = 0.0;
    importance_gamma = 0.0;
    importance_gamma_inv = 0.0;
    for (size_t i=0; i<models.size(); i++)
    {
        /* weights */
        models[i].weight = exp(-0.5 * models[i].delta) / sum_exp;
        if(models[i].model->is_I())
            importance_inv += models[i].weight;
        if(models[i].model->is_G())
            importance_gamma += models[i].weight;
        if(models[i].model->is_I() && models[i].model->is_G())
            importance_gamma_inv += models[i].weight;
        if(models[i].model->is_F())
            importance_freqs += models[i].weight;
    }
}

size_t ModelSelection::size() const
{
    return models.size();
}

const selection_model & ModelSelection::get_model(size_t i) const
{
    assert (i < models.size());
    return models[i];
}

void ModelSelection::print(ostream &out, mt_size_t limit)
{
    mt_size_t n_models = limit;
    out << endl
        << setw(8) << left << ic_name << "  "
        << setw(15) << left << "model"
        << setw(15) << right << "lnL"
        << setw(15) << "score"
        << setw(15) << "delta"
        << setw(10) << "weight"
        << endl;
    out << setw(80) << setfill('-') << "" << setfill(' ') << endl;
    if (n_models<=0 || (size_t) n_models > models.size())
        n_models = (mt_size_t) models.size();
    for (size_t i=0; i<n_models; i++)
    {
        out << setprecision(4)
            << setw(8) << right << i+1 << "  "
            << setw(15) << left << models[i].model->get_name()
            << setw(15) << right << models[i].model->get_lnl()
            << setw(15) << models[i].score
            << setw(15) << models[i].delta
            << setw(10) << models[i].weight
            << endl;
    }
    out << setw(80) << setfill('-') << "" << setfill(' ') << endl;
}

void ModelSelection::print_best_model(std::ostream  &out)
{
    selection_model &best_sel_model = models[0];
    Model * best_model = best_sel_model.model;
    best_model->print(out);
    out << setw(PRINTMODEL_TABSIZE) << left << "Score:" << best_sel_model.score << endl
        << setw(PRINTMODEL_TABSIZE) << left << "Weight:" << best_sel_model.weight << endl;
}

void ModelSelection::print_importances(std::ostream  &out)
{
    out << setw(PRINTMODEL_TABSIZE) << left << "P.Inv:" << importance_inv << endl
        << setw(PRINTMODEL_TABSIZE) << left << "Gamma:" << importance_gamma << endl
        << setw(PRINTMODEL_TABSIZE) << left << "Gamma-Inv:" << importance_gamma_inv << endl
        << setw(PRINTMODEL_TABSIZE) << left << "Frequencies:" << importance_freqs << endl;
}
}
