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
#ifndef DATAINFODIALOG_H
#define DATAINFODIALOG_H

#include "global_defs.h"

#include <QWidget>

namespace Ui {
class DataInfoDialog;
}

class DataInfoDialog : public QWidget
{
    Q_OBJECT

public:
    explicit DataInfoDialog(std::string const& msa_filename,
                            mt_size_t n_taxa,
                            mt_size_t n_sites,
                            std::string const& tree_filename,
                            double tree_len,
                            partitioning_scheme_t &scheme,
                            std::string const& parts_filename,
                            QWidget *parent = 0);
    ~DataInfoDialog();

private:
    Ui::DataInfoDialog *ui;
};

#endif // DATAINFODIALOG_H
