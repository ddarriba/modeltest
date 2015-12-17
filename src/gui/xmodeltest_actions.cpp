#include "ui_xmodeltest.h"
#include "xmodeltest.h"

#include "../utils.h"
#include "../modeltest.h"

#include <QtGui/QFileDialog>

void xmodeltest::toggle_settings( bool value )
{
    ui->act_toggle_settings->setChecked(value);
    ui->tool_settings->setChecked(value);
    ui->mnu_toggle_settings->setChecked(value);
    update_gui();
}

void xmodeltest::on_act_open_msa_triggered() {action_open_msa();}
void xmodeltest::on_mnu_open_msa_triggered() {action_open_msa();}
void xmodeltest::on_tool_open_msa_clicked()  {action_open_msa();}

void xmodeltest::on_act_open_tree_triggered() {action_open_tree();}
void xmodeltest::on_mnu_open_tree_triggered() {action_open_tree();}
void xmodeltest::on_tool_open_tree_clicked()  {action_open_tree();}

void xmodeltest::on_act_toggle_settings_triggered()
{
    toggle_settings(ui->act_toggle_settings->isChecked());
}

void xmodeltest::on_mnu_toggle_settings_triggered(bool checked) {
    toggle_settings(checked);
}

void xmodeltest::on_tool_settings_toggled(bool checked)
{
    toggle_settings(checked);
}

void xmodeltest::on_act_reset_triggered() {action_reset();}
void xmodeltest::on_mnu_reset_triggered() {action_reset();}
void xmodeltest::on_tool_reset_clicked()  {action_reset();}
