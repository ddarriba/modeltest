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

#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include "model/model.h"

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
