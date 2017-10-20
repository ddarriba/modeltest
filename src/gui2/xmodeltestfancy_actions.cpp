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

#include "xmodeltestfancy.h"
#include "../modeltest.h"
#include "../utils.h"

#include "ui_xmodeltestfancy.h"

#ifdef QT_WIDGETS_LIB
#include <QtWidgets>
#else
#include <QtGui/QFileDialog>
#endif

void XModelTestFancy::on_radDatatypeDna_clicked()
{
    //ui->grpSubstSchemes->setVisible(true);
    ui->grpSubstSchemes->setEnabled(true);
    ui->cbEqualFreq->setText("Equal frequencies");
    ui->cbMlFreq->setText("ML frequencies");
    ui->modelsListView->clear();

    ui->modelsListView->addItem("000000  JC / F81");
    ui->modelsListView->addItem("010010  K80 / HKY85");
    ui->modelsListView->addItem("010020  TrNef / TrN");
    ui->modelsListView->addItem("012210  TPM1 / TPM1uf");
    ui->modelsListView->addItem("010212  TPM2 / TPM2uf");
    ui->modelsListView->addItem("012012  TPM3 / TPM3uf");
    ui->modelsListView->addItem("012230  TIM1ef / TIM1");
    ui->modelsListView->addItem("010232  TIM2ef / TIM2");
    ui->modelsListView->addItem("012032  TIM3ef / TIM3");
    ui->modelsListView->addItem("012314  TVMef / TVM");
    ui->modelsListView->addItem("012345  SYM / GTR");

    ui->modelsListView->setMinimumHeight(250);
    ui->modelsListView->setMaximumHeight(250);
    on_radSchemes11_clicked();

    update_gui();
}

void XModelTestFancy::on_radDatatypeProt_clicked()
{
    ui->grpSubstSchemes->setEnabled(false);
    ui->cbEqualFreq->setText("Model frequencies");
    ui->cbMlFreq->setText("Empirical frequencies");
    ui->modelsListView->clear();
    for (mt_index_t i=0; i<N_PROT_MODEL_MATRICES; i++)
    {
        ui->modelsListView->addItem(prot_model_names[i].c_str());
        ui->modelsListView->item(i)->setCheckState(Qt::CheckState::Checked);
    }

    ui->modelsListView->setMinimumHeight(363);
    ui->modelsListView->setMaximumHeight(363);
    ui->modelsListView->selectAll();

    update_gui();
}

void XModelTestFancy::on_cbNoRateVarModels_toggled(bool checked)
{
    UNUSED(checked);
    update_gui();
}

void XModelTestFancy::on_cbGModels_toggled(bool checked)
{
    UNUSED(checked);
    update_gui();
}

void XModelTestFancy::on_cbIGModels_toggled(bool checked)
{
    UNUSED(checked);
    update_gui();
}

void XModelTestFancy::on_cbIModels_toggled(bool checked)
{
    UNUSED(checked);
    update_gui();
}

void XModelTestFancy::on_cbEqualFreq_toggled(bool checked)
{
    UNUSED(checked);
    update_gui();
}

void XModelTestFancy::on_cbMlFreq_toggled(bool checked)
{
    UNUSED(checked);
    update_gui();
}

void XModelTestFancy::on_radSchemes3_clicked() { set_substitution_schemes(3); }
void XModelTestFancy::on_radSchemes5_clicked() { set_substitution_schemes(5); }
void XModelTestFancy::on_radSchemes7_clicked() { set_substitution_schemes(7); }
void XModelTestFancy::on_radSchemes11_clicked() { set_substitution_schemes(11); }
void XModelTestFancy::on_radSchemes203_clicked() { set_substitution_schemes(203); }
void XModelTestFancy::on_radSchemesUser_clicked() { update_gui(); }

void XModelTestFancy::on_modelsListView_itemPressed(QListWidgetItem *item)
{
    on_modelsListView_itemClicked(item);
}
void XModelTestFancy::on_modelsListView_itemClicked(QListWidgetItem *item)
{
    UNUSED(item);
    ui->radSchemesUser->setChecked(true);
    update_gui();
}

void XModelTestFancy::autoSelectSchemes(const int schemes[], int n)
{

    if (n == 203)
    {
        for (int i=0; i < ui->modelsListView->count(); i++)
            ui->modelsListView->item(i)->setCheckState(Qt::CheckState::Unchecked);
    }
    else
    {
        for (int i=0; i < ui->modelsListView->count(); i++)
            ui->modelsListView->item(i)->setCheckState(Qt::CheckState::Unchecked);

        for (int i=0; i<n; i++)
            ui->modelsListView->item(schemes[i])->setCheckState(Qt::CheckState::Checked);
    }
    update_gui();
}

void XModelTestFancy::set_substitution_schemes(mt_index_t n_schemes)
{
    if (ui->radDatatypeDna->isChecked())
    {
        int schemes[11] = {0, 1, 10,    /* 3 schemes */
                           2, 3,        /* 5 schemes */
                           6, 9,        /* 7 schemes */
                           4, 5, 7, 8}; /* 11 schemes */
        switch(n_schemes)
        {
        case 3:
            ui->radSchemes3->setChecked(true);
            break;
        case 5:
            ui->radSchemes5->setChecked(true);
            break;
        case 7:
            ui->radSchemes7->setChecked(true);
            break;
        case 11:
            ui->radSchemes11->setChecked(true);
            break;
        case 203:
            ui->radSchemes203->setChecked(true);
            break;
        default:
            assert(0);
        }

        autoSelectSchemes(schemes, n_schemes);
    }
}

void XModelTestFancy::on_sliderNCat_sliderMoved(int position)
{
    char txt[30];
    sprintf(txt, "%d categories", position);
    ui->lblNCat->setText(QString(txt));

    compute_size(position, ui->slider_nthreads->value());
}

void XModelTestFancy::on_sliderNCat_valueChanged(int value)
{
    on_sliderNCat_sliderMoved(value);
}

void XModelTestFancy::on_radSetMrbayes_toggled(bool checked)
{
    if (!checked)
        return;
    on_radSchemes3_clicked();
    ui->cbMlFreq->setChecked(true);
    ui->cbEqualFreq->setChecked(true);
    ui->cbNoRateVarModels->setChecked(true);
    ui->cbGModels->setChecked(true);
    ui->cbIModels->setChecked(true);
    ui->cbIGModels->setChecked(true);
    update_gui();
}

void XModelTestFancy::on_radSetRaxml_toggled(bool checked)
{
    if (!checked)
        return;
    on_radSchemes3_clicked();
    ui->cbMlFreq->setChecked(true);
    ui->cbEqualFreq->setChecked(true);
    ui->cbNoRateVarModels->setChecked(false);
    ui->cbGModels->setChecked(true);
    ui->cbIModels->setChecked(false);
    ui->cbIGModels->setChecked(false);
    ui->sliderNCat->setValue(4);
    on_sliderNCat_sliderMoved(4);
    update_gui();
}

void XModelTestFancy::on_radSetPhyml_toggled(bool checked)
{
    if (!checked)
        return;
    on_radSchemes11_clicked();
    ui->cbMlFreq->setChecked(true);
    ui->cbEqualFreq->setChecked(true);
    ui->cbNoRateVarModels->setChecked(true);
    ui->cbIModels->setChecked(true);
    ui->cbGModels->setChecked(true);
    ui->cbIGModels->setChecked(true);
    update_gui();
}

void XModelTestFancy::on_radSetModelTest_toggled(bool checked)
{
    if (!checked)
        return;
    on_radSchemes11_clicked();
    ui->cbMlFreq->setChecked(true);
    ui->cbEqualFreq->setChecked(true);
    ui->cbNoRateVarModels->setChecked(true);
    ui->cbIModels->setChecked(true);
    ui->cbGModels->setChecked(true);
    ui->cbIGModels->setChecked(true);
    update_gui();
}
