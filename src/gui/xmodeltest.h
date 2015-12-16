#ifndef XMODELTEST_H
#define XMODELTEST_H

#include "gui/qdebugstream.h"

#include <QMainWindow>

enum msg_level_id {
    msg_lvl_error,
    msg_lvl_alert,
    msg_lvl_notify,
    msg_lvl_info
};

#define st_active       (1<<0)
#define st_msa_loaded   (1<<1)
#define st_tree_loaded  (1<<2)
#define st_parts_loaded (1<<3)
#define st_optimized    (1<<4)

namespace Ui {
class xmodeltest;
}

class xmodeltest : public QMainWindow
{
    Q_OBJECT

public:
    explicit xmodeltest(QWidget *parent = 0);
    ~xmodeltest();

private slots:
    void on_act_open_msa_triggered();

    void on_mnu_open_msa_triggered();

    void on_tool_open_msa_triggered(QAction *arg1);

    void on_tool_open_msa_released();

private:
    Ui::xmodeltest *ui;
    std::string msa_filename;
    unsigned long status;

    Q_DebugStream *redirect;

    void update_gui( void );
};

#endif // XMODELTEST_H
