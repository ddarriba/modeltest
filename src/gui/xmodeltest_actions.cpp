#include "ui_xmodeltest.h"
#include "xmodeltest.h"

#include "../utils.h"
#include "../modeltest.h"

#ifdef QT_WIDGETS_LIB
#include <QtWidgets>
#else
#include <QtGui/QFileDialog>
#endif

void xmodeltest::toggle_settings( bool value )
{
    ui->tool_settings->setChecked(value);
    ui->mnu_toggle_settings->setChecked(value);
    update_gui();
}

void xmodeltest::on_mnu_open_msa_triggered() {action_open_msa();}
void xmodeltest::on_tool_open_msa_clicked()  {action_open_msa();}

void xmodeltest::on_mnu_open_tree_triggered() {action_open_tree();}
void xmodeltest::on_tool_open_tree_clicked()  {action_open_tree();}

void xmodeltest::on_mnu_open_parts_triggered() {action_open_parts();}
void xmodeltest::on_tool_open_parts_clicked()  {action_open_parts();}

void xmodeltest::on_mnu_toggle_settings_triggered(bool checked) {
    toggle_settings(checked);
}

void xmodeltest::on_tool_settings_toggled(bool checked)
{
    toggle_settings(checked);
}

void xmodeltest::on_mnu_run_triggered()  {action_run();}
void xmodeltest::on_tool_run_clicked()  {action_run();}

void xmodeltest::on_mnu_results_triggered()  {action_viewresults();}
void xmodeltest::on_tool_results_clicked()  {action_viewresults();}

void xmodeltest::on_mnu_models_triggered()  {action_viewmodels();}
void xmodeltest::on_tool_models_clicked()  {action_viewmodels();}

void xmodeltest::on_mnu_reset_triggered() {action_reset();}
void xmodeltest::on_tool_reset_clicked()  {action_reset();}

void xmodeltest::on_mnu_view_datainfo_triggered() {action_view_datainfo();}

/* optimization settings */

void xmodeltest::on_sliderNCat_sliderMoved(int position)
{
    char txt[30];
    sprintf(txt, "%d categories", position);
    ui->lblNCat->setText(QString(txt));

    compute_size(position, ui->sliderNThreads->value());
}

void xmodeltest::on_radSchemes3_clicked() { set_substitution_schemes(3); }
void xmodeltest::on_radSchemes5_clicked() { set_substitution_schemes(5); }
void xmodeltest::on_radSchemes7_clicked() { set_substitution_schemes(7); }
void xmodeltest::on_radSchemes11_clicked() { set_substitution_schemes(11); }
void xmodeltest::on_radSchemes203_clicked() { set_substitution_schemes(203); }

void xmodeltest::on_radSetModelTest_clicked()
{
    on_radSetModelTest_toggled(true);
}

void xmodeltest::on_radSetMrbayes_clicked()
{
    on_radSetMrbayes_toggled(true);
}

void xmodeltest::on_radSetRaxml_clicked()
{
    on_radSetRaxml_toggled(true);
}

void xmodeltest::on_radSetPhyml_clicked()
{
    on_radSetPhyml_toggled(true);
}

void xmodeltest::on_radSetModelTest_toggled(bool checked)
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

void xmodeltest::on_radSetMrbayes_toggled(bool checked)
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

void xmodeltest::on_radSetRaxml_toggled(bool checked)
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

void xmodeltest::on_radSetPhyml_toggled(bool checked)
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

void xmodeltest::on_cbEqualFreq_toggled(bool checked)
{
    UNUSED(checked);
    update_gui();
}

void xmodeltest::on_cbMlFreq_toggled(bool checked)
{
    UNUSED(checked);
    update_gui();
}

void xmodeltest::on_cbNoRateVarModels_toggled(bool checked)
{
    UNUSED(checked);
    update_gui();
}

void xmodeltest::on_cbIModels_toggled(bool checked)
{
    UNUSED(checked);
    update_gui();
}

void xmodeltest::on_cbGModels_toggled(bool checked)
{
    UNUSED(checked);
    update_gui();
}

void xmodeltest::on_cbIGModels_toggled(bool checked)
{
    UNUSED(checked);
    update_gui();
}

void xmodeltest::on_cbAdvanced_clicked()
{
    update_gui();
}

void xmodeltest::on_cbShowMatrices_clicked()
{
    update_gui();
}

void xmodeltest::on_radDatatypeDna_clicked()
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

void xmodeltest::on_radDatatypeProt_clicked()
{

    //ui->grpSubstSchemes->setVisible(false);
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

void xmodeltest::on_radSchemesUser_clicked()
{
    ui->cbShowMatrices->setChecked(true);
    on_cbShowMatrices_clicked();
}

void xmodeltest::on_radTopoU_clicked()
{
    assert(ui->tool_open_tree->isEnabled());
    action_open_tree();
}

void xmodeltest::on_radTopoFixedMp_clicked()
{
    utree_filename = "";
    ui->lbl_tree->setStyleSheet("color: #007;");
    ui->lbl_tree->setText("Fixed MP");
    update_gui();
}

void xmodeltest::on_radTopoFixedGtr_clicked()
{
    utree_filename = "";
    ui->lbl_tree->setStyleSheet("color: #007;");
    ui->lbl_tree->setText("Fixed ML (GTR)");
    update_gui();
}

void xmodeltest::on_radTopoFixedJc_clicked()
{
    utree_filename = "";
    ui->lbl_tree->setStyleSheet("color: #007;");
    ui->lbl_tree->setText("Fixed ML (JC)");
    update_gui();
}

void xmodeltest::on_radTopoML_clicked()
{
    utree_filename = "";
    ui->lbl_tree->setStyleSheet("color: #007;");
    ui->lbl_tree->setText("Maximum Likelihood");
    update_gui();
}

void xmodeltest::on_modelsListView_itemClicked(QListWidgetItem *item)
{
    UNUSED(item);
    ui->radSchemesUser->setChecked(true);
    update_gui();
}

void xmodeltest::on_sliderNCat_valueChanged(int value)
{
    on_sliderNCat_sliderMoved(value);
}

void xmodeltest::on_sliderNThreads_valueChanged(int value)
{
    ui->lblNThreads->setText(QString::number(value));
    int n_cats = ui->sliderNCat->value();
    if (!(ui->cbGModels->isChecked() || ui->cbIGModels->isChecked()))
        n_cats = 1;
    compute_size(n_cats, value);
}
