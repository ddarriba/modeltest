#ifndef DATAINFODIALOG_H
#define DATAINFODIALOG_H

#include "global_defs.h"

#include <QWidget>

namespace Ui {
class DataInfoDialog;
}

class DataInfoDialog : public QWidget
{
    Q_OBJECT

public:
    explicit DataInfoDialog(const std::string &msa_filename,
                            mt_size_t ntaxa,
                            mt_size_t seqlen,
                            const std::string &tree_filename,
                            double treelen,
                            partitioning_scheme_t &scheme,
                            const std::string &parts_filename,
                            QWidget *parent = 0);
    ~DataInfoDialog();

private:
    Ui::DataInfoDialog *ui;
};

#endif // DATAINFODIALOG_H
