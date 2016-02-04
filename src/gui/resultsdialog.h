#ifndef RESULTSDIALOG_H
#define RESULTSDIALOG_H

#include "model_selection.h"
#include "partitioning_scheme.h"

#include <QWidget>
#include <vector>
#include <map>

namespace Ui {
class ResultsDialog;
}

class ResultsDialog : public QWidget
{
    Q_OBJECT

public:
    explicit ResultsDialog(modeltest::PartitioningScheme &scheme,
                           std::string const& base_name,
                           QWidget *parent = 0);
    ~ResultsDialog();

private slots:
    void on_tool_results_export_clicked();

private:
    Ui::ResultsDialog *ui;
    std::string const& base_name;
    std::vector<std::map<modeltest::ic_type, modeltest::ModelSelection *>> model_selection;
};

#endif // RESULTSDIALOG_H
