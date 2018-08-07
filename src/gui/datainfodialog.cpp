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

#include "../gui/datainfodialog.h"
#include "../gui/xutils.h"

#include "ui_datainfodialog.h"

#include <cstdio>

DataInfoDialog::DataInfoDialog(std::string const& msa_filename,
                               mt_size_t n_taxa,
                               mt_size_t n_sites,
                               std::string const& tree_filename,
                               double tree_len,
                               partitioning_scheme_t &scheme,
                               std::string const&  parts_filename,
                               QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataInfoDialog)
{
    ui->setupUi(this);

    char line[500];
    sprintf(line, "MSA filename           %s", msa_filename.c_str());
    ui->text->append(line);
    ui->text->append("");
    sprintf(line, "Number of sequences        %d", n_taxa);
    ui->text->append(line);
    sprintf(line, "Sequence length            %d", n_sites);
    ui->text->append(line);
    ui->text->append("");

    if (tree_filename.length())
    {
        sprintf(line, "Tree filename          %s", tree_filename.c_str());
        ui->text->append(line);
        ui->text->append("");
        sprintf(line, "Tree length                %.4f", tree_len);
        ui->text->append(line);
        ui->text->append("");
    }
    else
    {
        sprintf(line, "No user tree defined");
        ui->text->append(line);
    }

    if (parts_filename.length())
    {
        sprintf(line, "Partitions filename    %s", parts_filename.c_str());
        ui->text->append(line);
        ui->text->append("");
        mt_size_t n_dna_parts = 0, n_aa_parts = 0;
        for (partition_descriptor_t partition : scheme)
            if (partition.datatype == dt_dna)
                n_dna_parts++;
            else
                n_aa_parts++;
        sprintf(line, "Number of partitions       %ld (DNA: %d ; Protein %d)", scheme.size(), n_dna_parts, n_aa_parts);
        ui->text->append(line);
    }
    else
    {
        sprintf(line, "No partitioning defined");
        ui->text->append(line);
    }
}

DataInfoDialog::~DataInfoDialog()
{
    delete ui;
}
