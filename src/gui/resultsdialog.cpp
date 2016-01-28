#include "resultsdialog.h"
#include "ui_resultsdialog.h"

#include "model.h"

#include <QStandardItemModel>
#include <QTableView>
#include <QLabel>


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

        results_table_items->setItem(i, 2, new QStandardItem(QString::number(model->get_lnl(), 'f', 4)));
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


ResultsDialog::ResultsDialog(modeltest::ModelSelection & aic_selection,
                             modeltest::ModelSelection & aicc_selection,
                             modeltest::ModelSelection & bic_selection,
                             modeltest::ModelSelection & dt_selection,
                             QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ResultsDialog)
{
    ui->setupUi(this);

    //ui->frame_results_header->setVisible(scheme.get_size() > 1);

    fill_results(ui->table_results_aic, aic_selection,
                 ui->txt_imp_inv_aic, ui->txt_imp_gamma_aic,
                 ui->txt_imp_invgamma_aic, ui->txt_imp_freqs_aic);
    fill_results(ui->table_results_aicc, aicc_selection,
                 ui->txt_imp_inv_aicc, ui->txt_imp_gamma_aicc,
                 ui->txt_imp_invgamma_aicc, ui->txt_imp_freqs_aicc);
    fill_results(ui->table_results_bic, bic_selection,
                 ui->txt_imp_inv_bic, ui->txt_imp_gamma_bic,
                 ui->txt_imp_invgamma_bic, ui->txt_imp_freqs_bic);
    fill_results(ui->table_results_dt, dt_selection,
                 ui->txt_imp_inv_dt, ui->txt_imp_gamma_dt,
                 ui->txt_imp_invgamma_dt, ui->txt_imp_freqs_dt);

    for (int c = 0; c < ui->table_results_bic->horizontalHeader()->count(); ++c)
    {
        ui->table_results_bic->horizontalHeader()->setResizeMode(
            c, QHeaderView::Stretch);
        ui->table_results_aic->horizontalHeader()->setResizeMode(
            c, QHeaderView::Stretch);
        ui->table_results_aicc->horizontalHeader()->setResizeMode(
            c, QHeaderView::Stretch);
        ui->table_results_dt->horizontalHeader()->setResizeMode(
            c, QHeaderView::Stretch);
    }
}

ResultsDialog::~ResultsDialog()
{
    delete ui;
}

