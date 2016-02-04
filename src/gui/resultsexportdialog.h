#ifndef RESULTSEXPORTDIALOG_H
#define RESULTSEXPORTDIALOG_H

#include <QDialog>

namespace Ui {
class ResultsExportDialog;
}

class ResultsExportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResultsExportDialog(QWidget *parent = 0);
    ~ResultsExportDialog();

private:
    Ui::ResultsExportDialog *ui;
};

#endif // RESULTSEXPORTDIALOG_H
