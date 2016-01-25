#ifndef RESULTSDIALOG_H
#define RESULTSDIALOG_H

#include "model_selection.h"

#include <QWidget>

namespace Ui {
class ResultsDialog;
}

class ResultsDialog : public QWidget
{
    Q_OBJECT

public:
    explicit ResultsDialog(modeltest::ModelSelection & aic_selection,
                           modeltest::ModelSelection & aicc_selection,
                           modeltest::ModelSelection & bic_selection,
                           modeltest::ModelSelection & dt_selection,
                           QWidget *parent = 0);
    ~ResultsDialog();

private:
    Ui::ResultsDialog *ui;
};

#endif // RESULTSDIALOG_H
