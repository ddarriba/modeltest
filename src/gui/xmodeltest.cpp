#include "xmodeltest.h"
#include "ui_xmodeltest.h"

#include "../utils.h"
#include "gui/xthreadopt.h"
#include "gui/resultswidget.h"

#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtConcurrentRun>
#include <iomanip>

using namespace std;
using namespace modeltest;

static void enable(QToolButton * button, bool new_stat, bool set = false)
{
    button->setEnabled(new_stat);
    if (new_stat)
    {
        if (set)
            button->setStyleSheet("color: #333;\nbackground-color: #ba8bc4;");
            //button->setStyleSheet("color: #333;\nbackground-color: #7a3788;");
        else
            button->setStyleSheet("color: #333;\nbackground-color: #cfefa8;");
            //button->setStyleSheet("color: #333;\nbackground-color: #88bc49;");
    }
    else
    {
        //button->setStyleSheet("color: #f59983;\nbackground-color: #ffc2b3;");
        button->setStyleSheet("color: #999;\nbackground-color: #ddd;");
    }
}

size_t xmodeltest::compute_size(int n_cats, int n_threads)
{
    size_t mem_b = 0;
    int states = ui->radDatatypeDna->isChecked()?
                N_DNA_STATES :
                N_PROT_STATES;

    if (n_seqs && seq_len && n_cats && states)
    {
        mem_b = Utils::mem_size(n_seqs,
                        seq_len,
                        n_cats,
                        states);
        mem_b *= n_threads;

        /* overestimating factor */
        mem_b *= 1.2;
        mem_b /= (1024*1024);

        ui->lbl_mem->setText(QString("%1 MB").arg(
                    QString::number(
                        mem_b)));

        if (mem_b > 1000)
            ui->lbl_mem->setStyleSheet("color: #f00;");
        else
            ui->lbl_mem->setStyleSheet("");
    }
    else
    {
        ui->lbl_mem->setText("-");
    }
    return (mem_b);
}

xmodeltest::xmodeltest(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::xmodeltest)
{
    ui->setupUi(this);
    ui->frame_header->setStyleSheet("color: #4b0c59;\nbackground-color: #cfefa8;");
    ui->toolBar->setStyleSheet("color: #4b0c59;\nbackground-color: #f1fff4;");
    ui->centralwidget->setStyleSheet("color: #333;\nbackground-color: #f1fff4;");
    ui->frame_settings->setStyleSheet("color: #333;\nbackground-color: #cfefa8;");
    ui->frame_models->setStyleSheet("color: #333;\nbackground-color: #b1df78;");
    ui->frame_advanced->setStyleSheet("color: #333;\nbackground-color: #ba8bc4;");
    n_seqs = 0;
    seq_len = 0;
    mtest = 0;
    scheme = 0;
    status = st_active;

    msa_filename = "";
    utree_filename = "";
    parts_filename = "";

    enable(ui->tool_help, true);

    ui->sliderNThreads->setRange(1, QThread::idealThreadCount());
    ui->sliderNThreads->setValue(QThread::idealThreadCount());

    on_radDatatypeDna_clicked();
    update_gui();

    /* Redirect Console output to QTextEdit */
//    redirect = new Q_DebugStream(std::cout, ui->consoleRun);
    redirect = new MyDebugStream(std::cout);
    QObject::connect(redirect, SIGNAL(newText(QString)), this, SLOT(setText(QString)), Qt::QueuedConnection);

    reset_xmt();

    ui->mdiArea->addSubWindow(new resultsWidget(this));
}

xmodeltest::~xmodeltest()
{
    delete redirect;
    delete ui;
}

void xmodeltest::reset_xmt( void )
{
    n_seqs = 0;
    seq_len = 0;
    status = st_active;

    msa_filename = "";
    utree_filename = "";
    parts_filename = "";

    ui->lbl_msa->setText("-");
    ui->lbl_tree->setText("-");
    ui->lbl_parts->setText("-");

    ui->consoleRun->clear();
    Utils::print_header();
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
    enable(ui->tool_open_tree, status & st_msa_loaded, status & st_tree_loaded);
    enable(ui->tool_open_parts, status & st_msa_loaded, status & st_parts_loaded);
    enable(ui->tool_run, status & st_msa_loaded, status & st_optimized);
    enable(ui->tool_settings, !(status & st_optimized), ui->tool_settings->isChecked());
    enable(ui->tool_results, status & st_optimized);
    enable(ui->tool_reset, status & st_active);

    ui->frame_settings->setVisible(ui->tool_settings->isChecked());
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

    ui->frame_advanced->setVisible(ui->cbAdvanced->isChecked());
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
    compute_size(n_cats, ui->sliderNThreads->value());
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

void xmodeltest::run_modelselection()
{
    cerr << "Get Partition" << endl;
    partition_id_t part_id = {0};
    int number_of_threads  = ui->sliderNThreads->value();

    cerr << "Retrieve options" << endl;

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

    cerr << "Build modeltest for " << number_of_threads << " threads" << endl;

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

    std::vector<mt_index_t> matrices;
    if (ui->radDatatypeProt->isChecked())
    {
        for (int i=0; i < ui->modelsListView->count(); i++)
        {
            if (ui->modelsListView->item(i)->checkState() == Qt::CheckState::Checked)
                matrices.push_back(i);
        }
    }
    else
    {
        if (!ui->radSchemes203->isChecked())
        {
            for (int i=0; i < ui->modelsListView->count(); i++)
            {
                if (ui->modelsListView->item(i)->checkState() == Qt::CheckState::Checked)
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

    cerr << "Fill options" << endl;

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
    opts.n_threads = number_of_threads;
    data_type datatype = ui->radDatatypeDna->isChecked()?dt_dna:dt_protein;

    if (!scheme)
    {
        /* create single partition / single region */
        cerr << "Create partitioning scheme" << endl;
        scheme = new partitioning_scheme_t();
        partition_region_t region;
        partition_t partition;
        region.start = 1;
        region.end = seq_len;
        region.stride = 1;
        partition.datatype = datatype;
        partition.states = datatype==dt_dna?N_DNA_STATES:N_PROT_STATES;
        partition.partition_name = "DATA";
        partition.regions.push_back(region);
        scheme->push_back(partition);
    }
    opts.partitions_desc = scheme;
    //TODO: Create option for smoothing
    opts.smooth_freqs = false;

    cerr << "Build modeltest instance" << endl;

    bool ok_inst = mtest->build_instance(opts);
    if (!ok_inst)
    {
        ui->consoleRun->append(to_qstring("Error building instance [%1]", msg_lvl_error).arg(mt_errno));
        ui->consoleRun->append(to_qstring(mt_errmsg, msg_lvl_error));
        return;
    }

    cerr << "Set candidate models" << endl;
    if (c_models.size())
    {
        mtest->set_models(c_models, part_id);
    }

    /* print settings */
    modeltest::Utils::print_options(opts);

    xThreadOpt * mythread = new xThreadOpt(mtest, part_id, number_of_threads);
    QObject::connect(mythread, SIGNAL(optimization_done(partition_id_t)), this, SLOT(optimization_done(partition_id_t)));

    status &= ~st_optimized;
    status |= st_optimizing;
    mythread->start();

    on_run = true;


//    updateGUI();
}

void xmodeltest::optimization_done( partition_id_t part_id )
{
    const std::vector<Model *> & modelsPtr = mtest->get_models(part_id);
        QVector<int> models;
        for (int i=0; (size_t)i < modelsPtr.size(); i++)
            models.append(i);

        status &= ~st_optimizing;
        status |= st_optimized;

        on_run = false;

        //TODO: WAIT FOR FINISHING!

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
}

void xmodeltest::action_run( void )
{
    run_modelselection();
    update_gui();
}

void xmodeltest::action_reset( void )
{
    QMessageBox msgBox;
    msgBox.setText("This action will reset modeltest");
    msgBox.setInformativeText("Are you sure?");
    msgBox.setStandardButtons(QMessageBox::Reset | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    if (ret == QMessageBox::Reset)
    {
        reset_xmt();
        update_gui();
    }
}

void xmodeltest::action_open_msa()
{
    QString filters = tr("Multiple Sequence Alignment(*.phy *.nex *.fas);; All files(*)");
    QString file_name = QFileDialog::getOpenFileName(this,
                                                    tr("Open File"),
                                                    "",
                                                    filters);

    const std::string loaded_file = file_name.toStdString();
    bool load_file = true;

    if ( loaded_file.compare(""))
    {
        if (msa_filename.compare(""))
        {
            load_file = false;
            if (loaded_file.compare(msa_filename))
            {
                QMessageBox msgBox;
                msgBox.setText("Setting a different MSA will reset modeltest");
                msgBox.setInformativeText("Are you sure?");
                msgBox.setStandardButtons(QMessageBox::Reset | QMessageBox::Cancel);
                msgBox.setDefaultButton(QMessageBox::Cancel);
                int ret = msgBox.exec();
                if (ret == QMessageBox::Reset)
                {
                    reset_xmt();
                    load_file = true;
                }
            }
        }

        if (load_file)
        {
            msa_filename = loaded_file;
            if (ModelTest::test_msa(msa_filename,
                                     &n_seqs,
                                     &seq_len))
            {
                cout << endl << "Loaded alignment" << endl;
                ui->consoleRun->append(to_qstring("%1", msg_lvl_notify).arg(msa_filename.c_str()));
                cout << "Num.Sequences:   " << n_seqs << endl;
                cout << "Sequence Length: " << seq_len << endl;
                status |= st_msa_loaded;
            }
            else
            {
                ui->consoleRun->append(to_qstring("Error: Cannot load alignment %1", msg_lvl_error).arg(msa_filename.c_str()));
                msa_filename   = "";
                status &= ~st_msa_loaded;
            }
        }
    }

    if (status & st_msa_loaded)
    {
        ui->lbl_msa->setText(QString(Utils::getBaseName(msa_filename).c_str()));
        int n_cats = ui->sliderNCat->value();
        if (!(ui->cbGModels->isChecked() || ui->cbIGModels->isChecked()))
            n_cats = 1;
        compute_size(n_cats, ui->sliderNThreads->value());
    }
    else
        ui->lbl_tree->setText("-");

    update_gui();
}

void xmodeltest::action_open_tree()
{
    QString filters = tr("Newick tree(*.tree *.newick);; All files(*)");
    QString file_name = QFileDialog::getOpenFileName(this,
                                                    tr("Open File"),
                                                    "",
                                                    filters);
    const std::string loaded_file = file_name.toStdString();

    if ( loaded_file.compare(""))
    {
        mt_size_t n_tips;

        utree_filename = loaded_file;
        if (modeltest::ModelTest::test_tree(utree_filename,
                                 &n_tips))
        {
            if (n_tips == n_seqs)
            {
                cout << endl << "Loaded tree" << endl;
                ui->consoleRun->append(to_qstring("%1", msg_lvl_notify).arg(utree_filename.c_str()));
                status |= st_tree_loaded;
            }
            else
            {
                ui->consoleRun->append(to_qstring("Error: Tip number does not match in %1", msg_lvl_error).arg(utree_filename.c_str()));
                utree_filename = "";
                status &= ~st_tree_loaded;
            }
        }
        else
        {
            ui->consoleRun->append(to_qstring("%1", msg_lvl_error).arg(mt_errmsg));
            utree_filename = "";
            status &= ~st_tree_loaded;
        }

        if (ui->radTopoU->isChecked())
            ui->radTopoFixedGtr->setChecked(!(status & st_tree_loaded));
        else
            ui->radTopoU->setChecked(status & st_tree_loaded);
    }

    if (status & st_tree_loaded)
        ui->lbl_tree->setText(QString(Utils::getBaseName(utree_filename).c_str()));
    else
        ui->lbl_tree->setText("-");

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

    compute_size(position, ui->sliderNThreads->value());
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
    compute_size(n_cats, value);
}

void xmodeltest::setText(QString message)
{
    ui->consoleRun->append(message);
    ui->consoleRun->show();
}
