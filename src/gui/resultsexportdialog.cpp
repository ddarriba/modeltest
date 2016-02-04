#include "resultsexportdialog.h"
#include "ui_resultsexportdialog.h"

ResultsExportDialog::ResultsExportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResultsExportDialog)
{
    ui->setupUi(this);
}

ResultsExportDialog::~ResultsExportDialog()
{
    delete ui;
}
