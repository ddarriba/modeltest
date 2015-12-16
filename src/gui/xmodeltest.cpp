#include "xmodeltest.h"
#include "ui_xmodeltest.h"

#include "../utils.h"
#include "../modeltest.h"

#include <QtGui/QFileDialog>

static void enable(QToolButton * button, bool new_stat, bool set = false)
{
    button->setEnabled(new_stat);
    if (new_stat)
    {
        if (set)
            button->setStyleSheet("color: #000;\nbackground-color: #66c;");
        else
            button->setStyleSheet("color: #000;\nbackground-color: #6c6;");
    }
    else
    {
        button->setStyleSheet("");
    }
}

xmodeltest::xmodeltest(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::xmodeltest)
{
    ui->setupUi(this);
    status = st_active;
    enable(ui->tool_help, true);
    update_gui();

    /* Redirect Console output to QTextEdit */
    redirect = new Q_DebugStream(std::cout, ui->consoleRun);
    modeltest::Utils::print_header();
}

xmodeltest::~xmodeltest()
{
    delete redirect;
    delete ui;
}

void xmodeltest::update_gui( void )
{
    ui->act_open_tree->setEnabled(status & st_msa_loaded);
    ui->mnu_open_tree->setEnabled(status & st_msa_loaded);
    ui->act_open_parts->setEnabled(status & st_msa_loaded);
    ui->mnu_open_parts->setEnabled(status & st_msa_loaded);
    enable(ui->tool_open_msa, status & st_active, status & st_msa_loaded);
    enable(ui->tool_reset, status & st_active);
    enable(ui->tool_open_tree, status & st_msa_loaded, status & st_tree_loaded);
    enable(ui->tool_open_parts, status & st_msa_loaded, status & st_parts_loaded);
    enable(ui->tool_run_settings, status & st_msa_loaded, status & st_optimized);
    enable(ui->tool_results, status & st_optimized);
}

static QString to_qstring(const char * msg, msg_level_id level)
{
    QString line = msg;
    QString endHtml = "</font>";

    switch(level)
    {
        case msg_lvl_error: line = "<font color=\"Red\">" + line + "</font>"; break;
        case msg_lvl_alert: line = "<font color=\"DeepPink\">" + line + "</font>"; break;
        case msg_lvl_notify: line = "<font color=\"DarkGreen\">" + line + "</font>"; break;
        case msg_lvl_info: line  = "<font color=\"Black\">" + line + "</font>"; break;
        default: break;
    }

    line = line + endHtml;
    return line;
}

void xmodeltest::on_act_open_msa_triggered()
{
    QString filters = tr("Multiple Sequence Alignment(*.phy *.nex *.fas);; All files(*)");
    QString file_name = QFileDialog::getOpenFileName(this,
                                                    tr("Open File"),
                                                    "",
                                                    filters);

    const std::string loaded_file = file_name.toStdString();
    mt_size_t n_seqs, seq_len;

    if ( loaded_file.compare(""))
    {
        msa_filename = loaded_file;
        if (modeltest::ModelTest::test_msa(msa_filename,
                                 &n_seqs,
                                 &seq_len))
        {
            char text[250];

            ui->consoleMeta->append(to_qstring("Loaded alignment %1", msg_lvl_notify).arg(msa_filename.c_str()));
            sprintf(text, "Num.Sequences:   %d", n_seqs);
            ui->consoleMeta->append(to_qstring(text, msg_lvl_info));
            sprintf(text, "Sequence Length: %d", seq_len);
            ui->consoleMeta->append(to_qstring(text, msg_lvl_info));
            status |= st_msa_loaded;
        }
        else
        {
            ui->consoleMeta->append(to_qstring("Error: Cannot load alignment %1", msg_lvl_error).arg(msa_filename.c_str()));
            msa_filename   = "";
            status &= ~st_msa_loaded;
        }
    }
    update_gui();
}

void xmodeltest::on_mnu_open_msa_triggered()
{
    on_act_open_msa_triggered();
}

void xmodeltest::on_tool_open_msa_triggered(QAction *arg1)
{
    char text[250];
    sprintf(text, "TRIGGER\n");
    ui->consoleMeta->append(to_qstring(text, msg_lvl_info));
    on_act_open_msa_triggered();
}

void xmodeltest::on_tool_open_msa_released()
{
    on_act_open_msa_triggered();
}
