#include "xmodeltest.h"
#include "ui_xmodeltest.h"

#include "service/modeltestservice.h"
#include "meta.h"
#include "gui/progressdialog.h"
#include "gui/datainfodialog.h"

#ifdef QT_WIDGETS_LIB
#include <QtWidgets>
#else
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtConcurrentRun>
#endif

#include <iomanip>

using namespace std;

static unsigned int model_index;

static void enable(QPushButton * button, bool new_stat, bool set = false)
{
    button->setEnabled(new_stat);
    if (new_stat)
    {
        if (set)
            button->setStyleSheet("border:1px solid #005;border-radius: 10px;\ntext-align: left;padding-left: 5px;background-color: #cfefa8;");
        else
            button->setStyleSheet("border:1px solid #050;border-radius: 10px;text-align: left;padding-left: 5px;");
    }
    else
    {
        button->setStyleSheet("border:1px solid #999;border-radius: 10px;\ntext-align: left;padding-left: 5px;color:999;");
    }
}

size_t xmodeltest::compute_size(int n_cats, int n_threads)
{
    size_t mem_b = 0;
    int states = ui->radDatatypeDna->isChecked()?
                N_DNA_STATES :
                N_PROT_STATES;

    if (n_taxa && n_sites && n_cats && states)
    {
        mem_b = modeltest::Utils::mem_size(n_taxa,
                        n_sites,
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
    qRegisterMetaType<partition_id_t>();
    qRegisterMetaType<mt_size_t>();

    ui->setupUi(this);

//    ui->frame_header->setStyleSheet("color: #4b0c59;\nbackground-color: #cfefa8;");
//    ui->centralwidget->setStyleSheet("color: #333;\nbackground-color: #f1fff4;");
//    ui->frame_settings->setStyleSheet("color: #333;\nbackground-color: #cfefa8;");
//    ui->frame_models->setStyleSheet("color: #333;\nbackground-color: #b1df78;");
//    ui->frame_advanced->setStyleSheet("color: #333;\nbackground-color: #ba8bc4;");
    n_taxa = 0;
    n_sites = 0;
    scheme = 0;
    status = st_active;

    msa_filename = "";
    utree_filename = "";
    parts_filename = "";

    enable(ui->tool_help, true);

    ui->sliderNThreads->setRange(1, QThread::idealThreadCount());
    ui->sliderNThreads->setValue(modeltest::Utils::count_physical_cores());

    on_radDatatypeDna_clicked();

    /* Redirect Console output to QTextEdit */
    redirect = new MyDebugStream(std::cout);
    QObject::connect(redirect, SIGNAL(newText(char *)), this, SLOT(set_text(char *)), Qt::QueuedConnection);

    datainfo_dialog = 0;
    results_dialog = 0;
    models_dialog = 0;

#ifdef QT_WIDGETS_LIB
    addAction(ui->mnu_open_msa);
    addAction(ui->mnu_open_tree);
    addAction(ui->mnu_open_parts);
    addAction(ui->mnu_run);
    addAction(ui->mnu_toggle_settings);
    addAction(ui->mnu_copy);
    addAction(ui->mnu_reset);
    addAction(ui->mnu_index);
    addAction(ui->mnu_about);
    addAction(ui->mnu_view_datainfo);
    addAction(ui->mnu_models);
    addAction(ui->mnu_results);
#endif

    reset_xmt();
}

xmodeltest::~xmodeltest()
{
    ModelTestService::instance()->destroy_instance();
    delete redirect;
    delete ui;
}

void xmodeltest::reset_xmt( void )
{
    status = st_active;

    n_taxa  = 0;
    n_sites = 0;

    msa_filename   = "";
    utree_filename = "";
    parts_filename = "";
    asc_filename   = "";

    ui->lbl_msa->setText("none");
    ui->lbl_tree->setText("none");
    ui->lbl_parts->setText("none");

    ui->consoleRun->clear();
    Meta::print_header();

    if (scheme)
    {
        delete scheme;
        scheme = 0;
    }

    ModelTestService::instance()->destroy_instance();

    if (datainfo_dialog)
        delete datainfo_dialog;
    datainfo_dialog = 0;

    if (results_dialog)
        delete results_dialog;
    results_dialog = 0;

    if (models_dialog)
        delete models_dialog;
    models_dialog = 0;
}

void xmodeltest::update_gui( void )
{
    char txt[30];
    int n_model_sets, n_matrices, n_models;


    /* topology search is not available! */
    ui->radTopoFixedGtr->setEnabled(false);
    ui->radTopoFixedJc->setEnabled(false);
    ui->radTopoFixedMp->setEnabled(false);
    ui->radTopoML->setEnabled(false);

    bool enable_open_msa = (status & st_active) && !(status & st_optimized);
    ui->mnu_open_msa->setEnabled(enable_open_msa);
    enable(ui->tool_open_msa,
           enable_open_msa,
           status & st_msa_loaded);

    bool enable_open_tree = (status & st_msa_loaded) && !(status & st_optimized);
    ui->radTopoU->setEnabled(enable_open_tree);
    ui->mnu_open_tree->setEnabled(enable_open_tree);
    enable(ui->tool_open_tree,
           enable_open_tree,
           status & st_tree_loaded);

    bool enable_open_parts = (status & st_msa_loaded) && !(status & st_optimized);
    ui->mnu_open_parts->setEnabled(enable_open_parts);
    enable(ui->tool_open_parts,
           enable_open_parts,
           status & st_parts_loaded);

    if (enable_open_msa)
        ui->frame_tool_load->setStyleSheet("background-color: #c1d8ff;");
    else
        ui->frame_tool_load->setStyleSheet("background-color: #fff2db;");

    bool enable_run = (status & st_msa_loaded) && !(status & st_optimized);
    ui->mnu_run->setEnabled(enable_run);
    enable(ui->tool_run,
           enable_run,
           status & st_optimized);

    if (enable_run)
    {
        ui->frame_tool_setup->setStyleSheet("background-color: #c1d8ff;");
        ui->frame_tool_run->setStyleSheet("background-color: #c1d8ff;");
    }
    else
    {
        ui->frame_tool_setup->setStyleSheet("background: #fff2db;");
        ui->frame_tool_run->setStyleSheet("background: #fff2db;");
    }

    bool enable_settings = !(status & st_optimized);
    enable(ui->tool_settings,
           enable_settings,
           ui->tool_settings->isChecked());
    ui->mnu_toggle_settings->setEnabled(enable_settings);
    ui->tool_settings->setChecked(ui->tool_settings->isChecked() &&
                                  enable_settings);

    bool enable_results = status & st_optimized;
    enable(ui->tool_results,
           enable_results);
    ui->mnu_results->setEnabled(enable_results);

    bool enable_models = status & st_optimized;
    ui->mnu_models->setEnabled(enable_models);
    enable(ui->tool_models,
           enable_models);

    if (enable_results)
        ui->frame_tool_results->setStyleSheet("background-color: #c1d8ff;");
    else
        ui->frame_tool_results->setStyleSheet("background-color: #fff2db;");

    enable(ui->tool_reset,
           status & st_active);

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

    if (ui->radDatatypeDna->isChecked())
    {
        sprintf(txt, "DNA data");
        ui->lbl_datatype->setText(txt);
    }
    else if (ui->radDatatypeProt->isChecked())
    {
        sprintf(txt, "Protein data");
        ui->lbl_datatype->setText(txt);
        if (!(ui->radSetModelTest->isChecked()))
        {
            for (mt_index_t i=0; i<N_PROT_MODEL_MATRICES; i++)
                ui->modelsListView->item(i)->setCheckState(Qt::CheckState::Unchecked);
            if (ui->radSetMrbayes->isChecked())
                for (mt_index_t i : prot_mrbayes_matrices_indices)
                    ui->modelsListView->item(i)->setCheckState(Qt::CheckState::Checked);
            else if (ui->radSetPhyml->isChecked())
                for (mt_index_t i : prot_phyml_matrices_indices)
                    ui->modelsListView->item(i)->setCheckState(Qt::CheckState::Checked);
            else if (ui->radSetRaxml->isChecked())
                for (mt_index_t i : prot_raxml_matrices_indices)
                    ui->modelsListView->item(i)->setCheckState(Qt::CheckState::Checked);
            else if (ui->radSetPAUP->isChecked())
                for (mt_index_t i : prot_paup_matrices_indices)
                    ui->modelsListView->item(i)->setCheckState(Qt::CheckState::Checked);
        }
    }
    else
        assert(0);

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
    sprintf(txt, "%d models", n_models);
    ui->lbl_nmodels->setText(QString(txt));

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

    /* ascertainment bias correction */
    ui->frame_ascbias->setEnabled(!(ui->cbIModels->isChecked() || ui->cbIGModels->isChecked()));

    setStyleSheet("QToolTip { color: #000000; background-color: #aaa2da; border: none; } QToolTip:disabled { color: #000000; background-color: #aaa2da; border: none; }");
}

void xmodeltest::run_modelselection()
{
    partition_id_t part_id;
    int number_of_threads  = ui->sliderNThreads->value();

    QMessageBox msgBox;
    msgBox.setText("Start models optimizaion");
    msgBox.setInformativeText("Are you sure?");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    if (ret != QMessageBox::Ok)
        return;

    model_index = 0;

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

    if (ui->radAscbiasNone->isChecked())
        asc_bias = asc_none;
    else if (ui->radAscbiasLewis->isChecked())
        asc_bias = asc_lewis;
    else if (ui->radAscbiasFelsenstein->isChecked())
        asc_bias = asc_felsenstein;
    else if (ui->radAscbiasStamatakis->isChecked())
        asc_bias = asc_stamatakis;
    else
        assert(0);

    /* validate */
    if (ui->radAscbiasFelsenstein->isChecked() || ui->radAscbiasStamatakis->isChecked())
    {
        bool ascbias_file_ok = false;

        /* TODO: validate asc bias file */

        if (!ascbias_file_ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Invalid composition file for ascertainment bias correction");
            msgBox.setInformativeText("Set a good one or select None or Lewis");
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.exec();
            toggle_settings(true);
            return;
        }
    }

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

    opts.msa_format = msa_format;
    opts.asc_bias_corr = asc_bias;
    opts.n_taxa = n_taxa;
    opts.n_sites = n_sites;
    opts.rnd_seed = 12345;
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
    opts.output_tree_to_file = false;
    opts.partitions_desc = NULL;
    opts.partitions_eff = NULL;
    opts.n_threads = number_of_threads;
    opts.epsilon_param = ui->txtParEpsilon->text().toDouble();
    opts.epsilon_opt = ui->txtOptEpsilon->text().toDouble();
    opts.verbose = VERBOSITY_LOW;

    data_type_t datatype = ui->radDatatypeDna->isChecked()?dt_dna:dt_protein;

    if (!scheme)
    {
        /* create single partition / single region */
        scheme = new partitioning_scheme_t();
        partition_region_t region;
        partition_descriptor_t partition;
        region.start = 1;
        region.end = n_sites;
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

    ui->consoleRun->append(xutils::to_qstring("Building modeltest instance for %1 threads", msg_lvl_info).arg(number_of_threads));

    bool ok_inst = ModelTestService::instance()->reset_instance(opts);
    if (!ok_inst)
    {
        ui->consoleRun->append(xutils::to_qstring("Error building instance [%1]", msg_lvl_error).arg(modeltest::mt_errno));
        ui->consoleRun->append(xutils::to_qstring(modeltest::mt_errmsg, msg_lvl_error));
        return;
    }

    /* print settings */
   Meta::print_options(opts);

   modeltest::PartitioningScheme & partitioning_scheme = ModelTestService::instance()->get_partitioning_scheme();

   //TODO: FIX
   mt_size_t n_models;
   if (partitioning_scheme.get_size() == 1)
   {
       part_id = {0};
        mythread = new xThreadOpt(part_id,
                                  number_of_threads,
                                  opts.epsilon_param,
                                  opts.epsilon_opt);
   }
   else
   {
       mythread = new xThreadOpt(partitioning_scheme,
                                 number_of_threads,
                                 opts.epsilon_param,
                                 opts.epsilon_opt);
   }
    n_models = mythread->get_number_of_models();

    QObject::connect(mythread,
                     SIGNAL(optimization_done()),
                     this,
                     SLOT(optimization_done()));
    QObject::connect(mythread,
                     SIGNAL(optimization_interrupted()),
                     this,
                     SLOT(optimization_interrupted()));
    QObject::connect(mythread,
                     SIGNAL(optimized_single_done(modeltest::Model *, unsigned int)),
                     this,
                     SLOT(optimized_single_model(modeltest::Model *, unsigned int)));
    QObject::connect(mythread,
                     SIGNAL(optimized_partition_done(partition_id_t)),
                     this,
                     SLOT(optimized_partition(partition_id_t)));

    ProgressDialog dialog( n_models,
                           number_of_threads );

    QObject::connect(mythread, SIGNAL(optimization_done( void )), &dialog, SLOT(reset( void )));
    QObject::connect(mythread,
                     SIGNAL(optimized_single_done(modeltest::Model *, unsigned int)),
                     &dialog,
                     SLOT(optimized_single_model( void )));
    QObject::connect(mythread, SIGNAL(next_model(modeltest::Model*,uint)), &dialog, SLOT(set_model_running(modeltest::Model*,uint)));
    QObject::connect(mythread, SIGNAL(next_parameter(uint,double,uint)), &dialog, SLOT(set_delta_running(uint,double,uint)));
    QObject::connect(&dialog, SIGNAL(canceled()), mythread, SLOT(kill_threads()));

    status &= ~st_optimized;
    status |= st_optimizing;
    modeltest::on_run = true;

    mythread->start();
    dialog.exec();
}

void xmodeltest::action_run( void )
{
    if (!ui->tool_run->isEnabled())
        return;

    if (!ui->radTopoU->isChecked())
    {
        QMessageBox::warning(0, "We are sorry...", "<p>ModelTest Light works only with fixed user-defined trees so far</p>");
        return;
    }

    toggle_settings(false);
    update_gui();
    ini_t = time(NULL);
    run_modelselection();
    update_gui();
}

void xmodeltest::action_viewresults( void )
{
    if (!results_dialog)
    {
        results_dialog = new ResultsDialog(ModelTestService::instance()->get_partitioning_scheme(), msa_filename);
    }
    results_dialog->show();
    results_dialog->raise();

    update_gui();
}

void xmodeltest::action_viewmodels( void )
{
    if (!models_dialog)
        models_dialog = new ModelsDialog(ModelTestService::instance()->get_partitioning_scheme());
    models_dialog->show();
    models_dialog->raise();
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
    if (!ui->tool_open_msa->isEnabled())
        return;

    if (datainfo_dialog)
        delete datainfo_dialog;
    datainfo_dialog = 0;

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
            data_type_t test_dt;
            if (modeltest::ModelTest::test_msa(msa_filename,
                                     &n_taxa,
                                     &n_sites,
                                     &msa_format,
                                     &test_dt))
            {
                cout << endl << "Loaded alignment" << endl;

                if (test_dt == dt_dna)
                {
                    ui->radDatatypeDna->setChecked(true);
                    on_radDatatypeDna_clicked();
                }
                else if (test_dt == dt_protein)
                {
                    ui->radDatatypeProt->setChecked(true);
                    on_radDatatypeProt_clicked();
                }
                ui->consoleRun->append(xutils::to_qstring("%1", msg_lvl_notify).arg(msa_filename.c_str()));
                cout << "Format:          ";
                switch(msa_format)
                {
                case mf_fasta:
                    cout << "FASTA" << endl;
                    break;
                case mf_phylip:
                    cout << "PHYLIP" << endl;
                    break;
                case mf_undefined:
                    cout << "Undefined" << endl;
                    break;
                }
                cout << "Datatype:        " << (test_dt == dt_protein?"Protein":"DNA") << endl;
                cout << "Num.Sequences:   " << n_taxa << endl;
                cout << "Sequence Length: " << n_sites << endl;
                status |= st_msa_loaded;
            }
            else
            {
                ui->consoleRun->append(xutils::to_qstring("Error: Cannot load alignment %1", msg_lvl_error).arg(msa_filename.c_str()));
                msa_filename   = "";
                status &= ~st_msa_loaded;
            }
        }
    }

    if (status & st_msa_loaded)
    {
        ui->lbl_msa->setText(QString(modeltest::Utils::getBaseName(msa_filename).c_str()));
        int n_cats = ui->sliderNCat->value();
        if (!(ui->cbGModels->isChecked() || ui->cbIGModels->isChecked()))
            n_cats = 1;
        compute_size(n_cats, ui->sliderNThreads->value());
    }
    else
    {
        ui->lbl_msa->setText("none");
        ui->lbl_tree->setText("none");
        ui->lbl_parts->setText("none");
    }

    update_gui();
}

void xmodeltest::action_open_tree()
{
    if (!ui->tool_open_tree->isEnabled())
        return;

    if (datainfo_dialog)
        delete datainfo_dialog;
    datainfo_dialog = 0;

    QString filters = tr("Newick tree(*.tree *.newick);; All files(*)");
    QString file_name;
    if (status & st_optimized)
    {
            QMessageBox msgBox;
            msgBox.setText("Tree cannot be set after optimization");
            msgBox.setInformativeText("You must reset ModelTest before");
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.exec();
            return;
    }

    file_name = QFileDialog::getOpenFileName(this,
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
            if (n_tips == n_taxa)
            {
                cout << endl << "Loaded tree" << endl;
                ui->consoleRun->append(xutils::to_qstring("%1", msg_lvl_notify).arg(utree_filename.c_str()));
                status |= st_tree_loaded;
            }
            else
            {
                ui->consoleRun->append(xutils::to_qstring("Error: Tip number does not match in %1", msg_lvl_error).arg(utree_filename.c_str()));
                utree_filename = "";
                status &= ~st_tree_loaded;
            }
        }
        else
        {
            ui->consoleRun->append(xutils::to_qstring("%1", msg_lvl_error).arg(modeltest::mt_errmsg));
            utree_filename = "";
            status &= ~st_tree_loaded;
        }

        if (ui->radTopoU->isChecked())
            ui->radTopoFixedGtr->setChecked(!(status & st_tree_loaded));
        else
            ui->radTopoU->setChecked(status & st_tree_loaded);

        if (!(status & st_tree_loaded))
            ui->tool_settings->setChecked(false);
    }

    if (status & st_tree_loaded)
    {
        ui->lbl_tree->setText(QString(modeltest::Utils::getBaseName(utree_filename).c_str()));
    }
    else
    {
        ui->lbl_tree->setText("none");
        ui->radTopoFixedGtr->setChecked(true);
    }

    update_gui();
}

void xmodeltest::action_open_parts()
{
    if (!ui->tool_open_parts->isEnabled())
        return;

    if (datainfo_dialog)
        delete datainfo_dialog;
    datainfo_dialog = 0;

    QString filters = tr("Partitions file(*.parts *.model *.conf);; All files(*)");
    QString file_name;
    if (status & st_optimized)
    {
            QMessageBox msgBox;
            msgBox.setText("Partitions cannot be set after optimization");
            msgBox.setInformativeText("You must reset ModelTest before");
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.exec();
            return;
    }

    file_name = QFileDialog::getOpenFileName(this,
                                             tr("Open File"),
                                             "",
                                             filters);
    const std::string loaded_file = file_name.toStdString();

    if ( loaded_file.compare(""))
    {
        parts_filename = loaded_file;
        scheme = modeltest::Utils::parse_partitions_file(parts_filename);
        if (!scheme)
        {
            parts_filename = "";
            ui->consoleRun->append(xutils::to_qstring("Error loading partitions %1", msg_lvl_error).arg(parts_filename.c_str()));
            ui->consoleRun->append(xutils::to_qstring(modeltest::mt_errmsg, msg_lvl_error));
            if (scheme)
            {
                delete scheme;
                scheme = 0;
            }
            status &= ~st_parts_loaded;
        }
        else
        {
            if (!modeltest::ModelTest::test_partitions(*scheme, n_sites))
            {
                ui->consoleRun->append(xutils::to_qstring("Error loading partitions %1", msg_lvl_error).arg(parts_filename.c_str()));
                ui->consoleRun->append(xutils::to_qstring(modeltest::mt_errmsg, msg_lvl_error));
                parts_filename = "";
                delete scheme;
                scheme = 0;
                status &= ~st_parts_loaded;
            }
            else
            {
                ui->consoleRun->append(xutils::to_qstring("Loaded %1 partitions %2", msg_lvl_notify).arg(scheme->size()).arg(parts_filename.c_str()));
                if (modeltest::mt_errno)
                {
                    /* there are warnings */
                    ui->consoleRun->append("Warning: " + xutils::to_qstring(modeltest::mt_errmsg, msg_lvl_error));
                }
                status |= st_parts_loaded;
            }
        }
    }

    if (status & st_parts_loaded)
    {
        ui->lbl_parts->setText(QString(modeltest::Utils::getBaseName(parts_filename).c_str()));
    }
    else
    {
        ui->lbl_parts->setText("none");
    }

    update_gui();
}

void xmodeltest::action_view_datainfo( void )
{
    if (!datainfo_dialog)
        datainfo_dialog = new DataInfoDialog(msa_filename, n_taxa, n_sites, utree_filename, 1.0, *scheme, parts_filename);
    datainfo_dialog->show();
}

/** SETTINGS **/

void xmodeltest::autoSelectSchemes(const int schemes[], int n)
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

void xmodeltest::set_substitution_schemes(mt_index_t n_schemes)
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

/* SLOTS */

void xmodeltest::set_text(char * message)
{
    //TODO: Improve this allocation/deallocation
    ui->consoleRun->append(message);
    free(message);
    ui->consoleRun->show();
}

void xmodeltest::optimized_single_model(modeltest::Model * model, unsigned int n_models )
{
            /* print progress */
        cout << setw(5) << right << (++model_index) << "/"
             << setw(5) << left << n_models
             << setw(15) << left << model->get_name()
             << setw(18) << right << setprecision(MT_PRECISION_DIGITS) << fixed
             << model->get_lnl() << endl;
}

void xmodeltest::optimized_partition( partition_id_t part_id )
{
    UNUSED(part_id);
    cout << "Finished partition " << endl;
}

void xmodeltest::optimization_done( )
{
    //TODO: FIX
//    const std::vector<modeltest::Model *> & modelsPtr = ModelTestService::instance()->get_modeltest()->get_models(part_id);
//        QVector<int> models;
//        for (int i=0; (size_t)i < modelsPtr.size(); i++)
//            models.append(i);

        status &= ~st_optimizing;
        status |= st_optimized;

        modeltest::on_run = false;

        update_gui();

        cout << setw(80) << setfill('-') << "" << setfill(' ') << endl;
        cout << "optimization done! It took " << time(NULL) - ini_t << " seconds" << endl;

        delete mythread;
}

void xmodeltest::optimization_interrupted()
{
        status &= ~st_optimizing;
        status &= ~st_optimized;

        modeltest::on_run = false;

        update_gui();

        cout << setw(80) << setfill('-') << "" << setfill(' ') << endl;
        cout << "optimization interrupted after " << time(NULL) - ini_t << " seconds" << endl;

        ModelTestService::instance()->destroy_instance();
        delete mythread;
}
