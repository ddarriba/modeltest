#ifndef RESULTSEXPORTDIALOG_H
#define RESULTSEXPORTDIALOG_H

#include "model_selection.h"

#include <map>
#include <vector>
#include <QDialog>

namespace Ui {
class ResultsExportDialog;
}

class ResultsExportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResultsExportDialog(
            std::vector<std::map<modeltest::ic_type, modeltest::ModelSelection *>> &model_selection,
            std::string const& default_filename,
            QWidget *parent = 0);
    ~ResultsExportDialog();

private slots:
    void on_tool_outfile_clicked();

    void on_box_buttons_accepted();

    void on_slider_nmodels_valueChanged(int value);

    void on_cb_tofile_stateChanged(int newstate);

private:
    Ui::ResultsExportDialog *ui;
    std::vector<std::map<modeltest::ic_type, modeltest::ModelSelection *>> &model_selection;

    void export_results(std::ostream  &out = std::cout);
};

#endif // RESULTSEXPORTDIALOG_H
