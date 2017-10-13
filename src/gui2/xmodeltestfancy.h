#ifndef XMODELTESTFANCY_H
#define XMODELTESTFANCY_H

#include "../global_defs.h"
#include "../model_selection.h"

#include "../gui/qdebugstream.h"
#include "../gui/mydebugstream.h"
#include "../gui/xthreadopt.h"

#include <QMainWindow>
#ifdef QT_WIDGETS_LIB
#include <QtWidgets>
#else
#include <QtGui/QListWidgetItem>
#include <QtGui/QStandardItemModel>
#include <QtGui/QTableView>
#include <QtGui/QLabel>
#endif

#define st_active         (1<<0)
#define st_msa_loaded     (1<<1)
#define st_tree_loaded    (1<<2)
#define st_parts_loaded   (1<<3)
#define st_optimizing     (1<<5)
#define st_optimized      (1<<6)

//#ifdef FANCY_GUI
Q_DECLARE_METATYPE( partition_id_t )
Q_DECLARE_METATYPE( mt_size_t )
//#endif

namespace Ui {
class XModelTestFancy;
}

class XModelTestFancy : public QMainWindow
{
    Q_OBJECT

public slots:
    void set_text(char *message );
    void optimized_single_model(modeltest::Model * model, unsigned int n_models);
    void optimized_partition( partition_id_t part_id );
    void optimization_done( );
    void optimization_interrupted( );


public:
    explicit XModelTestFancy(QWidget *parent = 0);
    ~XModelTestFancy();

private slots:
    void on_slider_nthreads_valueChanged(int value);
    void on_cmb_tree_currentIndexChanged(int index);
    void on_actionConsole_triggered();
    void on_actionData_triggered();
    void on_actionSettings_triggered();
    void on_actionResults_triggered();
    void on_btn_loadmsa_clicked();
    void on_radDatatypeDna_clicked();
    void on_radDatatypeProt_clicked();
    void on_radSchemes11_clicked();
    void on_radSchemes203_clicked();
    void on_radSchemesUser_clicked();
    void on_radSchemes7_clicked();
    void on_radSchemes5_clicked();
    void on_radSchemes3_clicked();
    void on_actionLoad_MSA_triggered();

    void on_btn_loadtree_clicked();

    void on_actionLoad_Tree_triggered();

    void on_btn_run_clicked();

    void on_btn_reset_clicked();

    void on_action_quit_triggered();

    void on_btn_report_clicked();

    void on_actionRun_triggered();

    void on_actionSave_Report_triggered();

    void on_actionReset_triggered();

    void on_actionAbout_triggered();

    void on_actionProject_Site_triggered();

    void on_actionIndex_triggered();

    void on_actionLoad_Partitions_triggered();

    void on_btn_loadparts_clicked();

    void on_cmb_results_partition_currentIndexChanged(int index);

    void on_cbNoRateVarModels_toggled(bool checked);
    void on_cbGModels_toggled(bool checked);
    void on_cbIGModels_toggled(bool checked);
    void on_cbIModels_toggled(bool checked);
    void on_cbEqualFreq_toggled(bool checked);
    void on_cbMlFreq_toggled(bool checked);

    void on_modelsListView_itemClicked(QListWidgetItem *item);

    void on_sliderNCat_sliderMoved(int position);
    void on_sliderNCat_valueChanged(int value);

    void on_radSetMrbayes_toggled(bool checked);
    void on_radSetRaxml_toggled(bool checked);
    void on_radSetPhyml_toggled(bool checked);
    void on_radSetModelTest_toggled(bool checked);

    void on_modelsListView_itemPressed(QListWidgetItem *item);

private:
    void update_gui( void );
    void set_active_tab(QString text);
    void action_open_msa( void );
    size_t compute_size(int n_cats, int n_threads);
    void autoSelectSchemes(const int schemes[], int n);
    void set_substitution_schemes(mt_index_t n_schemes);
    void reset_xmt( void );
    bool run_modelselection( void );

    Ui::XModelTestFancy *ui;

    std::string msa_filename;
    std::string utree_filename;
    std::string parts_filename;
    std::string asc_filename;
    msa_format_t msa_format;

    mt_size_t n_taxa;
    mt_size_t n_sites;
    mt_size_t n_patterns;

    unsigned int status;
    unsigned int tree_type;
    unsigned int n_threads;

    partitioning_scheme_t * scheme;
    tree_type_t start_tree;
    asc_bias_t asc_bias;
    mt_options_t opts;

    time_t ini_t;

    MyDebugStream *redirect;
    xThreadOpt * mythread;

    std::vector<std::map<modeltest::ic_type, modeltest::ModelSelection *>> model_selection;
};

#endif // XMODELTESTFANCY_H
