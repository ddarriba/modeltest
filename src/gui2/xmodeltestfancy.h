#ifndef XMODELTESTFANCY_H
#define XMODELTESTFANCY_H

#include <QMainWindow>

#define st_active         (1<<0)
#define st_msa_loaded     (1<<1)
#define st_tree_loaded    (1<<2)
#define st_parts_loaded   (1<<3)
#define st_optimizing     (1<<5)
#define st_optimized      (1<<6)

namespace Ui {
class XModelTestFancy;
}

class XModelTestFancy : public QMainWindow
{
    Q_OBJECT

public:
    explicit XModelTestFancy(QWidget *parent = 0);
    ~XModelTestFancy();

private slots:
    void on_slider_nthreads_valueChanged(int value);

    void on_cmb_tree_currentIndexChanged(int index);

private:
    void update_gui( void );

    Ui::XModelTestFancy *ui;
    unsigned int status;
    unsigned int tree_type;
    unsigned int n_threads;
};

#endif // XMODELTESTFANCY_H
