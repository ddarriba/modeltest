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

#ifndef RESULTSDIALOG_H
#define RESULTSDIALOG_H

#include "../model_selection.h"
#include "../partitioning_scheme.h"

#include <QWidget>
#include <vector>
#include <map>

namespace Ui {
class ResultsDialog;
}

class ResultsDialog : public QWidget
{
    Q_OBJECT

public:
    explicit ResultsDialog(modeltest::PartitioningScheme &scheme,
                           std::string const& base_name,
                           QWidget *parent = 0);
    ~ResultsDialog();

private slots:
    void on_tool_results_export_clicked();

    void on_cmb_results_partition_currentIndexChanged(int index);

private:
    Ui::ResultsDialog *ui;
    std::string const& base_name;
    std::vector<std::map<modeltest::ic_type, modeltest::ModelSelection *>> model_selection;
};

#endif // RESULTSDIALOG_H
