/*
  Copyright (C) 2016 Diego Darriba

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU Affero General Public License as
  published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Affero General Public License for more details.

  You should have received a copy of the GNU Affero General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  Contact: Diego Darriba <Diego.Darriba@h-its.org>,
  Heidelberg Institute for Theoretical Studies,
  Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

#ifndef XMODELTEST_H
#define XMODELTEST_H

#include "../gui/xthreadopt.h"
#include "../gui/qdebugstream.h"
#include "../gui/mydebugstream.h"
#include "../gui/xutils.h"

/* dialogs */
#include "../gui/datainfodialog.h"
#include "../gui/modelsdialog.h"
#include "../gui/resultsdialog.h"

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

#if(0)
Q_DECLARE_METATYPE( partition_id_t )
Q_DECLARE_METATYPE( mt_size_t )
#endif

namespace Ui {
class xmodeltest;
}

class xmodeltest : public QMainWindow
{
    Q_OBJECT

public:
    explicit xmodeltest(QWidget *parent = 0);
    ~xmodeltest();

public slots:
    void set_text(char *message );
    void optimized_single_model(modeltest::Model * model, unsigned int n_models);
    void optimized_partition( partition_id_t part_id );
    void optimization_done( );
    void optimization_interrupted( );

private slots:
    void on_mnu_open_msa_triggered();
    void on_tool_open_msa_clicked();

    void on_mnu_open_tree_triggered();
    void on_tool_open_tree_clicked();

    void on_mnu_open_parts_triggered();
    void on_tool_open_parts_clicked();

    void on_mnu_toggle_settings_triggered(bool checked);
    void on_tool_settings_toggled(bool checked);

    void on_mnu_run_triggered();
    void on_tool_run_clicked();
    void run_modelselection();

    void on_mnu_results_triggered();
    void on_tool_results_clicked();

    void on_mnu_models_triggered();
    void on_tool_models_clicked();

    void on_mnu_reset_triggered();
    void on_tool_reset_clicked();

    void on_mnu_view_datainfo_triggered();

    /** SETTINGS **/
    void autoSelectSchemes(const int schemes[], int n);
    void on_radSchemes3_clicked();
    void on_radSchemes5_clicked();
    void on_radSchemes7_clicked();
    void on_radSchemes11_clicked();
    void on_radSchemes203_clicked();

    void on_radSetModelTest_clicked();
    void on_radSetMrbayes_clicked();
    void on_radSetRaxml_clicked();
    void on_radSetPhyml_clicked();
    void on_radSetModelTest_toggled(bool checked);
    void on_radSetMrbayes_toggled(bool checked);
    void on_radSetRaxml_toggled(bool checked);
    void on_radSetPhyml_toggled(bool checked);

    void on_cbEqualFreq_toggled(bool checked);
    void on_cbMlFreq_toggled(bool checked);
    void on_cbNoRateVarModels_toggled(bool checked);
    void on_cbIModels_toggled(bool checked);
    void on_cbGModels_toggled(bool checked);
    void on_cbIGModels_toggled(bool checked);
    void on_cbAdvanced_clicked();
    void on_sliderNThreads_valueChanged(int value);
    void on_radDatatypeDna_clicked();
    void on_radDatatypeProt_clicked();
    void on_cbShowMatrices_clicked();
    void on_radSchemesUser_clicked();
    void on_modelsListView_itemClicked(QListWidgetItem *item);

    void on_radTopoU_clicked();
    void on_radTopoFixedMp_clicked();
    void on_radTopoFixedGtr_clicked();
    void on_radTopoFixedJc_clicked();
    void on_radTopoML_clicked();

    void on_sliderNCat_valueChanged(int value);
    void on_sliderNCat_sliderMoved(int position);

private:
    void toggle_settings( bool value );
    void action_open_msa( void );
    void action_open_tree( void );
    void action_open_parts( void );
    void action_run( void );
    void action_viewresults( void );
    void action_viewmodels( void );
    void action_reset( void );
    void action_view_datainfo( void );

    void update_gui( void );
    void reset_xmt( void );

    void set_substitution_schemes(mt_index_t n_schemes);


    size_t compute_size(int n_cats, int n_threads);

    Ui::xmodeltest *ui;
    std::string msa_filename;
    std::string utree_filename;
    std::string parts_filename;
    std::string asc_filename;
    msa_format_t msa_format;

    mt_size_t n_taxa;
    mt_size_t n_sites;
    unsigned long status;

    //Q_DebugStream *redirect;
    MyDebugStream *redirect;
    xThreadOpt * mythread;

    //std::vector<modeltest::Model *> c_models;
    partitioning_scheme_t * scheme;
    tree_type_t start_tree;
    asc_bias_t asc_bias;
    mt_options_t opts;

    time_t ini_t;

    /* other dialogs */
    DataInfoDialog * datainfo_dialog;
    ResultsDialog * results_dialog;
    ModelsDialog * models_dialog;
};

#endif // XMODELTEST_H
