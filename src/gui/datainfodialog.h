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
    explicit DataInfoDialog(std::string const& msa_filename,
                            mt_size_t n_taxa,
                            mt_size_t n_sites,
                            std::string const& tree_filename,
                            double tree_len,
                            partitioning_scheme_t &scheme,
                            std::string const& parts_filename,
                            QWidget *parent = 0);
    ~DataInfoDialog();

private:
    Ui::DataInfoDialog *ui;
};

#endif // DATAINFODIALOG_H
