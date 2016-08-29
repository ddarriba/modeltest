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

#ifndef RESULTSEXPORTDIALOG_H
#define RESULTSEXPORTDIALOG_H

#include "model_selection.h"

#include <map>
#include <vector>
#include <QDialog>

namespace Ui {
class ResultsExportDialog;
}

class ResultsExportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResultsExportDialog(
            std::vector<std::map<modeltest::ic_type, modeltest::ModelSelection *>> &model_selection,
            std::string const& default_filename,
            QWidget *parent = 0);
    ~ResultsExportDialog();

private slots:
    void on_tool_outfile_clicked();

    void on_box_buttons_accepted();

    void on_slider_nmodels_valueChanged(int value);

    void on_cb_tofile_stateChanged(int newstate);

private:
    Ui::ResultsExportDialog *ui;
    std::vector<std::map<modeltest::ic_type, modeltest::ModelSelection *>> &model_selection;

    void export_results(std::ostream  &out = std::cout);
};

#endif // RESULTSEXPORTDIALOG_H
