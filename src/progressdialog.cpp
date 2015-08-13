#include "progressdialog.h"
#include "ui_progressdialog.h"

#include <iostream>
#include <cstdio>
#include <QMessageBox>

namespace modeltest {

ProgressDialog::ProgressDialog(int n_models, QWidget *parent) :
    QDialog(parent),
    n_models(n_models),
    ui(new Ui::ProgressDialog)
{
    cur_model = 0;
    ui->setupUi(this);
    ui->progressBar->setRange(0, n_models);
    start_time = time(NULL);
}

ProgressDialog::~ProgressDialog()
{
    emit canceled();
    delete ui;
}

void ProgressDialog::addModel( Model * m )
{
    char nextline[100];
    sprintf(nextline, "%3d/%3d %10s %.4f %lds",
            ++cur_model, n_models, m->get_name().c_str(), m->get_lnl(), time(NULL) - start_time);
    ui->textBrowser->append(QString(nextline));
    std::cout << nextline << std::endl;
}

void ProgressDialog::setValue( int value )
{
    ui->progressBar->setValue(value);
}

void ProgressDialog::setLabelText( QString text )
{
    ui->label->setText( text );
}

void ProgressDialog::reset()
{
    close();
}

}

void modeltest::ProgressDialog::on_btnStop_clicked()
{
    QMessageBox msgBox;
    msgBox.setText("Stop execution");
    msgBox.setInformativeText("Are you sure?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    if (ret == QMessageBox::Ok)
    {
        emit canceled();
        close();
    }
    //emit canceled();
}
