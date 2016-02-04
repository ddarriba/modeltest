#ifndef JMODELTEST_H
#define JMODELTEST_H

#include "modeltest.h"
#include "gui/qdebugstream.h"

#include <QMainWindow>
#include <QtGui/QListWidgetItem>
#include <QtGui/QStandardItemModel>
#include <QtGui/QTableView>
#include <QtGui/QLabel>

#include <string>
#include <vector>

#define UNUSED(expr) do { (void)(expr); } while (0)

#define TAB_CONFIG          0
#define TAB_RUN             1
#define TAB_RESULTS         2
#define TAB_CONSOLE         3

#define TAB_RESULTS_BIC  0
#define TAB_RESULTS_AIC  1
#define TAB_RESULTS_AICC 2
#define TAB_RESULTS_DT   3

enum current_state {
    STATE_INITIAL            = 0,
    STATE_ALIGNMENT_LOADED   = 1<<0,
    STATE_TREE_LOADED        = 1<<1,
    STATE_PARTITIONS_LOADED  = 1<<2,
    STATE_MODELS_OPTIMIZING  = 1<<3,
    STATE_MODELS_OPTIMIZED   = 1<<4,
    STATE_DONE               = 1<<5
};

enum msg_level {
    msg_error,
    msg_alert,
    msg_notify,
    msg_info
};

namespace Ui {
class jModelTest;
}

namespace modeltest {

class jModelTest : public QMainWindow
{
    Q_OBJECT

public:
    explicit jModelTest(QWidget *parent = 0);
    ~jModelTest();
    void evalmodels(int i, int thread_id = 0);

signals:
    void finishedModel(Model * model);

private slots:
    void on_btnLoadAlignment_clicked();
    void on_btnLoadTree_clicked();
    void on_btnLoadParts_clicked();

    void on_radSchemes3_clicked();
    void on_radSchemes5_clicked();
    void on_radSchemes7_clicked();
    void on_radSchemes11_clicked();
    void on_radSchemes203_clicked();
    //void on_listMatrices_clicked();

    void on_radTopoU_clicked();
    void on_radTopoFixedMp_clicked();
    void on_radTopoFixedGtr_clicked();
    void on_radTopoFixedJc_clicked();
    void on_radTopoML_clicked();

    void on_radSetModelTest_clicked();
    void on_radSetMrbayes_clicked();
    void on_radSetRaxml_clicked();
    void on_radSetPhyml_clicked();
    void on_radSetModelTest_toggled(bool checked);
    void on_radSetMrbayes_toggled(bool checked);
    void on_radSetRaxml_toggled(bool checked);
    void on_radSetPhyml_toggled(bool checked);

    void on_sliderNCat_sliderMoved(int position);

    /* MENU */
    void on_actionReset_triggered();
    void on_menuFileLoad_triggered();
    void on_menuFileQuit_triggered();
    void on_menuTreeLoad_triggered();
    void on_actionConsole_triggered();
    void on_actionConfigure_triggered();
    void on_actionProgress_triggered();
    void on_actionResults_triggered();

    void on_cbEqualFreq_toggled(bool checked);
    void on_cbMlFreq_toggled(bool checked);
    void on_cbNoRateVarModels_toggled(bool checked);
    void on_cbIModels_toggled(bool checked);
    void on_cbGModels_toggled(bool checked);
    void on_cbIGModels_toggled(bool checked);

    void on_btnResetConfig_clicked();

    void on_btnRun_clicked();

    void on_cbAdvanced_clicked();

    void on_sliderNThreads_valueChanged(int value);

    void updateModelsTableSize();
    void on_cbShowFreqs_toggled(bool checked);
    void on_cbShowRates_toggled(bool checked);
    void on_cbShowHetParams_toggled(bool checked);
    void on_cbShowSelection_toggled(bool checked);

    void on_radDatatypeDna_clicked();
    void on_radDatatypeProt_clicked();
    void on_cbShowMatrices_clicked();
    void on_radSchemesUser_clicked();
    void on_modelsListView_itemClicked(QListWidgetItem *item);

    void on_sliderNCat_valueChanged(int value);

public slots:
    void cancel_jobs();

private:
    Ui::jModelTest *ui;
    Q_DebugStream *redirect;

    void evaluate_models(ModelTest & mtest);

    void autoSelectSchemes(const int schemes[], int n);
    void updateGUI();
    void resetSettings();

    void fill_results(QTableView * result_table,
                      ModelSelection &model_selection,
                      QLabel *imp_inv = 0, QLabel *imp_gamma = 0,
                      QLabel *imp_gammainv = 0, QLabel *imp_freqs = 0);
    void clear_table(QTableView * result_table);

    bool check_state(current_state st);
    void set_state(current_state st);
    void unset_state(current_state st);
    void clear_state() { state = STATE_INITIAL; }

    size_t compute_size(int n_cats, int n_threads);

    std::string msa_filename;
    std::string utree_filename;
    std::string partitions_filename;
    std::string partitions_basename;

    tree_type_t start_tree;

    mt_size_t n_seqs;
    mt_size_t seq_len;

    ModelTest *mtest;
    mt_options_t opts;
    std::vector<Model *> c_models;
    current_state state;
    QStandardItemModel * models_table_items;

    void set_tablemodels_header();
    void set_tablemodels_visibility();
    void set_tablemodels_line(int line_id, Model * model);

    partitioning_scheme_t * scheme;

    bool tabs_multi_shown;
};

}
#endif // JMODELTEST_H
