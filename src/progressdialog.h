#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include "model.h"
#include <QDialog>
#include <QString>
#include <ctime>

namespace Ui {
class ProgressDialog;
}

namespace modeltest {

class ProgressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProgressDialog(int n_models, QWidget *parent = 0);
    ~ProgressDialog();

public slots:
    void setValue( int value );
    void setLabelText( QString text );
    void addModel( Model * m );
    void reset();

signals:
    void canceled();

private slots:
    void on_btnStop_clicked();

private:
    int cur_model;
    int n_models;
    Ui::ProgressDialog *ui;
    time_t start_time;
};

}
#endif // PROGRESSDIALOG_H
