#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include "model.h"
#include <QDialog>
#include <QString>
#include <QLabel>
#include <ctime>
#include <vector>

namespace Ui {
class ProgressDialog;
}

class ProgressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProgressDialog(int n_models, int n_threads, QWidget *parent = 0);
    ~ProgressDialog();

public slots:
    void set_progress_value( int value );
    void set_delta_running( unsigned int parameter, double value, unsigned int thread_idx );
    void set_model_running( modeltest::Model * m, unsigned int  thread_idx );
    void optimized_single_model( void );
    void reset( void );

signals:
    void canceled();

private slots:
    void on_btn_stop_clicked();

private:
    int cur_model;
    int n_models;
    int n_threads;
    Ui::ProgressDialog *ui;
    time_t start_time;
    std::vector<QLabel *> lbl_procesors;
    std::vector<QLabel *> lbl_models;
    std::vector<QLabel *> lbl_delta;

};

#endif // PROGRESSDIALOG_H
