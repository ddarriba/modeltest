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

#ifndef MODELSDIALOG_H
#define MODELSDIALOG_H

#include "partitioning_scheme.h"

#include <QWidget>
#include <QStandardItemModel>


namespace Ui {
class ModelsDialog;
}

class ModelsDialog : public QWidget
{
    Q_OBJECT

public:
    explicit ModelsDialog(modeltest::PartitioningScheme &scheme,
                          QWidget *parent = 0);
    ~ModelsDialog();

private slots:
    void on_cmb_partition_currentIndexChanged(int index);

    void on_action_close_triggered();

    void on_toolButton_clicked();

private:
    void fill_header( data_type_t type = dt_dna );
    void fill_data( modeltest::Partition &partition );

    Ui::ModelsDialog *ui;
    modeltest::PartitioningScheme &scheme;
    QStandardItemModel * models_table_items;
};

#endif // MODELSDIALOG_H
