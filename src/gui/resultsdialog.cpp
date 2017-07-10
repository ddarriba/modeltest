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

#include "gui/resultsdialog.h"
#include "gui/resultsexportdialog.h"
#include "service/modeltestservice.h"
#include "model/model.h"

#include "ui_resultsdialog.h"


#ifdef QT_WIDGETS_LIB
#include <QtWidgets>
#else
#include <QtGui/QFileDialog>
#include <QStandardItemModel>
#include <QTableView>
#include <QLabel>
#endif

#include <sstream>


static void fill_results(QTableView * result_table,
                         modeltest::ModelSelection &model_selection,
                         QLabel *imp_inv, QLabel *imp_gamma, QLabel *imp_gammainv, QLabel *imp_freqs)
{
    QStandardItemModel * results_table_items;
    double cum_weight = 0.0;

    if (result_table->model() != NULL)
    {
        delete result_table->model();
    }
    results_table_items = new QStandardItemModel(0,7);

    int cur_column = 0;
    results_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("Model")));
    results_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("K")));

    results_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("lnL")));
    results_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("score")));
    results_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("delta")));
    results_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("weight")));
    results_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("cum weight")));
    result_table->setModel(results_table_items);

    for (size_t i=0; i<model_selection.size(); i++)
    {
        modeltest::Model * model = model_selection.get_model(i).model;
        results_table_items->setItem(i, 0, new QStandardItem(QString(model->get_name().c_str())));
        results_table_items->setItem(i, 1, new QStandardItem(QString::number(model->get_n_free_variables())));

        results_table_items->setItem(i, 2, new QStandardItem(QString::number(model->get_loglh(), 'f', 4)));
        results_table_items->setItem(i, 3, new QStandardItem(QString::number(model_selection.get_model(i).score, 'f', 4)));
        results_table_items->setItem(i, 4, new QStandardItem(QString::number(model_selection.get_model(i).delta, 'f', 4)));
        results_table_items->setItem(i, 5, new QStandardItem(QString::number(model_selection.get_model(i).weight, 'f', 4)));
        cum_weight += model_selection.get_model(i).weight;
        results_table_items->setItem(i, 6, new QStandardItem(QString::number(cum_weight, 'f', 4)));
    }

    if (imp_gamma)
        imp_gamma->setText(QString::number(model_selection.get_importance_gamma()));
    if (imp_inv)
        imp_inv->setText(QString::number(model_selection.get_importance_inv()));
    if (imp_gammainv)
        imp_gammainv->setText(QString::number(model_selection.get_importance_gamma_inv()));
    if (imp_freqs)
        imp_freqs->setText(QString::number(model_selection.get_importance_freqs()));
}


ResultsDialog::ResultsDialog(modeltest::PartitioningScheme &scheme,
                             std::string const& base_name,
                             QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ResultsDialog),
    base_name(base_name)
{
    ui->setupUi(this);

    ui->cmb_results_partition->setVisible(scheme.get_size() > 1);
    model_selection.resize(scheme.get_size());
    for (mt_index_t i=0; i<scheme.get_size(); ++i)
    {
      model_selection[i][modeltest::ic_aic]  =  ModelTestService::instance()->select_models(scheme.get_partition(i).get_id(), modeltest::ic_aic);
      model_selection[i][modeltest::ic_aicc] =  ModelTestService::instance()->select_models(scheme.get_partition(i).get_id(), modeltest::ic_aicc);
      model_selection[i][modeltest::ic_bic]  =  ModelTestService::instance()->select_models(scheme.get_partition(i).get_id(), modeltest::ic_bic);

      //TODO: Allow this only for fixed trees
      if (false)
        model_selection[i][modeltest::ic_dt]   =  ModelTestService::instance()->select_models(scheme.get_partition(i).get_id(), modeltest::ic_dt);
    }

    fill_results(ui->table_results_aic, *model_selection[0][modeltest::ic_aic],
                 ui->txt_imp_inv_aic, ui->txt_imp_gamma_aic,
                 ui->txt_imp_invgamma_aic, ui->txt_imp_freqs_aic);
    fill_results(ui->table_results_aicc, *model_selection[0][modeltest::ic_aicc],
                 ui->txt_imp_inv_aicc, ui->txt_imp_gamma_aicc,
                 ui->txt_imp_invgamma_aicc, ui->txt_imp_freqs_aicc);
    fill_results(ui->table_results_bic, *model_selection[0][modeltest::ic_bic],
                 ui->txt_imp_inv_bic, ui->txt_imp_gamma_bic,
                 ui->txt_imp_invgamma_bic, ui->txt_imp_freqs_bic);
    //TODO: Allow this only for fixed trees
    if (false)
      fill_results(ui->table_results_dt, *model_selection[0][modeltest::ic_dt],
                   ui->txt_imp_inv_dt, ui->txt_imp_gamma_dt,
                   ui->txt_imp_invgamma_dt, ui->txt_imp_freqs_dt);

    for (int c = 0; c < ui->table_results_bic->horizontalHeader()->count(); ++c)
    {
#ifdef QT_WIDGETS_LIB
        ui->table_results_bic->horizontalHeader()->setSectionResizeMode(
            c, QHeaderView::Stretch);
        ui->table_results_aic->horizontalHeader()->setSectionResizeMode(
            c, QHeaderView::Stretch);
        ui->table_results_aicc->horizontalHeader()->setSectionResizeMode(
            c, QHeaderView::Stretch);
        ui->table_results_dt->horizontalHeader()->setSectionResizeMode(
            c, QHeaderView::Stretch);
#else
        ui->table_results_bic->horizontalHeader()->setResizeMode(
            c, QHeaderView::Stretch);
        ui->table_results_aic->horizontalHeader()->setResizeMode(
            c, QHeaderView::Stretch);
        ui->table_results_aicc->horizontalHeader()->setResizeMode(
            c, QHeaderView::Stretch);
        ui->table_results_dt->horizontalHeader()->setResizeMode(
            c, QHeaderView::Stretch);
#endif
    }
}

ResultsDialog::~ResultsDialog()
{
    for (mt_index_t i=0; i<model_selection.size(); ++i)
    {
        delete model_selection[i][modeltest::ic_aic];
        delete model_selection[i][modeltest::ic_aicc];
        delete model_selection[i][modeltest::ic_bic];

        //TODO: Allow this only for fixed trees
        if (false)
          delete model_selection[i][modeltest::ic_dt];
    }
    delete ui;
}


void ResultsDialog::on_tool_results_export_clicked()
{
    std::stringstream default_filename;
    default_filename << base_name << ".results.txt";
    ResultsExportDialog exportdialog(model_selection, default_filename.str());
    exportdialog.exec();
}

void ResultsDialog::on_cmb_results_partition_currentIndexChanged(int index)
{
    fill_results(ui->table_results_aic, *model_selection[index][modeltest::ic_aic],
                 ui->txt_imp_inv_aic, ui->txt_imp_gamma_aic,
                 ui->txt_imp_invgamma_aic, ui->txt_imp_freqs_aic);
    fill_results(ui->table_results_aicc, *model_selection[index][modeltest::ic_aicc],
                 ui->txt_imp_inv_aicc, ui->txt_imp_gamma_aicc,
                 ui->txt_imp_invgamma_aicc, ui->txt_imp_freqs_aicc);
    fill_results(ui->table_results_bic, *model_selection[index][modeltest::ic_bic],
                 ui->txt_imp_inv_bic, ui->txt_imp_gamma_bic,
                 ui->txt_imp_invgamma_bic, ui->txt_imp_freqs_bic);
    //TODO: Allow this only for fixed trees
    if (false)
      fill_results(ui->table_results_dt, *model_selection[index][modeltest::ic_dt],
                   ui->txt_imp_inv_dt, ui->txt_imp_gamma_dt,
                   ui->txt_imp_invgamma_dt, ui->txt_imp_freqs_dt);
}
