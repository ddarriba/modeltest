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

namespace modeltest {

static char amino_acids[20] = {'A', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'K', 'L',
                               'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'V', 'W', 'Y'};

jModelTest::jModelTest(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::jModelTest)
{
    models_table_items = 0;
    state = STATE_INITIAL;

    ui->setupUi(this);

    QPixmap gearPix("IMG/gear.png");
    ui->lblGear->setPixmap(gearPix.scaled(ui->lblGear->width(),ui->lblGear->height(),Qt::KeepAspectRatio));
    ui->sliderNThreads->setRange(1, QThread::idealThreadCount());
    ui->sliderNThreads->setValue(QThread::idealThreadCount());

    n_seqs = 0;
    seq_len = 0;

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

    ui->lblLoadAlignText->setVisible( state >= STATE_ALIGNMENT_LOADED );
    ui->lblLoadTreeText->setVisible( state >= STATE_TREE_LOADED );

    if (ui->radTopo1->isChecked())
        start_tree = tree_mp;
    else if (ui->radTopo2->isChecked())
        start_tree = tree_ml_jc_fixed;
    else if (ui->radTopo3->isChecked())
        start_tree = tree_ml_gtr_fixed;
    else if (ui->radTopo4->isChecked())
        start_tree = tree_ml;
    else if (ui->radTopoU->isChecked())
        start_tree = tree_user_fixed;

    ui->lblTree->setVisible(state >= STATE_TREE_LOADED);
    if (state >= STATE_TREE_LOADED)
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

    ui->btnLoadTree->setEnabled( state >= STATE_ALIGNMENT_LOADED );

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

    ui->tabView->setTabEnabled(TAB_CONFIG, state >= STATE_ALIGNMENT_LOADED );
    ui->grpOptions->setEnabled( state == STATE_ALIGNMENT_LOADED || state == STATE_TREE_LOADED );
    ui->tabView->setTabEnabled(TAB_RUN, state >= STATE_MODELS_OPTIMIZING);
    ui->tabView->setTabEnabled(TAB_RESULTS, state >= STATE_MODELS_OPTIMIZED);

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
    ui->radTopo4->setChecked(true);

    ui->cbAdvanced->setChecked(false);

    if (msa_filename.compare(""))
        state = STATE_ALIGNMENT_LOADED;
        if (utree_filename.compare(""))
        {
            state = STATE_TREE_LOADED;
        }
    else
    {
        state = STATE_INITIAL;
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
        case msg_error: line = "<font color=\"Red\">ERROR:</font> " + line; break;
        case msg_alert: line = "<font color=\"DeepPing\">ALERT:</font> " + line; break;
        case msg_notify: line = "<font color=\"Lime\">NOTIFY:</font> " + line; break;
        case msg_info: line  = "<font color=\"DarkGreen\"> " + line; break;
        default: break;
    }

    line = line + endHtml;
    return line;
}

void jModelTest::on_btnLoadAlignment_clicked()
{
    QString filters = "MSA (*.phy *.nex *.fas *)";
    QString file_name = QFileDialog::getOpenFileName(this,
                                                    tr("Open File"),
                                                    "",
                                                    filters);

    const std::string loaded_file = file_name.toStdString();

    if ( loaded_file.compare(""))
    {
        msa_filename = loaded_file;
        //ModelTest mtest;
        if (ModelTest::test_msa(msa_filename,
                                 &n_seqs,
                                 &seq_len))
        {
            char text[250];

            ui->txtMessages->append(to_qstring("Loaded alignment %1", msg_info).arg(msa_filename.c_str()));
            sprintf(text, "Num.Sequences:   %d", n_seqs);
            ui->txtMessages->append(to_qstring(text, msg_info));
            sprintf(text, "Sequence Length: %d", seq_len);
            ui->txtMessages->append(to_qstring(text, msg_info));

            utree_filename = "";
            state = STATE_ALIGNMENT_LOADED;
            ui->tabView->setCurrentIndex(TAB_CONFIG);
            on_radSchemes11_clicked();
        }
        else
        {
            ui->txtMessages->append(to_qstring("Cannot load alignment", msg_error));
            ui->tabView->setCurrentIndex(TAB_CONSOLE);
            msa_filename   = "";
            utree_filename = "";
            state = STATE_INITIAL;
        }
    }

    ui->lblAlignment->setText(QString(Utils::getBaseName(msa_filename).c_str()));
    updateGUI();
}

void jModelTest::on_btnLoadTree_clicked()
{
    QString filters = "MSA (*.phy *.nex *.fas *)";
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
                ui->txtMessages->append(to_qstring("Loaded tree", msg_info));
                ui->tabView->setCurrentIndex(TAB_CONFIG);
                if (state == STATE_ALIGNMENT_LOADED)
                    state = STATE_TREE_LOADED;
            }
            else
            {
                ui->txtMessages->append(to_qstring("Tip number does not match", msg_error));
                ui->tabView->setCurrentIndex(TAB_CONSOLE);
                utree_filename = "";
            }
        }
        else
        {
            ui->txtMessages->append(to_qstring("Cannot read tree", msg_error));
            ui->tabView->setCurrentIndex(TAB_CONSOLE);
            utree_filename = "";
        }

        if (ui->radTopoU->isChecked())
            ui->radTopo4->setChecked(!utree_filename.compare(""));
        else
            ui->radTopoU->setChecked(utree_filename.compare(""));
    }

    ui->lblTree->setText(QString(Utils::getBaseName(utree_filename).c_str()));
    updateGUI();
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
    /* SKIP */
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
        state = STATE_INITIAL;
        ui->lblAlignment->setText("");
        ui->lblTree->setText("");
        msa_filename = "";
        resetSettings();
    }
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
    msgBox.setText("If you change the settings, current results will be discarded.");
    msgBox.setInformativeText("Are you sure?");
    msgBox.setStandardButtons(QMessageBox::Reset | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    if (ret == QMessageBox::Reset)
        resetSettings();
}

void jModelTest::evaluate_models(ModelTest &mtest)
{
    const std::vector<Model *> models = mtest.get_models();
    for (size_t i=0; i<models.size(); i++)
    {
        Model * model = models[i];
        mtest.evaluate_single_model(model);
            updateGUI();
    }
}

void jModelTest::evalmodels(int i, int thread_id)
{
    const std::vector<Model *> modelsPtr = mtest->get_models();
    Model * model = modelsPtr[i];
    double tolerance = ui->txtParEpsilon->text().toDouble();
    double epsilon = ui->txtOptEpsilon->text().toDouble();

    if (!model->is_optimized())
        mtest->evaluate_single_model(model, thread_id, tolerance, epsilon);

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
    int number_of_threads = ui->sliderNThreads->value();

    QMessageBox msgBox;
    msgBox.setText("Start models optimizaion");
    msgBox.setInformativeText("Are you sure?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    if (ret != QMessageBox::Ok)
        return;

    if (ui->radTopo1->isChecked())
        start_tree = tree_mp;
    else if (ui->radTopo2->isChecked())
        start_tree = tree_ml_jc_fixed;
    else if (ui->radTopo3->isChecked())
        start_tree = tree_ml_gtr_fixed;
    else if (ui->radTopo4->isChecked())
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

    mt_options opts;
    opts.model_params = model_params;
    opts.n_catg = ui->sliderNCat->value();
    opts.msa_filename = msa_filename;
    opts.tree_filename = utree_filename;
    opts.partitions_filename = "";
    opts.candidate_models = matrices;
    opts.starting_tree = start_tree;
    opts.datatype = ui->radDatatypeDna->isChecked()?dt_dna:dt_protein;

    bool ok_inst = mtest->build_instance(opts, ui->radSchemes203->isChecked());

    if (c_models.size())
    {
        mtest->set_models(c_models);
    }

    state = STATE_MODELS_OPTIMIZING;
    ui->tabView->setCurrentIndex(TAB_RUN);
    updateGUI();

    QThreadPool::globalInstance()->setMaxThreadCount(number_of_threads);

    /* print settings */
    {
    ui->txtMessages->append("");
    ui->txtMessages->append("Execution options:");
    ui->txtMessages->append(to_qstring("Alignment %1", msg_info).arg(msa_filename.c_str()));
    ui->txtMessages->append(to_qstring("Tree      %1", msg_info).arg(utree_filename.c_str()));
    if (ui->radDatatypeDna->isChecked())
        ui->txtMessages->append("DataType  DNA");
    else
        ui->txtMessages->append("DataType  Protein");
    ui->txtMessages->append(to_qstring("N.Models  %1", msg_info).arg(ui->lblNumModels->text()));
    }

    //QFuture<void> fut = QtConcurrent::run(evalmodels, mtest, ui);
    const std::vector<Model *> & modelsPtr = mtest->get_models();
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
    state = STATE_MODELS_OPTIMIZED;

    dialog.exec();
    futureWatcher.waitForFinished();
    on_run = false;

    ModelSelection bic_selection(modelsPtr, ic_bic);
    fill_results(ui->tblResultsBic, bic_selection);
    ModelSelection aic_selection(modelsPtr, ic_aic);
    fill_results(ui->tblResultsAic, aic_selection);
    ModelSelection aicc_selection(modelsPtr, ic_aicc);
    fill_results(ui->tblResultsAicc, aicc_selection);
    ModelSelection dt_selection(modelsPtr, ic_dt);
    fill_results(ui->tblResultsDt, dt_selection);

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

}
