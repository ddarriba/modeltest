#ifndef JMODELTEST_H
#define JMODELTEST_H

#include "modeltest.h"

#include <QMainWindow>
#include <QtGui/QStandardItemModel>
#include <QtGui/QTableView>
#include <string>
#include <vector>

#define UNUSED(expr) do { (void)(expr); } while (0)

#define TAB_CONSOLE 0
#define TAB_CONFIG  1
#define TAB_RUN     2
#define TAB_RESULTS 3

enum current_state {
    STATE_INITIAL,
    STATE_ALIGNMENT_LOADED,
    STATE_TREE_LOADED,
    STATE_MODELS_OPTIMIZING,
    STATE_MODELS_OPTIMIZED,
    STATE_DONE
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

    void on_radSchemes3_clicked();
    void on_radSchemes5_clicked();
    void on_radSchemes7_clicked();
    void on_radSchemes11_clicked();
    void on_radSchemes203_clicked();
    //void on_listMatrices_clicked();

    void on_radTopoU_clicked();

    void on_radSetModelTest_clicked();
    void on_radSetMrbayes_clicked();
    void on_radSetRaxml_clicked();
    void on_radSetPhyml_clicked();
    void on_radSetModelTest_toggled(bool checked);
    void on_radSetMrbayes_toggled(bool checked);
    void on_radSetRaxml_toggled(bool checked);
    void on_radSetPhyml_toggled(bool checked);

    void on_listMatrices_itemSelectionChanged();

    void on_sliderNCat_sliderMoved(int position);

    /* MENU */
    void on_actionReset_triggered();
    void on_menuFileLoad_triggered();
    void on_menuTreeLoad_triggered();

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

    void on_btnLoadTree_clicked();

public slots:
    void cancel_jobs();

private:
    Ui::jModelTest *ui;

    void evaluate_models(ModelTest & mtest);

    void autoSelectSchemes(const int schemes[], int n);
    void updateGUI();
    void resetSettings();

    void fill_results(QTableView * result_table, ModelSelection &model_selection);
    void clear_table(QTableView * result_table);

    size_t compute_size(int n_cats, int n_threads);

    std::string msa_filename;
    std::string msa_basename;
    std::string utree_filename;
    std::string utree_basename;

    tree_type start_tree;

    int n_seqs;
    int seq_len;

    ModelTest *mtest;
    std::vector<Model *> c_models;
    current_state state;
    QStandardItemModel * models_table_items;
};

}
#endif // JMODELTEST_H
