#include "xmodeltestfancy.h"
#include "ui_xmodeltestfancy.h"

#include "service/modeltestservice.h"
#include "meta.h"

#ifdef QT_WIDGETS_LIB
#include <QtWidgets>
#else
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtConcurrentRun>
#endif

#define TREE_ML     0
#define TREE_MP     1
#define TREE_ML_JC  2
#define TREE_ML_GTR 3
#define TREE_USER   4

using namespace std;

XModelTestFancy::XModelTestFancy(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::XModelTestFancy)
{
  ui->setupUi(this);

  ui->slider_nthreads->setRange(1, QThread::idealThreadCount());
  ui->slider_nthreads->setValue(modeltest::Utils::count_physical_cores());

  status = st_active;

  update_gui();
}

XModelTestFancy::~XModelTestFancy()
{
  delete ui;
}

typedef enum { LABEL_FAIL, LABEL_UNDEF, LABEL_OK } oklabel_t;

static void set_oklabel(QLabel * label, oklabel_t ok)
{
  switch(ok)
  {
  case LABEL_FAIL:
    label->setText(QString("NO"));
    label->setStyleSheet("color: #aa0000;");
    break;
  case LABEL_UNDEF:
    label->setText(QString("-"));
    label->setStyleSheet("");
    break;
  case LABEL_OK:
    label->setText(QString("OK"));
    label->setStyleSheet("color: #00aa00;");
    break;
  }
}

void XModelTestFancy::update_gui()
{
  /* get values */
  tree_type = ui->cmb_tree->currentIndex();
  n_threads = ui->slider_nthreads->value();

  /* apply changes */
  ui->lbl_nthreads->setText(QString::number(n_threads));
  set_oklabel(ui->lbl_okmsa, (status & st_msa_loaded)?LABEL_OK:LABEL_FAIL);
  if (status & st_tree_loaded)
    set_oklabel(ui->lbl_oktree, LABEL_OK);
  else
  {
    if (tree_type == TREE_USER)
      set_oklabel(ui->lbl_oktree, LABEL_FAIL);
    else
      set_oklabel(ui->lbl_oktree, LABEL_UNDEF);
  }
  set_oklabel(ui->lbl_okparts, (status & st_parts_loaded)?LABEL_OK:LABEL_UNDEF);
}

void XModelTestFancy::on_slider_nthreads_valueChanged(int value)
{
  ui->lbl_nthreads->setText(QString::number(value));
}

void XModelTestFancy::on_cmb_tree_currentIndexChanged(int index)
{
    status |= st_msa_loaded;
    update_gui();
}
