#ifndef MODELSDIALOG_H
#define MODELSDIALOG_H

#include <QWidget>
#include <QStandardItemModel>

#include "partitioning_scheme.h"

namespace Ui {
class ModelsDialog;
}

class ModelsDialog : public QWidget
{
    Q_OBJECT

public:
    explicit ModelsDialog(modeltest::PartitioningScheme &scheme,
                          QWidget *parent = 0);
    ~ModelsDialog();

private slots:
    void on_cmb_partition_currentIndexChanged(int index);

    void on_action_close_triggered();

    void on_toolButton_clicked();

private:
    void fill_header( data_type type = dt_dna );
    void fill_data( modeltest::Partition &partition );

    Ui::ModelsDialog *ui;
    modeltest::PartitioningScheme &scheme;
    QStandardItemModel * models_table_items;
};

#endif // MODELSDIALOG_H
