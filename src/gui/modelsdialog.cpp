#include "modelsdialog.h"
#include "ui_modelsdialog.h"

ModelsDialog::ModelsDialog(modeltest::PartitioningScheme &scheme,
                           QWidget *parent) :
    QWidget(parent), ui(new Ui::ModelsDialog),
    scheme(scheme)
{
    ui->setupUi(this);

    ui->cmb_partition->setVisible(scheme.get_size() > 1);

    models_table_items = new QStandardItemModel(0,13);
    ui->table_models->setModel(models_table_items);

    for (mt_index_t i=0; i<scheme.get_size(); ++i)
        ui->cmb_partition->addItem(scheme.get_partition(i).get_name().c_str());

   // fill_data(scheme.get_partition({0}));
}

ModelsDialog::~ModelsDialog()
{
    if (models_table_items)
        delete models_table_items;
    delete ui;
}

void ModelsDialog::fill_data( modeltest::Partition &partition )
{
    models_table_items->clear();
    fill_header(partition.get_datatype());
    for (size_t i=0; i<partition.get_number_of_models(); i++)
    {
        int cur_column = 0;
        modeltest::Model * model = partition.get_model(i);
        models_table_items->setItem(i, cur_column++, new QStandardItem(QString(model->get_name().c_str())));
        models_table_items->setItem(i, cur_column++, new QStandardItem(QString::number(model->get_n_free_variables())));
        models_table_items->setItem(i, cur_column++, new QStandardItem(QString::number(model->get_lnl(), 'f', 4)));
        switch (partition.get_datatype())
        {
        case dt_dna:
        {
            modeltest::DnaModel * dna_model = static_cast<modeltest::DnaModel *>(model);
            const double * subst_rates = dna_model->get_subst_rates();
            const double * frequencies = dna_model->get_frequencies();
            models_table_items->setItem(i, cur_column++, new QStandardItem(QString::number(subst_rates[0], 'f', 2)));
            models_table_items->setItem(i, cur_column++, new QStandardItem(QString::number(subst_rates[1], 'f', 2)));
            models_table_items->setItem(i, cur_column++, new QStandardItem(QString::number(subst_rates[2], 'f', 2)));
            models_table_items->setItem(i, cur_column++, new QStandardItem(QString::number(subst_rates[3], 'f', 2)));
            models_table_items->setItem(i, cur_column++, new QStandardItem(QString::number(subst_rates[4], 'f', 2)));
            models_table_items->setItem(i, cur_column++, new QStandardItem(QString::number(frequencies[0], 'f', 2)));
            models_table_items->setItem(i, cur_column++, new QStandardItem(QString::number(frequencies[1], 'f', 2)));
            models_table_items->setItem(i, cur_column++, new QStandardItem(QString::number(frequencies[2], 'f', 2)));
            models_table_items->setItem(i, cur_column++, new QStandardItem(QString::number(frequencies[3], 'f', 2)));
        }
            break;
        case dt_protein:
            break;
        default:
            assert(0);
        }
        models_table_items->setItem(i, cur_column++, new QStandardItem(QString::number(model->get_alpha(), 'f', 4)));
        models_table_items->setItem(i, cur_column++, new QStandardItem(QString::number(model->get_prop_inv(), 'f', 4)));
    }
}

void ModelsDialog::fill_header( data_type_t type )
{
    if (models_table_items)
        models_table_items->clear();
    else
    {
        models_table_items = new QStandardItemModel(0,14);
        ui->table_models->setModel(models_table_items);
    }

    int cur_column = 0;
    models_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("Model")));
    models_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("K")));
    models_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("lnLK")));
    switch (type)
    {
    case dt_dna:
        models_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("Rac")));
        models_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("Rag")));
        models_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("Rat")));
        models_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("Rcg")));
        models_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("Rct")));
        models_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("fa")));
        models_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("fc")));
        models_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("fg")));
        models_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("ft")));
        break;
    case dt_protein:
        break;
    default:
        assert(0);
    }
    models_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("alpha")));
    models_table_items->setHorizontalHeaderItem(cur_column++, new QStandardItem(QString("p-inv")));

    ui->table_models->horizontalHeader()->setResizeMode(
        0, QHeaderView::ResizeToContents);
    for (int c = 1; c < ui->table_models->horizontalHeader()->count(); ++c)
        ui->table_models->horizontalHeader()->setResizeMode(
            c, QHeaderView::Stretch);
}

void ModelsDialog::on_cmb_partition_currentIndexChanged(int index)
{
    fill_data(scheme.get_partition((mt_index_t)index));
}

void ModelsDialog::on_action_close_triggered()
{
    close();
}

void ModelsDialog::on_toolButton_clicked()
{
    hide();
}
