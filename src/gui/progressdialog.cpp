#include "progressdialog.h"
#include "ui_progressdialog.h"

#include <iostream>
#include <cstdio>
#include <QFrame>
#include <QMessageBox>
#include <QHBoxLayout>

ProgressDialog::ProgressDialog(int n_models, int n_threads, QWidget *parent) :
    QDialog(parent),
    n_models(n_models),
    n_threads(n_threads),
    ui(new Ui::ProgressDialog)
{
    cur_model = 0;
    ui->setupUi(this);
    ui->progress_bar->setRange(0, n_models);
    ui->progress_bar->setValue(0);
    start_time = time(NULL);

    char thread_info[250];
    sprintf(thread_info, "thread 1/%d", n_threads);
    ui->lbl_thread1->setText(thread_info);

    lbl_models.push_back(ui->lbl_model1);
    lbl_delta.push_back(ui->lbl_delta1);
    lbl_procesors.push_back(ui->lbl_thread1);

    // add items for other threads
    for (int i=1; i<n_threads; i++)
    {
        QFrame * new_frame = new QFrame();
        new_frame->setLineWidth(1);
        new_frame->setFrameShape(QFrame::StyledPanel);
        new_frame->setFrameShadow(QFrame::Raised);

        QHBoxLayout * horizontal_layout = new QHBoxLayout(new_frame);
        horizontal_layout->setObjectName(QString::fromUtf8("horizontal_layout"));
        horizontal_layout->setContentsMargins(9, 9, 9, 9);

        QLabel * new_labmodel  = new QLabel(new_frame);
        QLabel * new_labthread = new QLabel(new_frame);
        QLabel * new_labdelta  = new QLabel(new_frame);
        sprintf(thread_info, "thread %d/%d", i+1, n_threads);

        new_labmodel->setText("iddle");
        new_labmodel->setMinimumWidth(150);
        new_labmodel->setMaximumWidth(150);
        lbl_models.push_back(new_labmodel);

        new_labthread->setText(thread_info);
        new_labthread->setMinimumWidth(120);
        new_labthread->setMaximumWidth(120);
        lbl_procesors.push_back(new_labthread);

        new_labdelta->setText("n/a");
        new_labdelta->setMinimumWidth(150);
        new_labdelta->setMaximumWidth(150);
        lbl_delta.push_back(new_labdelta);


        horizontal_layout->addWidget(new_labthread);
        horizontal_layout->addWidget(new_labmodel);
        horizontal_layout->addWidget(new_labdelta);
        //new_frame->setLayout(QLayout::fgre);

        ui->verticalLayout_2->addWidget(new_frame);
    }
    ui->verticalLayout_2->addWidget(ui->btn_stop);
}

ProgressDialog::~ProgressDialog()
{
    emit canceled();
    delete ui;
}

void ProgressDialog::optimized_single_model( void )
{
    ui->progress_bar->setValue(ui->progress_bar->value()+1);
}

void ProgressDialog::set_progress_value( int value )
{
    ui->progress_bar->setValue(value);
}

void ProgressDialog::set_delta_running( unsigned int parameter, double value, unsigned int thread_idx )
{
    lbl_delta[thread_idx]->setText( QString::number(value, 'f', 4) );
    //ui->label->setText( text );
}

void ProgressDialog::set_model_running(modeltest::Model * m, unsigned int thread_idx )
{
    lbl_models[thread_idx]->setText( m->get_name().c_str() );
}

void ProgressDialog::reset()
{
    close();
}

void ProgressDialog::on_btn_stop_clicked()
{
    QMessageBox msgBox;
    msgBox.setText("Stop execution");
    msgBox.setInformativeText("Are you sure?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    if (ret == QMessageBox::Ok)
    {
        close();
    }
}
