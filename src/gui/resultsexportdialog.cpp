#include "resultsexportdialog.h"
#include "ui_resultsexportdialog.h"

#include <QFileDialog>
#include <fstream>

using namespace std;

ResultsExportDialog::ResultsExportDialog(
        vector<map<modeltest::ic_type, modeltest::ModelSelection *> > &model_selection,
        string const& default_filename,
        QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResultsExportDialog),
    model_selection(model_selection)
{
    int value, maximum;
    maximum = model_selection[0][modeltest::ic_aic]->size();
    value = std::min(maximum, 10);
    ui->setupUi(this);
    ui->txt_outfile->setText(default_filename.c_str());
    ui->slider_nmodels->setMinimum(1);
    ui->slider_nmodels->setMaximum(maximum);
    ui->slider_nmodels->setValue(value);
    //on_slider_nmodels_valueChanged(value);

    ui->cb_tofile->setChecked(false);
    on_cb_tofile_stateChanged(ui->cb_tofile->isChecked());
}

ResultsExportDialog::~ResultsExportDialog()
{
    delete ui;
}

void ResultsExportDialog::on_tool_outfile_clicked()
{
    QString file_name = QFileDialog::getSaveFileName(this,
                                                    tr("Export results"),
                                                    ui->txt_outfile->text());
    ui->txt_outfile->setText(file_name);
}


void ResultsExportDialog::on_slider_nmodels_valueChanged(int value)
{
    ui->lbl_slidervalue->setText(QString::number(value));
}

void ResultsExportDialog::on_cb_tofile_stateChanged(int newstate)
{
    ui->lbl_outfile->setEnabled(newstate);
    ui->txt_outfile->setEnabled(newstate);
    ui->tool_outfile->setEnabled(newstate);
}

void ResultsExportDialog::export_results(std::ostream  &out)
{
    mt_size_t limit = ui->slider_nmodels->value();

    for (mt_index_t i=0; i<model_selection.size(); ++i)
    {
        if (ui->rad_human->isChecked())
        {
            if (ui->cb_aic->isChecked())
                model_selection[i][modeltest::ic_aic]->print(out, limit);
            if (ui->cb_aicc->isChecked())
                model_selection[i][modeltest::ic_aicc]->print(out, limit);
            if (ui->cb_bic->isChecked())
                model_selection[i][modeltest::ic_bic]->print(out, limit);
            if (ui->cb_dt->isChecked())
                model_selection[i][modeltest::ic_dt]->print(out, limit);
        }
        else
        {
            if (ui->cb_aic->isChecked())
                model_selection[i][modeltest::ic_aic]->print_xml(out, limit);
            if (ui->cb_aicc->isChecked())
                model_selection[i][modeltest::ic_aicc]->print_xml(out, limit);
            if (ui->cb_bic->isChecked())
                model_selection[i][modeltest::ic_bic]->print_xml(out, limit);
            if (ui->cb_dt->isChecked())
                model_selection[i][modeltest::ic_dt]->print_xml(out, limit);
        }
    }
}

void ResultsExportDialog::on_box_buttons_accepted()
{
    if (ui->cb_toconsole->isChecked())
        export_results(cout);

    if (ui->cb_tofile->isChecked())
    {
        const std::string save_filename = ui->txt_outfile->text().toStdString();

        if ( save_filename.compare(""))
        {
            //TODO: check valid file, create foutputstream and export there!
            ofstream myfile;
            myfile.open (save_filename);
            export_results(myfile);
            myfile.close();
        }
    }
}
