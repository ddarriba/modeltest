#include "xmodeltest.h"
#include "ui_xmodeltest.h"

#include "../utils.h"
#include "../modeltest.h"

#include <QtGui/QFileDialog>

static void enable(QToolButton * button, bool new_stat, bool set = false)
{
    button->setEnabled(new_stat);
    if (new_stat)
    {
        if (set)
            button->setStyleSheet("color: #000;\nbackground-color: #66c;");
        else
            button->setStyleSheet("color: #000;\nbackground-color: #6c6;");
    }
    else
    {
        button->setStyleSheet("");
    }
}

xmodeltest::xmodeltest(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::xmodeltest)
{
    ui->setupUi(this);
    status = st_active;
    enable(ui->tool_help, true);

    on_radDatatypeDna_clicked();
    update_gui();

    /* Redirect Console output to QTextEdit */
    redirect = new Q_DebugStream(std::cout, ui->consoleRun);
    modeltest::Utils::print_header();
}

xmodeltest::~xmodeltest()
{
    delete redirect;
    delete ui;
}

void xmodeltest::update_gui( void )
{
    char txt[30];
    int n_model_sets, n_matrices, n_models;

    ui->act_open_tree->setEnabled(status & st_msa_loaded);
    ui->mnu_open_tree->setEnabled(status & st_msa_loaded);
    ui->act_open_parts->setEnabled(status & st_msa_loaded);
    ui->mnu_open_parts->setEnabled(status & st_msa_loaded);
    enable(ui->tool_open_msa, status & st_active, status & st_msa_loaded);
    enable(ui->tool_reset, status & st_active);
    enable(ui->tool_open_tree, status & st_msa_loaded, status & st_tree_loaded);
    enable(ui->tool_open_parts, status & st_msa_loaded, status & st_parts_loaded);
    enable(ui->tool_run_settings, status & st_msa_loaded, status & st_optimized);
    enable(ui->tool_settings, !(status & st_optimized), ui->tool_settings->isChecked());
    enable(ui->tool_results, status & st_optimized);

    ui->grpOptions->setVisible(ui->tool_settings->isChecked());
    ui->grpConsoles->setVisible(!ui->tool_settings->isChecked());

    /** SETTINGS **/
    ui->modelsListView->setEnabled(
                ui->radDatatypeProt->isChecked() ||
                ui->radSetModelTest->isChecked() ||
                ui->radSetPhyml->isChecked());
    ui->grpSubstSchemes->setEnabled(
                !ui->radDatatypeProt->isChecked() &&
                (ui->radSetModelTest->isChecked() ||
                ui->radSetPhyml->isChecked()));

    ui->cbIModels->setEnabled( !ui->radSetRaxml->isChecked() );
    ui->cbIGModels->setEnabled( !ui->radSetRaxml->isChecked() );

    ui->sliderNCat->setEnabled( !ui->radSetRaxml->isChecked() &&
                                ( ui->cbGModels->isChecked() ||
                                  ui->cbIGModels->isChecked()));
    ui->lblNCat->setEnabled(ui->sliderNCat->isEnabled());

    ui->grpAdvanced->setVisible(ui->cbAdvanced->isChecked());
    ui->modelsListView->setVisible(ui->cbShowMatrices->isChecked());

    ui->lblNThreads->setText(QString::number(ui->sliderNThreads->value()));

    n_model_sets = ui->cbNoRateVarModels->isChecked() +
            ui->cbIModels->isChecked() +
            ui->cbGModels->isChecked() +
            ui->cbIGModels->isChecked();

    n_matrices = 0;
    if (ui->radDatatypeDna->isChecked() && ui->radSchemes203->isChecked())
        n_matrices = N_DNA_ALLMATRIX_COUNT;
    else
        for (int i=0; i < ui->modelsListView->count(); i++)
            if (ui->modelsListView->item(i)->checkState() == Qt::CheckState::Checked)
                n_matrices++;

    n_models = n_matrices * n_model_sets *
            (ui->cbEqualFreq->isChecked() + ui->cbMlFreq->isChecked());
    sprintf(txt, "%d", n_models);
    ui->lblNumModels->setText(QString(txt));

    if (n_models == 0)
    {
        ui->lblTextNModels->setStyleSheet("background-color:#f00;");
        ui->lblNumModels->setStyleSheet("background-color:#f00;");
    }
    else
    {
        ui->lblTextNModels->setStyleSheet("");
        ui->lblNumModels->setStyleSheet("");
    }

    int n_cats = ui->sliderNCat->value();
    if (!(ui->cbGModels->isChecked() || ui->cbIGModels->isChecked()))
        n_cats = 1;
}

static QString to_qstring(const char * msg, msg_level_id level)
{
    QString line = msg;
    QString endHtml = "</font>";

    switch(level)
    {
        case msg_lvl_error: line = "<font color=\"Red\">" + line + "</font>"; break;
        case msg_lvl_alert: line = "<font color=\"DeepPink\">" + line + "</font>"; break;
        case msg_lvl_notify: line = "<font color=\"DarkGreen\">" + line + "</font>"; break;
        case msg_lvl_info: line  = "<font color=\"Black\">" + line + "</font>"; break;
        default: break;
    }

    line = line + endHtml;
    return line;
}

void xmodeltest::on_act_open_msa_triggered()
{
    QString filters = tr("Multiple Sequence Alignment(*.phy *.nex *.fas);; All files(*)");
    QString file_name = QFileDialog::getOpenFileName(this,
                                                    tr("Open File"),
                                                    "",
                                                    filters);

    const std::string loaded_file = file_name.toStdString();
    mt_size_t n_seqs, seq_len;

    if ( loaded_file.compare(""))
    {
        msa_filename = loaded_file;
        if (modeltest::ModelTest::test_msa(msa_filename,
                                 &n_seqs,
                                 &seq_len))
        {
            char text[250];

            ui->consoleMeta->append(to_qstring("Loaded alignment %1", msg_lvl_notify).arg(msa_filename.c_str()));
            sprintf(text, "Num.Sequences:   %d", n_seqs);
            ui->consoleMeta->append(to_qstring(text, msg_lvl_info));
            sprintf(text, "Sequence Length: %d", seq_len);
            ui->consoleMeta->append(to_qstring(text, msg_lvl_info));
            status |= st_msa_loaded;
        }
        else
        {
            ui->consoleMeta->append(to_qstring("Error: Cannot load alignment %1", msg_lvl_error).arg(msa_filename.c_str()));
            msa_filename   = "";
            status &= ~st_msa_loaded;
        }
    }
    update_gui();
}

void xmodeltest::on_mnu_open_msa_triggered()
{
    on_act_open_msa_triggered();
}

void xmodeltest::on_tool_open_msa_triggered(QAction *arg1)
{
    char text[250];
    sprintf(text, "TRIGGER\n");
    ui->consoleMeta->append(to_qstring(text, msg_lvl_info));
    on_act_open_msa_triggered();
}

void xmodeltest::on_tool_open_msa_released()
{
    on_act_open_msa_triggered();
}

void xmodeltest::on_tool_settings_toggled(bool checked)
{
    update_gui();
}




/** SETTINGS **/

void xmodeltest::autoSelectSchemes(const int schemes[], int n)
{

    for (int i=0; i < ui->modelsListView->count(); i++)
        ui->modelsListView->item(i)->setCheckState(Qt::CheckState::Unchecked);

    for (int i=0; i<n; i++)
        ui->modelsListView->item(schemes[i])->setCheckState(Qt::CheckState::Checked);

    update_gui();
}

void xmodeltest::on_sliderNCat_sliderMoved(int position)
{
    char txt[30];
    sprintf(txt, "%d categories", position);
    ui->lblNCat->setText(QString(txt));

//    ui->lblEstimatedMem->setText(
//                QString::number(
//                    compute_size(position,
//                                 ui->sliderNThreads->value())));
}

void xmodeltest::on_radSchemes3_clicked()
{
    if (ui->radDatatypeDna->isChecked())
    {
        int schemes[3] = {0, 1, 10};
        autoSelectSchemes(schemes, 3);
        ui->radSchemes3->setChecked(true);
        // updateGUI();
    }
}

void xmodeltest::on_radSchemes5_clicked()
{
    if (ui->radDatatypeDna->isChecked())
    {
        int schemes[5] = {0, 1, 2, 3, 10};
        autoSelectSchemes(schemes, 5);
        ui->radSchemes5->setChecked(true);
        // updateGUI();
    }
}

void xmodeltest::on_radSchemes7_clicked()
{
    if (ui->radDatatypeDna->isChecked())
    {
        int schemes[7] = {0, 1, 2, 3, 6, 9, 10};
        autoSelectSchemes(schemes, 7);
        ui->radSchemes7->setChecked(true);
        //updateGUI();
    }
}

void xmodeltest::on_radSchemes11_clicked()
{
    if (ui->radDatatypeDna->isChecked())
    {
        for (int i=0; i < ui->modelsListView->count(); i++)
            ui->modelsListView->item(i)->setCheckState(Qt::CheckState::Checked);
        ui->radSchemes11->setChecked(true);
        // updateGUI();
    }
}

void xmodeltest::on_radSchemes203_clicked()
{
    if (ui->radDatatypeDna->isChecked())
    {
        for (int i=0; i < ui->modelsListView->count(); i++)
            ui->modelsListView->item(i)->setCheckState(Qt::CheckState::Unchecked);
        ui->radSchemes203->setChecked(true);
        update_gui();
    }
}

//void xmodeltest::on_radTopoU_clicked()
//{
//    assert(ui->btnLoadTree->isEnabled());
//    on_btnLoadTree_clicked();

//    if (!utree_filename.compare(""))
//    {
//        ui->radTopoFixedGtr->setChecked(true);
//    }
//    updateGUI();
//}

//void xmodeltest::on_radTopoFixedMp_clicked()
//{
//    utree_filename = "";
//    ui->lblTree->setStyleSheet("color: #007;");
//    ui->lblTree->setText("Fixed Maximum Parsimony");
//    updateGUI();
//}

//void xmodeltest::on_radTopoFixedGtr_clicked()
//{
//    utree_filename = "";
//    ui->lblTree->setStyleSheet("color: #007;");
//    ui->lblTree->setText("Fixed Maximum Likelihood (GTR)");
//    updateGUI();
//}

//void xmodeltest::on_radTopoFixedJc_clicked()
//{
//    utree_filename = "";
//    ui->lblTree->setStyleSheet("color: #007;");
//    ui->lblTree->setText("Fixed Maximum Likelihood (JC)");
//    updateGUI();
//}

//void xmodeltest::on_radTopoML_clicked()
//{
//    utree_filename = "";
//    ui->lblTree->setStyleSheet("color: #007;");
//    ui->lblTree->setText("Maximum Likelihood");
//    updateGUI();
//}

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
}

void xmodeltest::on_radDatatypeProt_clicked()
{

    //ui->grpSubstSchemes->setVisible(false);
    ui->grpSubstSchemes->setEnabled(false);
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
}

void xmodeltest::on_radSchemesUser_clicked()
{
    ui->cbShowMatrices->setChecked(true);
    on_cbShowMatrices_clicked();
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
//    ui->lblEstimatedMem->setText(
//                QString::number(
//                    compute_size(n_cats,
//                                 value)));
}
