#include "datainfodialog.h"
#include "ui_datainfodialog.h"
#include "gui/xutils.h"

DataInfoDialog::DataInfoDialog(std::string const& msa_filename,
                               mt_size_t n_taxa,
                               mt_size_t n_sites,
                               std::string const& tree_filename,
                               double tree_len,
                               partitioning_scheme_t &scheme,
                               std::string const&  parts_filename,
                               QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataInfoDialog)
{
    ui->setupUi(this);

    char line[500];
    sprintf(line, "MSA filename           %s", msa_filename.c_str());
    ui->text->append(line);
    ui->text->append("");
    sprintf(line, "Number of sequences        %d", n_taxa);
    ui->text->append(line);
    sprintf(line, "Sequence length            %d", n_sites);
    ui->text->append(line);
    ui->text->append("");

    if (tree_filename.length())
    {
        sprintf(line, "Tree filename          %s", tree_filename.c_str());
        ui->text->append(line);
        ui->text->append("");
        sprintf(line, "Tree length                %.4f", tree_len);
        ui->text->append(line);
        ui->text->append("");
    }
    else
    {
        sprintf(line, "No user tree defined");
        ui->text->append(line);
    }

    if (parts_filename.length())
    {
        sprintf(line, "Partitions filename    %s", parts_filename.c_str());
        ui->text->append(line);
        ui->text->append("");
        mt_size_t n_dna_parts = 0, n_aa_parts = 0;
        for (partition_t partition : scheme)
            if (partition.datatype == dt_dna)
                n_dna_parts++;
            else
                n_aa_parts++;
        sprintf(line, "Number of partitions       %ld (DNA: %d ; Protein %d)", scheme.size(), n_dna_parts, n_aa_parts);
        ui->text->append(line);
    }
    else
    {
        sprintf(line, "No partitioning defined");
        ui->text->append(line);
    }
}

DataInfoDialog::~DataInfoDialog()
{
    delete ui;
}
