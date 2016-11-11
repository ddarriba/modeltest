#ifndef XMODELTESTFANCY_H
#define XMODELTESTFANCY_H

#include <QMainWindow>
#include "global_defs.h"

#include "gui/qdebugstream.h"
#include "gui/mydebugstream.h"
#include "gui/xthreadopt.h"

#define st_active         (1<<0)
#define st_msa_loaded     (1<<1)
#define st_tree_loaded    (1<<2)
#define st_parts_loaded   (1<<3)
#define st_optimizing     (1<<5)
#define st_optimized      (1<<6)

namespace Ui {
class XModelTestFancy;
}

class XModelTestFancy : public QMainWindow
{
    Q_OBJECT

public slots:
    void set_text(char *message );

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

private:
    void update_gui( void );
    void set_active_tab(QString text);
    void action_open_msa( void );
    size_t compute_size(int n_cats, int n_threads);
    void autoSelectSchemes(const int schemes[], int n);
    void set_substitution_schemes(mt_index_t n_schemes);
    void reset_xmt( void );
    void run_modelselection( void );

    Ui::XModelTestFancy *ui;

    std::string msa_filename;
    std::string utree_filename;
    std::string parts_filename;
    std::string asc_filename;
    msa_format_t msa_format;

    mt_size_t n_taxa;
    mt_size_t n_sites;

    unsigned int status;
    unsigned int tree_type;
    unsigned int n_threads;

    partitioning_scheme_t * scheme;
    tree_type_t start_tree;
    asc_bias_t asc_bias;
    mt_options_t opts;

    MyDebugStream *redirect;
    xThreadOpt * mythread;
};

#endif // XMODELTESTFANCY_H
