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

#include "treepll.h"
#include "model_selection.h"

#include <cmath>
#include <cassert>
#include <iomanip>
#include <algorithm>

namespace modeltest {

using namespace std;

static bool compare_score(const selection_model &m1, const selection_model &m2)
{
    return m1.score < m2.score;
}

ModelSelection::ModelSelection(const vector<Model *> &c_models,
                               ic_type type)
{
  mt_size_t n_states = c_models[0]->get_n_states();
  double sum_exp = 0.0;
  models.resize(c_models.size());
  for (size_t i=0; i<c_models.size(); i++)
  {
      models[i].model = c_models[i];
      switch(type)
      {
      case ic_lnl:
          ic_name = "lnL";
          models[i].score = models[i].model->get_loglh();
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
      for (mt_index_t i=0; i<c_models.size(); i++)
      {
          Model * model1 = models[i].model;
          sum = 0.0;
          for (mt_index_t j=0; j<c_models.size(); j++)
          {
              Model * model2 = models[j].model;

              double distance = TreePll::compute_euclidean_distance(model1->get_tree(), model2->get_tree());
              /* TODO: compute distance between trees in a table*/
              // distance = distances.getDistance(model1.getTree(), model2.getTree());

              assert(distance >= 0);

              /* sum(exp(log(dij) − bic[j] + min_bic)) */
              sum += distance * bicLike[j];
          }

          models[i].score = sum / denom;
          if (models[i].score < minDT)
          {
              minDT = models[i].score;
          }
      }

      // Calculate DT differences
      sumReciprocal = sum = 0;
      for (mt_index_t i=0; i<c_models.size(); i++)
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

  sort(models.begin(), models.end(), compare_score);

  for (mt_index_t i=0; i<models.size(); i++)
  {
      models[i].delta = models[i].score - models[0].score;
      sum_exp += exp(-0.5 * models[i].delta);
  }

  importance_freqs     = 0.0;
  importance_inv       = 0.0;
  importance_gamma     = 0.0;
  importance_gamma_inv = 0.0;
  avg_frequencies.resize(n_states);
  fill(avg_frequencies.begin(), avg_frequencies.end(), 0.0);
  fill(avg_rates.begin(), avg_rates.end(), 0.0);
  avg_pinv        = 0.0;
  avg_gamma       = 0.0;
  avg_pinv_gamma  = 0.0;
  avg_gamma_pinv  = 0.0;
  for (mt_index_t i=0; i<models.size(); i++)
  {
    Model * model = models[i].model;

    /* weights */
    models[i].weight = exp(-0.5 * models[i].delta) / sum_exp;
    if(model->is_I()&& !model->is_G())
    {
      avg_pinv += model->get_prop_inv() * models[i].weight;
      importance_inv += models[i].weight;
    }
    if(model->is_G() && !model->is_I())
    {
      avg_gamma += model->get_alpha() * models[i].weight;
      importance_gamma += models[i].weight;
    }
    if(model->is_I() && model->is_G())
    {
      avg_pinv_gamma += model->get_prop_inv() * models[i].weight;
      avg_gamma_pinv += model->get_alpha() * models[i].weight;
      importance_gamma_inv += models[i].weight;
    }
    if(model->is_F())
    {
      for (mt_index_t j=0; j<n_states; ++j)
        avg_frequencies[j] += model->get_frequencies()[j] * models[i].weight;
      importance_freqs += models[i].weight;
    }
    //TODO: We need to compute the importance of every rate for doing this
  }
  avg_gamma /= importance_gamma;
  avg_pinv /= importance_inv;
  avg_pinv_gamma /= importance_gamma_inv;
  avg_gamma_pinv /= importance_gamma_inv;
  for (mt_index_t j=0; j<n_states; ++j)
    avg_frequencies[j] /= importance_freqs;

  //mt_size_t n_subst_rates = models[0].model->get_n_subst_rates();
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

double ModelSelection::get_weight(Model * model) const
{
  for (selection_model m : models)
    if (m.model == model)
      return m.weight;

  assert(0);
  return 0;
}

void ModelSelection::print(ostream &out, mt_size_t limit) const
{
    mt_size_t n_models = limit;
    out << endl
        << setw(8)  << left << ic_name << "  "
        << setw(15) << left << "model"
        << setw(5)  << right << "K"
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
        out << fixed << setprecision(MT_PRECISION_DIGITS)
            << setw(8)  << right << i+1 << "  "
            << setw(15) << left << models[i].model->get_name()
            << setw(5)  << right << models[i].model->get_n_free_variables()
            << setw(15) << right << models[i].model->get_loglh()
            << setw(15) <<  models[i].score
            << setw(15) << models[i].delta
            << setw(10) << models[i].weight
            << endl;
    }
    out << setw(80) << setfill('-') << "" << setfill(' ') << endl;
}

void ModelSelection::print_xml(std::ostream  &out, mt_size_t limit) const
{
    mt_size_t n_models = limit;
    if (n_models<=0 || (size_t) n_models > models.size())
        n_models = (mt_size_t) models.size();

    out << "<selection type=\""<<ic_name<<"\">" << endl;
    for (size_t i=0; i<n_models; i++)
    {
      out << fixed << setprecision(MT_PRECISION_DIGITS)
          << "  <model rank=\"" << i+1
          << "\" name=\"" << models[i].model->get_name()
          << "\" lnL=\"" << models[i].model->get_loglh()
          << "\" score=\"" << models[i].score
          << "\" delta=\"" << models[i].delta
          << "\" weight=\"" << models[i].weight
          << "\" />" << endl;
    }
    out << "</selection>" << endl;
}

void ModelSelection::print_best_model(std::ostream  &out) const
{
    const selection_model &best_sel_model = models[0];
    Model * best_model = best_sel_model.model;
    best_model->print(out);
    out << fixed << setprecision(MT_PRECISION_DIGITS)
        << setw(PRINTMODEL_TABSIZE) << left << "Score:" << best_sel_model.score << endl
        << setw(PRINTMODEL_TABSIZE) << left << "Weight:" << best_sel_model.weight << endl;
}

void ModelSelection::print_inline_header(std::ostream &out)
{
    out << setw(10) << " ";
    out << setw(20) << "Model";
    out << setw(14) << "Score";
    out << setw(14) << "Weight";
    out << endl;
    out << setfill('-') << setw(10+20+14+14) << "" << setfill(' ') << endl;
}

void ModelSelection::print_inline_best_model(ic_type type, selection_model &model, std::ostream &out)
{
    switch (type)
    {
    case ic_bic:
        out << setw(10) << "  BIC";
        break;
    case ic_aic:
        out << setw(10) << "  AIC";
        break;
    case ic_aicc:
        out << setw(10) << "  AICc";
        break;
    case ic_dt:
        out << setw(10) << "  DT";
        break;
    case ic_lnl:
        out << setw(10) << "  lnL";
        break;
    }

    out << setw(20) << model.model->get_name();
    out << fixed << setprecision(MT_PRECISION_DIGITS);
    out << setw(14) << model.score;
    out << setw(14) << model.weight;
    out << endl;
}

void ModelSelection::print_importances(std::ostream  &out) const
{
  out << fixed << setprecision(MT_PRECISION_DIGITS);
  out << setw(PRINTMODEL_TABSIZE) << left << "P.Inv:" << importance_inv << endl
      << setw(PRINTMODEL_TABSIZE) << left << "Gamma:" << importance_gamma << endl
      << setw(PRINTMODEL_TABSIZE) << left << "Gamma-Inv:" << importance_gamma_inv << endl
      << setw(PRINTMODEL_TABSIZE) << left << "Frequencies:" << importance_freqs << endl;
}

void ModelSelection::print_averages(std::ostream  &out) const
{
  out << fixed << setprecision(MT_PRECISION_DIGITS);
  out << setw(PRINTMODEL_TABSIZE) << left << "P.Inv:" << avg_pinv << endl
      << setw(PRINTMODEL_TABSIZE) << left << "Alpha:" << avg_gamma << endl
      << setw(PRINTMODEL_TABSIZE) << left << "Alpha-P.Inv:" << avg_gamma_pinv << endl
      << setw(PRINTMODEL_TABSIZE) << left << "P.Inv-Alpha:" << avg_pinv_gamma << endl
      << setw(PRINTMODEL_TABSIZE) << left << "Frequencies:";
  for (mt_index_t j=0; j<avg_frequencies.size(); ++j)
    out  << avg_frequencies[j] << " ";
  out << endl;
}
}
