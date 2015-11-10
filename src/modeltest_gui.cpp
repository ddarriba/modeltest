#include "utils.h"
#include "global_defs.h"
#include "modeltest_gui.h"
#include "ui_modeltest_gui.h"
#include "progressdialog.h"

#include <unistd.h>

#include <iostream>
#include <QtGui/QFileDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QMessageBox>
#include <QtConcurrentRun>
#include <qtconcurrentmap.h>
#include <QFutureWatcher>
#include <QVector>
#include <pthread.h>

#include <QProgressDialog>

#define TABLE_MODELS_WIDTH 905
#define TABLE_COL_DT 6

namespace modeltest {


static char amino_acids[20] = {'A', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'K', 'L',
                               'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'V', 'W', 'Y'};

jModelTest::jModelTest(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::jModelTest)
{
    models_table_items = 0;
    set_state(STATE_INITIAL);

    ui->setupUi(this);

    QPixmap gearPix("IMG/gear.png");
    ui->lblGear->setPixmap(gearPix.scaled(ui->lblGear->width(),ui->lblGear->height(),Qt::KeepAspectRatio));
    ui->sliderNThreads->setRange(1, QThread::idealThreadCount());
    ui->sliderNThreads->setValue(QThread::idealThreadCount());

    n_seqs = 0;
    seq_len = 0;
    scheme = 0;
    tabs_multi_shown = false;

    updateGUI();
}

void jModelTest::clear_table(QTableView * result_table)
{
    if (result_table->model())
    {
        delete result_table->model();
        result_table->setModel(NULL);
    }
}

void jModelTest::fill_results(QTableView * result_table, ModelSelection &model_selection)
{
    QStandardItemModel * results_table_items;
    double cum_weight = 0.0;

    if (result_table->model() != NULL)
    {
        results_table_items = static_cast<QStandardItemModel *>(result_table->model());
        results_table_items->clear();
    }
    else
    {
        results_table_items = new QStandardItemModel(0,7, this);
    }

    int cur_column = 0;
    results_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("Model")));
    results_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("K")));

    results_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("lnL")));
    results_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("score")));
    results_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("delta")));
    results_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("weight")));
    results_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("cum weight")));
    result_table->setModel(results_table_items);

    for (size_t i=0; i<model_selection.size(); i++)
    {
        Model * model = model_selection.get_model(i).model;
        results_table_items->setItem(i, 0, new QStandardItem(QString(model->get_name().c_str())));
        results_table_items->setItem(i, 1, new QStandardItem(QString::number(model->get_n_free_variables())));

        results_table_items->setItem(i, 2, new QStandardItem(QString::number(model->get_lnl(), 'f', 2)));
        results_table_items->setItem(i, 3, new QStandardItem(QString::number(model_selection.get_model(i).score, 'f', 2)));
        results_table_items->setItem(i, 4, new QStandardItem(QString::number(model_selection.get_model(i).delta, 'f', 2)));
        results_table_items->setItem(i, 5, new QStandardItem(QString::number(model_selection.get_model(i).weight, 'f', 2)));
        cum_weight += model_selection.get_model(i).weight;
        results_table_items->setItem(i, 6, new QStandardItem(QString::number(cum_weight, 'f', 2)));
    }
}

size_t jModelTest::compute_size(int n_cats, int n_threads)
{
    size_t mem_b = Utils::mem_size(n_seqs,
                    seq_len,
                    n_cats,
                    N_DNA_STATES);
    mem_b *= n_threads;

    /* overestimating factor */
    mem_b *= 1.2;

    return (mem_b / (1024*1024));
}

void jModelTest::updateGUI()
{
    char txt[30];
    int n_model_sets, n_matrices, n_models;

    ui->lblLoadAlignText->setVisible( check_state(STATE_ALIGNMENT_LOADED) );
    ui->lblLoadTreeText->setVisible( check_state(STATE_TREE_LOADED) && utree_filename.compare(""));
    ui->lblTree->setVisible(check_state(STATE_ALIGNMENT_LOADED));
    ui->lblLoadPartsText->setVisible(check_state(STATE_PARTITIONS_LOADED) && partitions_filename.compare(""));
    ui->lblParts->setVisible(check_state(STATE_ALIGNMENT_LOADED));

    tabs_multi_shown = check_state(STATE_PARTITIONS_LOADED);

    if (ui->radTopoFixedMp->isChecked())
        start_tree = tree_mp;
    else if (ui->radTopoFixedJc->isChecked())
        start_tree = tree_ml_jc_fixed;
    else if (ui->radTopoFixedGtr->isChecked())
        start_tree = tree_ml_gtr_fixed;
    else if (ui->radTopoML->isChecked())
        start_tree = tree_ml;
    else if (ui->radTopoU->isChecked())
        start_tree = tree_user_fixed;

    if (check_state(STATE_ALIGNMENT_LOADED))
    {
        if (start_tree == tree_user_fixed)
            ui->lblTree->setStyleSheet("");
        else
        {
            ui->lblTree->setStyleSheet("color: #007;");
            switch(start_tree)
            {
            case tree_mp:
                ui->lblTree->setText("Fixed Maximum Parsimony");
                break;
            case tree_ml_jc_fixed:
                ui->lblTree->setText("Fixed Maximum Likelihood (JC)");
                break;
            case tree_ml_gtr_fixed:
                ui->lblTree->setText("Fixed Maximum Likelihood (GTR)");
                break;
            case tree_ml:
                ui->lblTree->setText("Maximum Likelihood");
                break;
            default:
                assert(0);
            }
        }
    }

    ui->btnLoadTree->setEnabled(check_state(STATE_ALIGNMENT_LOADED));
    if (ui->btnLoadTree->isEnabled())
        ui->btnLoadTree->setStyleSheet("");
    else
        ui->btnLoadTree->setStyleSheet("color: #999;");
    ui->btnLoadParts->setEnabled(check_state(STATE_ALIGNMENT_LOADED));
    if (ui->btnLoadParts->isEnabled())
        ui->btnLoadParts->setStyleSheet("");
    else
        ui->btnLoadParts->setStyleSheet("color: #999;");

    ui->listMatrices->setEnabled(
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
    ui->btnRun->setEnabled(ui->tabView->isEnabled());

    ui->grpAdvanced->setVisible(ui->cbAdvanced->isChecked());

    bool tabConfigEnabled = check_state(STATE_ALIGNMENT_LOADED);
    bool tabRunEnabled = check_state(STATE_MODELS_OPTIMIZING);
    bool tabResultsEnabled = check_state(STATE_MODELS_OPTIMIZED);
    ui->actionConfigure->setEnabled(tabConfigEnabled);
    ui->tabView->setTabEnabled(TAB_CONFIG, tabConfigEnabled);
    ui->grpOptions->setEnabled(check_state(STATE_ALIGNMENT_LOADED));
    ui->actionProgress->setEnabled(tabRunEnabled);
    ui->tabView->setTabEnabled(TAB_RUN, tabRunEnabled);
    ui->actionResults->setEnabled(tabResultsEnabled);
    ui->tabView->setTabEnabled(TAB_RESULTS, tabResultsEnabled);

    ui->lblNThreads->setText(QString::number(ui->sliderNThreads->value()));

    n_model_sets = ui->cbNoRateVarModels->isChecked() +
            ui->cbIModels->isChecked() +
            ui->cbGModels->isChecked() +
            ui->cbIGModels->isChecked();

    if (ui->radDatatypeDna->isChecked() && ui->radSchemes203->isChecked())
        n_matrices = N_DNA_ALLMATRIX_COUNT;
    else
        n_matrices = ui->listMatrices->selectedItems().size();

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

    set_tablemodels_visibility();

    int n_cats = ui->sliderNCat->value();
    if (!(ui->cbGModels->isChecked() || ui->cbIGModels->isChecked()))
        n_cats = 1;
    ui->lblEstimatedMem->setText(
                QString::number(
                    compute_size(n_cats,
                                 ui->sliderNThreads->value())));
}

void jModelTest::resetSettings()
{
    ui->radSetModelTest->setChecked(true);
    ui->radSchemes11->setChecked(true);
    ui->listMatrices->selectAll();
    ui->cbEqualFreq->setChecked(true);
    ui->cbMlFreq->setChecked(true);
    ui->cbNoRateVarModels->setChecked(true);
    ui->cbIModels->setChecked(true);
    ui->cbGModels->setChecked(true);
    ui->cbIGModels->setChecked(true);
    ui->sliderNCat->setValue(4);
    ui->radTopoFixedGtr->setChecked(true);

    ui->cbAdvanced->setChecked(false);

    clear_state();
    if (msa_filename.compare(""))
        set_state(STATE_ALIGNMENT_LOADED);
        if (utree_filename.compare(""))
        {
            set_state(STATE_TREE_LOADED);
        }
    else
    {
        set_state(STATE_INITIAL);
    }

    on_sliderNCat_sliderMoved(4);

    /* clear models */
    for (size_t i=0; i<c_models.size(); i++)
        delete(c_models[i]);
    c_models.clear();

    /* reset tables */
//    models_table_items->clear();
    clear_table(ui->tblResultsBic);
    clear_table(ui->tblResultsAic);
    clear_table(ui->tblResultsAicc);
    clear_table(ui->tblResultsDt);

    updateGUI();
}

jModelTest::~jModelTest()
{
    delete models_table_items;
    delete ui;
}

static QString to_qstring(const char * msg, msg_level level)
{
    QString line = msg;
    QString endHtml = "</font>";

    switch(level)
    {
        case msg_error: line = "<font color=\"Red\">" + line + "</font>"; break;
        case msg_alert: line = "<font color=\"DeepPink\">" + line + "</font>"; break;
        case msg_notify: line = "<font color=\"DarkGreen\">" + line + "</font>"; break;
        case msg_info: line  = "<font color=\"Black\">" + line + "</font>"; break;
        default: break;
    }

    line = line + endHtml;
    return line;
}

void jModelTest::on_btnLoadAlignment_clicked()
{
    QString filters = tr("Multiple Sequence Alignment(*.phy *.nex *.fas);; All files(*)");
    QString file_name = QFileDialog::getOpenFileName(this,
                                                    tr("Open File"),
                                                    "",
                                                    filters);

    const std::string loaded_file = file_name.toStdString();

    if ( loaded_file.compare(""))
    {
        msa_filename = loaded_file;
        if (ModelTest::test_msa(msa_filename,
                                 &n_seqs,
                                 &seq_len))
        {
            char text[250];

            ui->txt_messages->append(to_qstring("Loaded alignment %1", msg_notify).arg(msa_filename.c_str()));
            sprintf(text, "Num.Sequences:   %d", n_seqs);
            ui->txt_messages->append(to_qstring(text, msg_info));
            sprintf(text, "Sequence Length: %d", seq_len);
            ui->txt_messages->append(to_qstring(text, msg_info));

            utree_filename = "";
            clear_state();
            set_state(STATE_ALIGNMENT_LOADED);
            ui->tabView->setCurrentIndex(TAB_CONFIG);
            on_radSchemes11_clicked();
        }
        else
        {
            ui->txt_messages->append(to_qstring("Error: Cannot load alignment %1", msg_error).arg(msa_filename.c_str()));
            ui->tabView->setCurrentIndex(TAB_CONSOLE);
            msa_filename   = "";
            utree_filename = "";
            set_state(STATE_INITIAL);
        }
    }

    ui->lblAlignment->setText(QString(Utils::getBaseName(msa_filename).c_str()));
    updateGUI();
}

void jModelTest::on_btnLoadTree_clicked()
{
    QString filters = tr("Newick tree(*.tree *.newick);; All files(*)");
    QString file_name = QFileDialog::getOpenFileName(this,
                                                    tr("Open File"),
                                                    "",
                                                    filters);
    const std::string loaded_file = file_name.toStdString();

    if ( loaded_file .compare(""))
    {
        mt_size_t n_tips;

        utree_filename = loaded_file;
        if (ModelTest::test_tree(utree_filename,
                                 &n_tips))
        {
            if (n_tips == n_seqs)
            {
                ui->txt_messages->append(to_qstring("Loaded tree %1", msg_notify).arg(utree_filename.c_str()));
                ui->tabView->setCurrentIndex(TAB_CONFIG);
                if (check_state(STATE_ALIGNMENT_LOADED))
                    set_state(STATE_TREE_LOADED);
            }
            else
            {
                ui->txt_messages->append(to_qstring("Error: Tip number does not match in %1", msg_error).arg(utree_filename.c_str()));
                ui->tabView->setCurrentIndex(TAB_CONSOLE);
                utree_filename = "";
                unset_state(STATE_TREE_LOADED);
            }
        }
        else
        {
            ui->txt_messages->append(to_qstring("Error: Cannot read tree %1", msg_error).arg(utree_filename.c_str()));
            ui->tabView->setCurrentIndex(TAB_CONSOLE);
            utree_filename = "";
            unset_state(STATE_TREE_LOADED);
        }

        if (ui->radTopoU->isChecked())
            ui->radTopoFixedGtr->setChecked(!check_state(STATE_TREE_LOADED));
        else
            ui->radTopoU->setChecked(check_state(STATE_TREE_LOADED));
    }

    ui->lblTree->setText(QString(Utils::getBaseName(utree_filename).c_str()));
    updateGUI();
}

void jModelTest::on_btnLoadParts_clicked()
{
    QString filters = tr("Partitions file(*.model *.parts *.conf);; All files (*)");
    QString file_name = QFileDialog::getOpenFileName(this,
                                                    tr("Open File"),
                                                    "",
                                                    filters);
    const std::string loaded_file = file_name.toStdString();

    if (loaded_file.compare(""))
    {
        partitions_filename = loaded_file;

        /*TODO: validate */
        scheme = Utils::parse_partitions_file(partitions_filename);
        if (!scheme)
        {
            partitions_filename = "";
            ui->txt_messages->append(to_qstring("Error loading partitions %1", msg_error).arg(partitions_filename.c_str()));
            ui->txt_messages->append(to_qstring(mt_errmsg, msg_error));
            ui->tabView->setCurrentIndex(TAB_CONSOLE);
            if (scheme)
            {
                delete scheme;
                scheme = 0;
            }
            unset_state(STATE_PARTITIONS_LOADED);
        }
        else
        {
            if (!ModelTest::test_partitions(*scheme, seq_len))
            {
                ui->txt_messages->append(to_qstring("Error loading partitions %1", msg_error).arg(partitions_filename.c_str()));
                ui->txt_messages->append(to_qstring(mt_errmsg, msg_error));
                ui->tabView->setCurrentIndex(TAB_CONSOLE);
                partitions_filename = "";
                delete scheme;
                scheme = 0;
                unset_state(STATE_PARTITIONS_LOADED);
            }
            else
            {
                ui->txt_messages->append(to_qstring("Loaded %1 partitions %2", msg_notify).arg(scheme->size()).arg(partitions_filename.c_str()));
                if (mt_errno)
                {
                    /* there are warnings */
                    ui->txt_messages->append("Warning: " + to_qstring(mt_errmsg, msg_error));
                }
                ui->tabView->setCurrentIndex(TAB_CONSOLE);
                set_state(STATE_PARTITIONS_LOADED);
            }
        }

        ui->lblParts->setText(QString(Utils::getBaseName(partitions_filename).c_str()));
        if (partitions_filename.compare(""))
            ui->lblLoadPartsText->setText("Loaded");
        else
            ui->lblLoadPartsText->setText("None");
        updateGUI();
    }
}

void jModelTest::autoSelectSchemes(const int schemes[], int n)
{
    ui->listMatrices->clearSelection();

    for (int i=0; i<n; i++)
        ui->listMatrices->item( schemes[i] )->setSelected(true);

    updateGUI();
}

void jModelTest::on_listMatrices_itemSelectionChanged()
{
    ui->radSchemesUser->setChecked(true);
    updateGUI();
}

void jModelTest::on_radSchemes3_clicked()
{
    if (ui->radDatatypeDna->isChecked())
    {
        int schemes[3] = {0, 1, 10};
        autoSelectSchemes(schemes, 3);
        ui->radSchemes3->setChecked(true);
        // updateGUI();
    }
}

void jModelTest::on_radSchemes5_clicked()
{
    if (ui->radDatatypeDna->isChecked())
    {
        int schemes[5] = {0, 1, 2, 3, 10};
        autoSelectSchemes(schemes, 5);
        ui->radSchemes5->setChecked(true);
        // updateGUI();
    }
}

void jModelTest::on_radSchemes7_clicked()
{
    if (ui->radDatatypeDna->isChecked())
    {
        int schemes[7] = {0, 1, 2, 3, 6, 9, 10};
        autoSelectSchemes(schemes, 7);
        ui->radSchemes7->setChecked(true);
        //updateGUI();
    }
}

void jModelTest::on_radSchemes11_clicked()
{
    if (ui->radDatatypeDna->isChecked())
    {
        ui->listMatrices->selectAll();
        ui->radSchemes11->setChecked(true);
        // updateGUI();
    }
}

void jModelTest::on_radSchemes203_clicked()
{
    if (ui->radDatatypeDna->isChecked())
    {
        ui->listMatrices->clearSelection();
        ui->radSchemes203->setChecked(true);
        // updateGUI();
    }
}

void jModelTest::on_radTopoU_clicked()
{
    assert(ui->btnLoadTree->isEnabled());
    on_btnLoadTree_clicked();

    if (!utree_filename.compare(""))
    {
        ui->radTopoFixedGtr->setChecked(true);
    }
    updateGUI();
}

void jModelTest::on_radTopoFixedMp_clicked()
{
    utree_filename = "";
    ui->lblTree->setStyleSheet("color: #007;");
    ui->lblTree->setText("Fixed Maximum Parsimony");
    updateGUI();
}

void jModelTest::on_radTopoFixedGtr_clicked()
{
    utree_filename = "";
    ui->lblTree->setStyleSheet("color: #007;");
    ui->lblTree->setText("Fixed Maximum Likelihood (GTR)");
    updateGUI();
}

void jModelTest::on_radTopoFixedJc_clicked()
{
    utree_filename = "";
    ui->lblTree->setStyleSheet("color: #007;");
    ui->lblTree->setText("Fixed Maximum Likelihood (JC)");
    updateGUI();
}

void jModelTest::on_radTopoML_clicked()
{
    utree_filename = "";
    ui->lblTree->setStyleSheet("color: #007;");
    ui->lblTree->setText("Maximum Likelihood");
    updateGUI();
}

void jModelTest::on_radSetModelTest_clicked()
{
    on_radSetModelTest_toggled(true);
}

void jModelTest::on_radSetMrbayes_clicked()
{
    on_radSetMrbayes_toggled(true);
}

void jModelTest::on_radSetRaxml_clicked()
{
    on_radSetRaxml_toggled(true);
}

void jModelTest::on_radSetPhyml_clicked()
{
    on_radSetPhyml_toggled(true);
}

void jModelTest::on_radSetModelTest_toggled(bool checked)
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
    updateGUI();
}

void jModelTest::on_radSetMrbayes_toggled(bool checked)
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
    updateGUI();
}

void jModelTest::on_radSetRaxml_toggled(bool checked)
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
    updateGUI();
}

void jModelTest::on_radSetPhyml_toggled(bool checked)
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
    updateGUI();
}

void jModelTest::on_sliderNCat_sliderMoved(int position)
{
    char txt[30];
    sprintf(txt, "%d categories", position);
    ui->lblNCat->setText(QString(txt));

    ui->lblEstimatedMem->setText(
                QString::number(
                    compute_size(position,
                                 ui->sliderNThreads->value())));
}

void jModelTest::on_menuFileLoad_triggered()
{
    on_btnLoadAlignment_clicked();
}

void jModelTest::on_menuTreeLoad_triggered()
{
    on_btnLoadTree_clicked();
}

void jModelTest::on_menuFileQuit_triggered()
{
    exit(EXIT_SUCCESS);
}

void jModelTest::on_actionReset_triggered()
{
    QMessageBox msgBox;
    msgBox.setText("Reset ModelTest");
    msgBox.setInformativeText("Are you sure?");
    msgBox.setStandardButtons(QMessageBox::Reset | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    if (ret == QMessageBox::Reset)
    {
        set_state(STATE_INITIAL);
        ui->lblAlignment->setText("");
        ui->lblTree->setText("");
        msa_filename = "";
        resetSettings();
        ui->txt_messages->append(to_qstring("\nReset\n", msg_alert));
    }
}

void jModelTest::on_actionConsole_triggered()
{
    /* if tab is disabled, the menu should be as well */
    assert(ui->tabView->isTabEnabled(TAB_CONSOLE));
    ui->tabView->setCurrentIndex(TAB_CONSOLE);
}

void jModelTest::on_actionConfigure_triggered()
{
    /* if tab is disabled, the menu should be as well */
    assert(ui->tabView->isTabEnabled(TAB_CONFIG));
    ui->tabView->setCurrentIndex(TAB_CONFIG);
}

void jModelTest::on_actionProgress_triggered()
{
    /* if tab is disabled, the menu should be as well */
    assert(ui->tabView->isTabEnabled(TAB_RUN));
    ui->tabView->setCurrentIndex(TAB_RUN);
}

void jModelTest::on_actionResults_triggered()
{
    /* if tab is disabled, the menu should be as well */
    assert(ui->tabView->isTabEnabled(TAB_RESULTS));
    ui->tabView->setCurrentIndex(TAB_RESULTS);
}

void jModelTest::on_cbEqualFreq_toggled(bool checked)
{
    UNUSED(checked);
    updateGUI();
}

void jModelTest::on_cbMlFreq_toggled(bool checked)
{
    UNUSED(checked);
    updateGUI();
}

void jModelTest::on_cbNoRateVarModels_toggled(bool checked)
{
    UNUSED(checked);
    updateGUI();
}

void jModelTest::on_cbIModels_toggled(bool checked)
{
    UNUSED(checked);
    updateGUI();
}

void jModelTest::on_cbGModels_toggled(bool checked)
{
    UNUSED(checked);
    updateGUI();
}

void jModelTest::on_cbIGModels_toggled(bool checked)
{
    UNUSED(checked);
    updateGUI();
}

void jModelTest::on_btnResetConfig_clicked()
{
    QMessageBox msgBox;
    if (c_models.size())
        msgBox.setText("If you reset the settings, results will be discarded.");
    else
        msgBox.setText("Current settings will be discarded.");
    msgBox.setInformativeText("Are you sure?");
    msgBox.setStandardButtons(QMessageBox::Reset | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    if (ret == QMessageBox::Reset)
        resetSettings();
}

void jModelTest::evaluate_models(ModelTest &mtest)
{
    //TODO: Get actual partition
    partition_id_t part_id = {0};
    const std::vector<Model *> models = mtest.get_models(part_id);
    for (size_t i=0; i<models.size(); i++)
    {
        Model * model = models[i];
        mtest.evaluate_single_model(model, part_id);
            updateGUI();
    }
}

void jModelTest::evalmodels(int i, int thread_id)
{
    //TODO: Get actual partition
    partition_id_t part_id = {0};

    const std::vector<Model *> modelsPtr = mtest->get_models(part_id);
    Model * model = modelsPtr[i];
    double tolerance = ui->txtParEpsilon->text().toDouble();
    double epsilon = ui->txtOptEpsilon->text().toDouble();

    if (!model->is_optimized())
        mtest->evaluate_single_model(model, part_id,
                                     thread_id,
                                     tolerance, epsilon);

    set_tablemodels_line(i, model);

    emit finishedModel(modelsPtr[i]);
}

static pthread_mutex_t mutex;

static bool test_and_set(bool *lock)
{
    bool retval = true;
    pthread_mutex_lock(&mutex);
    if (*lock)
    {
        retval = false;
    }
    else
        *lock = true;
    pthread_mutex_unlock(&mutex);
    return retval;
}

static void unlock(bool *lock)
{
    *lock = false;
}

struct ModOptWrapper {

  jModelTest *instance;
  int number_of_threads;
  std::map<Qt::HANDLE, int> thread_map;
  int cur_model;
  int cur_thread;
  bool lock;

  ModOptWrapper(jModelTest *w, int number_of_threads):
      instance(w), number_of_threads(number_of_threads)
  {
      cur_model = 0;
      cur_thread = 0;
      lock = false;
      assert(!pthread_mutex_init(&mutex, NULL));
  }
  ~ModOptWrapper()
  {
      pthread_mutex_destroy(&mutex);
  }

  void operator()(int &i) {
      UNUSED(i);
      usleep(1000*(rand()%100));
      while (!test_and_set(&lock))
          usleep(1000*(rand()%100));

      if (thread_map.find(QThread::currentThreadId()) == thread_map.end())
      {
          thread_map[QThread::currentThreadId()] = cur_thread++;
      }
      int thread_id = thread_map[QThread::currentThreadId()];

      assert(thread_id < number_of_threads);

      unlock(&lock);
      instance->evalmodels(cur_model++, thread_id);
  }
};

void jModelTest::cancel_jobs()
{
    on_run = false;
}

void jModelTest::on_btnRun_clicked()
{
    //TODO: Get actual partition
    partition_id_t part_id = {0};
    int number_of_threads  = ui->sliderNThreads->value();

    QMessageBox msgBox;
    msgBox.setText("Start models optimizaion");
    msgBox.setInformativeText("Are you sure?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    if (ret != QMessageBox::Ok)
        return;

    if (ui->radTopoFixedMp->isChecked())
        start_tree = tree_mp;
    else if (ui->radTopoFixedJc->isChecked())
        start_tree = tree_ml_jc_fixed;
    else if (ui->radTopoFixedGtr->isChecked())
        start_tree = tree_ml_gtr_fixed;
    else if (ui->radTopoML->isChecked())
        start_tree = tree_ml;
    else if (ui->radTopoU->isChecked())
        start_tree = tree_user_fixed;

    set_tablemodels_header();

    mtest = new ModelTest(number_of_threads);

    int model_params = 0;
    if (ui->cbEqualFreq->isChecked())
        model_params += MOD_PARAM_FIXED_FREQ;
    if (ui->cbNoRateVarModels->isChecked())
        model_params += MOD_PARAM_NO_RATE_VAR;
    if (ui->cbIModels->isChecked())
        model_params += MOD_PARAM_INV;
    if (ui->cbGModels->isChecked())
        model_params += MOD_PARAM_GAMMA;
    if (ui->cbIGModels->isChecked())
        model_params += MOD_PARAM_INV_GAMMA;
    if (ui->cbMlFreq->isChecked())
        model_params += MOD_PARAM_ESTIMATED_FREQ;

    ui->cbShowRates->setEnabled(ui->radDatatypeDna->isChecked());
    ui->cbShowRates->setVisible(ui->radDatatypeDna->isChecked());

    std::vector<mt_index_t> matrices;
    if (ui->radDatatypeProt->isChecked())
    {
        for (int i=0; i < ui->listMatrices->count(); i++)
        {
            if (ui->listMatrices->item(i)->isSelected())
                matrices.push_back(i);
        }
    }
    else
    {
        if (!ui->radSchemes203->isChecked())
        {
            for (int i=0; i < ui->listMatrices->count(); i++)
            {
                if (ui->listMatrices->item(i)->isSelected())
                    matrices.push_back(dna_model_matrices_indices[i]);
            }
        }
        else
        {
            for (int i=0; i<N_DNA_ALLMATRIX_COUNT; i++)
            {
                matrices.push_back(i);
            }
        }
    }

    opts.model_params = model_params;
    opts.n_catg = ui->sliderNCat->value();
    opts.msa_filename = msa_filename;
    opts.tree_filename = utree_filename;
    opts.partitions_filename = "";
    if (ui->radDatatypeDna->isChecked())
        opts.nt_candidate_models = matrices;
    else
        opts.aa_candidate_models = matrices;
    opts.starting_tree = start_tree;
    opts.partitions_desc = NULL;
    opts.partitions_eff = NULL;
    data_type datatype = ui->radDatatypeDna->isChecked()?dt_dna:dt_protein;
    if (!scheme)
    {
        /* create single partition / single region */
        scheme = new partitioning_scheme_t();
        partition_region_t region;
        partition_t partition;
        region.start = 1;
        region.end = seq_len;
        region.stride = 1;
        partition.datatype = datatype;
        partition.partition_name = "DATA";
        partition.regions.push_back(region);
        scheme->push_back(partition);
    }
    opts.partitions_desc = scheme;

    bool ok_inst = mtest->build_instance(opts);
    if (!ok_inst)
        ui->txt_messages->append(to_qstring("Error building instance", msg_error));

    if (c_models.size())
    {
        mtest->set_models(c_models, part_id);
    }

    unset_state(STATE_MODELS_OPTIMIZED);
    set_state(STATE_MODELS_OPTIMIZING);
    ui->tabView->setCurrentIndex(TAB_RUN);

    /* hide DT tabs/columns if the tree is fixed */
    ui->results_content->setTabEnabled(TAB_RESULTS_DT,
                                       opts.starting_tree == tree_ml);
    ui->tblModels->setColumnHidden(TABLE_COL_DT,
                                   (opts.starting_tree != tree_ml));
    updateGUI();

    QThreadPool::globalInstance()->setMaxThreadCount(number_of_threads);

    /* print settings */
    {
    ui->txt_messages->append("");
    ui->txt_messages->append("Execution options:");
    ui->txt_messages->append(to_qstring("Alignment %1", msg_info).arg(msa_filename.c_str()));
    ui->txt_messages->append(to_qstring("Tree      %1", msg_info).arg(utree_filename.c_str()));
    if (ui->radDatatypeDna->isChecked())
        ui->txt_messages->append("DataType  DNA");
    else
        ui->txt_messages->append("DataType  Protein");
    ui->txt_messages->append(to_qstring("N.Models  %1", msg_info).arg(ui->lblNumModels->text()));
    }

    //QFuture<void> fut = QtConcurrent::run(evalmodels, mtest, ui);
    const std::vector<Model *> & modelsPtr = mtest->get_models(part_id);
    QVector<int> models;
    for (int i=0; (size_t)i < modelsPtr.size(); i++)
        models.append(i);

    QFutureWatcher<void> futureWatcher;
    ModOptWrapper wrap(this, number_of_threads);

    //futureWatcher.

    ProgressDialog dialog( modelsPtr.size() );
    dialog.setLabelText(QString("Progressing using %1 thread(s)...").arg(number_of_threads));

    QObject::connect(&futureWatcher, SIGNAL(finished()), &dialog, SLOT(reset()));
    QObject::connect(&dialog, SIGNAL(canceled()), &futureWatcher, SLOT(cancel()));
    QObject::connect(&dialog, SIGNAL(canceled()), this, SLOT(cancel_jobs()));
    //QObject::connect(&futureWatcher, SIGNAL(progressRangeChanged(int,int)), &dialog, SLOT(setRange(int,int)));
    QObject::connect(&futureWatcher, SIGNAL(progressValueChanged(int)), &dialog, SLOT(setValue(int)));
    QObject::connect(this, SIGNAL(finishedModel(Model *)), &dialog, SLOT(addModel(Model *)));

    on_run = true;
    futureWatcher.setFuture(QtConcurrent::map(models, wrap));//&jModelTest::evalmodels);
    set_state(STATE_MODELS_OPTIMIZED);

    dialog.exec();
    futureWatcher.waitForFinished();
    on_run = false;

    ModelSelection bic_selection(modelsPtr, ic_bic);
    fill_results(ui->tblResultsBic, bic_selection);
    ModelSelection aic_selection(modelsPtr, ic_aic);
    fill_results(ui->tblResultsAic, aic_selection);
    ModelSelection aicc_selection(modelsPtr, ic_aicc);
    fill_results(ui->tblResultsAicc, aicc_selection);
    if (opts.starting_tree == tree_ml)
    {
        ui->tabResultsDt->setVisible(true);
        ModelSelection dt_selection(modelsPtr, ic_dt);
        fill_results(ui->tblResultsDt, dt_selection);
    }
    else
    {
        ui->tabResultsDt->setVisible(false);
    }

    /* clear and clone models */
    for (size_t i=0; i<c_models.size(); i++)
        delete(c_models[i]);
    c_models.clear();
    c_models.resize( modelsPtr.size() );
    for (size_t i=0; i<modelsPtr.size(); i++)
        if (ui->radDatatypeDna->isChecked())
            c_models[i] = new DnaModel(*(modelsPtr[i]));
        else
            c_models[i] = new ProtModel(*(modelsPtr[i]));

    delete mtest;

    updateGUI();
}

void jModelTest::on_cbAdvanced_clicked()
{
    updateGUI();
}

#define TABLE_WIDTH (ui->radDatatypeDna->isChecked()?19:30)
#define TABLE_INI_SELECTION 3
#define TABLE_INI_HET_PARAMS 7
#define TABLE_INI_FREQS 9
#define N_FREQS (ui->radDatatypeDna->isChecked()?4:20)
#define TABLE_INI_RATES 13 /* DNA only */

void jModelTest::set_tablemodels_visibility()
{
    if (models_table_items)
    {
        ui->tblModels->setColumnHidden(7, !ui->cbShowHetParams->isChecked());
        ui->tblModels->setColumnHidden(8, !ui->cbShowHetParams->isChecked());
        for (int cur_column=TABLE_INI_FREQS;cur_column<TABLE_INI_FREQS+N_FREQS; cur_column++)
            ui->tblModels->setColumnHidden(cur_column, !ui->cbShowFreqs->isChecked());
        if (ui->radDatatypeDna->isChecked())
        {
            for (int cur_column=TABLE_INI_RATES;cur_column<TABLE_INI_RATES+5; cur_column++)
             ui->tblModels->setColumnHidden(cur_column, !ui->cbShowRates->isChecked());
        }
        ui->tblModels->resizeColumnsToContents();
    }
}

void jModelTest::set_tablemodels_line(int line_id, Model * model)
{
    mt_index_t cur_column = 0;
    models_table_items->setItem(line_id, cur_column++, new QStandardItem(QString(model->get_name().c_str())));
    models_table_items->setItem(line_id, cur_column++, new QStandardItem(QString::number(model->get_n_free_variables())));

    models_table_items->setItem(line_id, cur_column++, new QStandardItem(QString::number(model->get_lnl(), 'f', 2)));
    models_table_items->setItem(line_id, cur_column++, new QStandardItem(QString::number(model->get_aic(), 'f', 2)));
    models_table_items->setItem(line_id, cur_column++, new QStandardItem(QString::number(model->get_aicc(), 'f', 2)));
    models_table_items->setItem(line_id, cur_column++, new QStandardItem(QString::number(model->get_bic(), 'f', 2)));
    models_table_items->setItem(line_id, cur_column++, new QStandardItem(QString::number(model->get_dt(), 'f', 2)));

    if (model->is_I())
      models_table_items->setItem(line_id, cur_column++, new QStandardItem(QString::number(model->get_prop_inv(), 'f', 2)));
    else
      models_table_items->setItem(line_id, cur_column++, new QStandardItem(QString("-")));

    if (model->is_G())
      models_table_items->setItem(line_id, cur_column++, new QStandardItem(QString::number(model->get_alpha(), 'f', 2)));
    else
      models_table_items->setItem(line_id, cur_column++, new QStandardItem(QString("-")));

    assert(model->get_n_states() == N_FREQS);
    const double * frequencies = model->get_frequencies();
    for (int i=0; i<N_FREQS; i++)
    {
        models_table_items->setItem(line_id, cur_column++, new QStandardItem(QString::number(frequencies[i], 'f', 4)));
    }

    if (ui->radDatatypeDna->isChecked())
    {
    const double * rates = model->get_subst_rates();
        models_table_items->setItem(line_id, cur_column++, new QStandardItem(QString::number(rates[0], 'f', 2)));
        models_table_items->setItem(line_id, cur_column++, new QStandardItem(QString::number(rates[1], 'f', 2)));
        models_table_items->setItem(line_id, cur_column++, new QStandardItem(QString::number(rates[2], 'f', 2)));
        models_table_items->setItem(line_id, cur_column++, new QStandardItem(QString::number(rates[3], 'f', 2)));
        models_table_items->setItem(line_id, cur_column++, new QStandardItem(QString::number(rates[4], 'f', 2)));
    }

    models_table_items->setItem(line_id, cur_column++, new QStandardItem(QString::number(model->get_exec_time())));

    assert(cur_column == TABLE_WIDTH);
}

void jModelTest::set_tablemodels_header()
{
    int cur_column = 0;

    if (models_table_items)
        delete models_table_items;

    models_table_items = new QStandardItemModel(0,TABLE_WIDTH, this);
    models_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("Model")));
    models_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("K")));

    models_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("lnL")));
    models_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("AIC")));
    models_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("AICc")));
    models_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("BIC")));
    models_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("DT")));

    models_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("p-inv")));
    models_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("alpha")));

    if (ui->radDatatypeDna->isChecked())
    {
        models_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("f(a)")));
        models_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("f(c)")));
        models_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("f(g)")));
        models_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("f(t)")));

        models_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("r(a->c)")));
        models_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("r(a->g)")));
        models_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("r(a->t)")));
        models_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("r(c->g)")));
        models_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("r(c->t)")));
    }
    else
    {
        for (mt_index_t i=0; i<N_PROT_STATES; i++)
        {
            models_table_items->setHorizontalHeaderItem(cur_column, new QStandardItem(QString("f") + QString(amino_acids[i])));
            cur_column++;
        }
    }

    models_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("Time")));
    ui->tblModels->setModel(models_table_items);

    assert(cur_column == TABLE_WIDTH);
}

void jModelTest::updateModelsTableSize()
{
    QRect rect = ui->tblModels->geometry();
    int totalWidth = 0;
    for (int i=0; i<TABLE_WIDTH; i++)
    {
        if (!ui->tblModels->isColumnHidden(i))
            totalWidth += ui->tblModels->columnWidth(i);
    }

    if (totalWidth > TABLE_MODELS_WIDTH)
        totalWidth = TABLE_MODELS_WIDTH;

    rect.setWidth(45 + totalWidth);
    ui->tblModels->setGeometry(rect);
}

void jModelTest::on_cbShowFreqs_toggled(bool checked)
{
    for (int cur_column=TABLE_INI_FREQS;cur_column<TABLE_INI_FREQS+N_FREQS; cur_column++)
        ui->tblModels->setColumnHidden(cur_column, !checked);
    updateModelsTableSize();
}

void jModelTest::on_cbShowRates_toggled(bool checked)
{
    if (ui->radDatatypeDna->isChecked())
    {
        for (int cur_column=TABLE_INI_RATES;cur_column<TABLE_INI_RATES+5; cur_column++)
            ui->tblModels->setColumnHidden(cur_column, !checked);
        updateModelsTableSize();
    }
}

void jModelTest::on_cbShowHetParams_toggled(bool checked)
{
    ui->tblModels->setColumnHidden(TABLE_INI_HET_PARAMS, !checked);
    ui->tblModels->setColumnHidden(TABLE_INI_HET_PARAMS+1, !checked);
    updateModelsTableSize();
}

void jModelTest::on_cbShowSelection_toggled(bool checked)
{
    for (int cur_column=TABLE_INI_SELECTION;cur_column<(TABLE_INI_SELECTION+4); cur_column++)
        ui->tblModels->setColumnHidden(cur_column, !checked);
    updateModelsTableSize();
}

void jModelTest::on_sliderNThreads_valueChanged(int value)
{
    ui->lblNThreads->setText(QString::number(value));
    ui->lblEstimatedMem->setText(
                QString::number(
                    compute_size(ui->sliderNCat->value(),
                                 value)));
}

void jModelTest::on_radDatatypeDna_clicked()
{
    ui->grpSubstSchemes->setVisible(true);
    ui->grpSubstSchemes->setEnabled(true);
    ui->cbMlFreq->setText("ML frequencies");
    ui->listMatrices->clear();
    ui->listMatrices->addItem("000000  JC / F81");
    ui->listMatrices->addItem("010010  K80 / HKY85");
    ui->listMatrices->addItem("010020  TrNef / TrN");
    ui->listMatrices->addItem("012210  TPM1 / TPM1uf");
    ui->listMatrices->addItem("010212  TPM2 / TPM2uf");
    ui->listMatrices->addItem("012012  TPM3 / TPM3uf");
    ui->listMatrices->addItem("012230  TIM1ef / TIM1");
    ui->listMatrices->addItem("010232  TIM2ef / TIM2");
    ui->listMatrices->addItem("012032  TIM3ef / TIM3");
    ui->listMatrices->addItem("012314  TVMef / TVM");
    ui->listMatrices->addItem("012345  SYM / GTR");
    ui->listMatrices->setMinimumHeight(211);
    ui->listMatrices->setMaximumHeight(211);
    on_radSchemes11_clicked();
}

void jModelTest::on_radDatatypeProt_clicked()
{
    ui->grpSubstSchemes->setVisible(false);
    ui->grpSubstSchemes->setEnabled(false);
    ui->cbMlFreq->setText("Empirical frequencies");
    ui->listMatrices->clear();
    for (mt_index_t i=0; i<N_PROT_MODEL_MATRICES; i++)
        ui->listMatrices->addItem(prot_model_names[i].c_str());
    ui->listMatrices->setMinimumHeight(363);
    ui->listMatrices->setMaximumHeight(363);
    ui->listMatrices->selectAll();
}

bool jModelTest::check_state(current_state st)
{
    return (state & st);
}

void jModelTest::set_state(current_state st)
{
    if (st == STATE_INITIAL)
        state = STATE_INITIAL;
    else
        state = (current_state)(state | st);
}

void jModelTest::unset_state(current_state st)
{
    state = (current_state)(state & ~st);
}

}
