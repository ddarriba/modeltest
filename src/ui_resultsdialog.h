/********************************************************************************
** Form generated from reading UI file 'resultsdialog.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RESULTSDIALOG_H
#define UI_RESULTSDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSpacerItem>
#include <QtGui/QTabWidget>
#include <QtGui/QTableView>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ResultsDialog
{
public:
    QVBoxLayout *verticalLayout;
    QFrame *frame_results_inner;
    QVBoxLayout *verticalLayout_14;
    QFrame *frame_results_header;
    QHBoxLayout *horizontalLayout_6;
    QComboBox *cmb_results_partition;
    QSpacerItem *horizontalSpacer;
    QLabel *label;
    QSpacerItem *spacer_results_header;
    QTabWidget *results_content;
    QWidget *tab_results_bic;
    QVBoxLayout *verticalLayout_8;
    QTableView *table_results_bic;
    QFrame *frame_imp_bic;
    QGridLayout *gridLayout_5;
    QLabel *lbl_imp_bic;
    QLabel *lbl_imp_gamma_bic;
    QSpacerItem *spacer_imp_bic;
    QLabel *lbl_imp_freqs_bic;
    QLabel *txt_imp_freqs_bic;
    QLabel *txt_imp_gamma_bic;
    QLabel *lbl_imp_inv_bic;
    QLabel *txt_imp_inv_bic;
    QLabel *lbl_imp_invgamma_bic;
    QLabel *txt_imp_invgamma_bic;
    QWidget *tab_results_aic;
    QVBoxLayout *verticalLayout_10;
    QTableView *table_results_aic;
    QFrame *frame_imp_aic;
    QGridLayout *gridLayout_6;
    QLabel *lbl_imp_aic;
    QLabel *lbl_imp_gamma_aic;
    QSpacerItem *spacer_imp_aic;
    QLabel *lbl_imp_freqs_aic;
    QLabel *txt_imp_freqs_aic;
    QLabel *txt_imp_gamma_aic;
    QLabel *lbl_imp_inv_aic;
    QLabel *txt_imp_inv_aic;
    QLabel *lbl_imp_invgamma_aic;
    QLabel *txt_imp_invgamma_aic;
    QWidget *tab_results_aicc;
    QVBoxLayout *verticalLayout_11;
    QTableView *table_results_aicc;
    QFrame *frame_imp_aicc;
    QGridLayout *gridLayout_8;
    QLabel *lbl_imp_aicc;
    QLabel *lbl_imp_gamma_aicc;
    QSpacerItem *spacer_imp_aicc;
    QLabel *lbl_imp_freqs_aicc;
    QLabel *txt_imp_freqs_aicc;
    QLabel *txt_imp_gamma_aicc;
    QLabel *lbl_imp_inv_aicc;
    QLabel *txt_imp_inv_aicc;
    QLabel *lbl_imp_invgamma_aicc;
    QLabel *txt_imp_invgamma_aicc;
    QWidget *tab_results_dt;
    QVBoxLayout *verticalLayout_13;
    QTableView *table_results_dt;
    QFrame *frame_imp_dt;
    QGridLayout *gridLayout_9;
    QLabel *lbl_imp_dt;
    QLabel *lbl_imp_gamma_dt;
    QSpacerItem *spacer_imp_dt;
    QLabel *lbl_imp_freqs_dt;
    QLabel *txt_imp_freqs_dt;
    QLabel *txt_imp_gamma_dt;
    QLabel *lbl_imp_inv_dt;
    QLabel *txt_imp_inv_dt;
    QLabel *lbl_imp_invgamma_dt;
    QLabel *txt_imp_invgamma_dt;

    void setupUi(QWidget *ResultsDialog)
    {
        if (ResultsDialog->objectName().isEmpty())
            ResultsDialog->setObjectName(QString::fromUtf8("ResultsDialog"));
        ResultsDialog->resize(640, 480);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/IMG/IMG/results-icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        ResultsDialog->setWindowIcon(icon);
        ResultsDialog->setStyleSheet(QString::fromUtf8("background: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 #3c3b37, stop:0.55 #104BA9, stop:0.98 #000015, stop:1 #000015);\n"
"QToolTip { color: #000s; border: none; }"));
        verticalLayout = new QVBoxLayout(ResultsDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        frame_results_inner = new QFrame(ResultsDialog);
        frame_results_inner->setObjectName(QString::fromUtf8("frame_results_inner"));
        frame_results_inner->setFrameShape(QFrame::StyledPanel);
        frame_results_inner->setFrameShadow(QFrame::Raised);
        verticalLayout_14 = new QVBoxLayout(frame_results_inner);
        verticalLayout_14->setObjectName(QString::fromUtf8("verticalLayout_14"));
        verticalLayout_14->setContentsMargins(0, 0, 0, 0);
        frame_results_header = new QFrame(frame_results_inner);
        frame_results_header->setObjectName(QString::fromUtf8("frame_results_header"));
        frame_results_header->setStyleSheet(QString::fromUtf8("background: #104BA9;"));
        frame_results_header->setFrameShape(QFrame::NoFrame);
        frame_results_header->setFrameShadow(QFrame::Raised);
        horizontalLayout_6 = new QHBoxLayout(frame_results_header);
        horizontalLayout_6->setSpacing(5);
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        horizontalLayout_6->setContentsMargins(5, 1, 5, 1);
        cmb_results_partition = new QComboBox(frame_results_header);
        cmb_results_partition->setObjectName(QString::fromUtf8("cmb_results_partition"));
        cmb_results_partition->setMinimumSize(QSize(200, 0));
        cmb_results_partition->setMaximumSize(QSize(200, 16777215));
        QFont font;
        font.setFamily(QString::fromUtf8("Courier 10 Pitch"));
        font.setPointSize(10);
        cmb_results_partition->setFont(font);
        cmb_results_partition->setStyleSheet(QString::fromUtf8("border: 1px; background: #fff2db; border-radius:5px;"));

        horizontalLayout_6->addWidget(cmb_results_partition);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer);

        label = new QLabel(frame_results_header);
        label->setObjectName(QString::fromUtf8("label"));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Courier 10 Pitch"));
        font1.setPointSize(12);
        font1.setBold(true);
        font1.setWeight(75);
        label->setFont(font1);
        label->setStyleSheet(QString::fromUtf8("color: #fff;"));

        horizontalLayout_6->addWidget(label);

        spacer_results_header = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(spacer_results_header);


        verticalLayout_14->addWidget(frame_results_header);

        results_content = new QTabWidget(frame_results_inner);
        results_content->setObjectName(QString::fromUtf8("results_content"));
        results_content->setFont(font);
        results_content->setAutoFillBackground(false);
        results_content->setStyleSheet(QString::fromUtf8("background: #fff2db; border-radius: 5px;"));
        results_content->setTabShape(QTabWidget::Rounded);
        results_content->setDocumentMode(false);
        results_content->setTabsClosable(false);
        results_content->setMovable(false);
        tab_results_bic = new QWidget();
        tab_results_bic->setObjectName(QString::fromUtf8("tab_results_bic"));
        verticalLayout_8 = new QVBoxLayout(tab_results_bic);
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        table_results_bic = new QTableView(tab_results_bic);
        table_results_bic->setObjectName(QString::fromUtf8("table_results_bic"));
        table_results_bic->setFont(font);
        table_results_bic->setAlternatingRowColors(true);
        table_results_bic->setSortingEnabled(true);

        verticalLayout_8->addWidget(table_results_bic);

        frame_imp_bic = new QFrame(tab_results_bic);
        frame_imp_bic->setObjectName(QString::fromUtf8("frame_imp_bic"));
        frame_imp_bic->setMinimumSize(QSize(0, 20));
        frame_imp_bic->setFrameShape(QFrame::StyledPanel);
        frame_imp_bic->setFrameShadow(QFrame::Raised);
        gridLayout_5 = new QGridLayout(frame_imp_bic);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        gridLayout_5->setVerticalSpacing(6);
        lbl_imp_bic = new QLabel(frame_imp_bic);
        lbl_imp_bic->setObjectName(QString::fromUtf8("lbl_imp_bic"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(lbl_imp_bic->sizePolicy().hasHeightForWidth());
        lbl_imp_bic->setSizePolicy(sizePolicy);
        lbl_imp_bic->setMinimumSize(QSize(0, 0));
        QFont font2;
        font2.setBold(true);
        font2.setUnderline(false);
        font2.setWeight(75);
        lbl_imp_bic->setFont(font2);
        lbl_imp_bic->setLineWidth(1);

        gridLayout_5->addWidget(lbl_imp_bic, 0, 0, 1, 2, Qt::AlignHCenter);

        lbl_imp_gamma_bic = new QLabel(frame_imp_bic);
        lbl_imp_gamma_bic->setObjectName(QString::fromUtf8("lbl_imp_gamma_bic"));
        lbl_imp_gamma_bic->setMinimumSize(QSize(100, 0));

        gridLayout_5->addWidget(lbl_imp_gamma_bic, 3, 0, 1, 1);

        spacer_imp_bic = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_5->addItem(spacer_imp_bic, 3, 2, 1, 1);

        lbl_imp_freqs_bic = new QLabel(frame_imp_bic);
        lbl_imp_freqs_bic->setObjectName(QString::fromUtf8("lbl_imp_freqs_bic"));
        lbl_imp_freqs_bic->setMinimumSize(QSize(100, 0));

        gridLayout_5->addWidget(lbl_imp_freqs_bic, 4, 0, 1, 1);

        txt_imp_freqs_bic = new QLabel(frame_imp_bic);
        txt_imp_freqs_bic->setObjectName(QString::fromUtf8("txt_imp_freqs_bic"));
        txt_imp_freqs_bic->setMinimumSize(QSize(60, 0));

        gridLayout_5->addWidget(txt_imp_freqs_bic, 4, 1, 1, 1);

        txt_imp_gamma_bic = new QLabel(frame_imp_bic);
        txt_imp_gamma_bic->setObjectName(QString::fromUtf8("txt_imp_gamma_bic"));
        txt_imp_gamma_bic->setMinimumSize(QSize(60, 0));

        gridLayout_5->addWidget(txt_imp_gamma_bic, 3, 1, 1, 1);

        lbl_imp_inv_bic = new QLabel(frame_imp_bic);
        lbl_imp_inv_bic->setObjectName(QString::fromUtf8("lbl_imp_inv_bic"));
        lbl_imp_inv_bic->setMinimumSize(QSize(100, 0));

        gridLayout_5->addWidget(lbl_imp_inv_bic, 2, 0, 1, 1);

        txt_imp_inv_bic = new QLabel(frame_imp_bic);
        txt_imp_inv_bic->setObjectName(QString::fromUtf8("txt_imp_inv_bic"));
        txt_imp_inv_bic->setMinimumSize(QSize(60, 0));

        gridLayout_5->addWidget(txt_imp_inv_bic, 2, 1, 1, 1);

        lbl_imp_invgamma_bic = new QLabel(frame_imp_bic);
        lbl_imp_invgamma_bic->setObjectName(QString::fromUtf8("lbl_imp_invgamma_bic"));

        gridLayout_5->addWidget(lbl_imp_invgamma_bic, 1, 0, 1, 1);

        txt_imp_invgamma_bic = new QLabel(frame_imp_bic);
        txt_imp_invgamma_bic->setObjectName(QString::fromUtf8("txt_imp_invgamma_bic"));

        gridLayout_5->addWidget(txt_imp_invgamma_bic, 1, 1, 1, 1);


        verticalLayout_8->addWidget(frame_imp_bic);

        results_content->addTab(tab_results_bic, QString());
        tab_results_aic = new QWidget();
        tab_results_aic->setObjectName(QString::fromUtf8("tab_results_aic"));
        verticalLayout_10 = new QVBoxLayout(tab_results_aic);
        verticalLayout_10->setObjectName(QString::fromUtf8("verticalLayout_10"));
        table_results_aic = new QTableView(tab_results_aic);
        table_results_aic->setObjectName(QString::fromUtf8("table_results_aic"));
        table_results_aic->setFont(font);
        table_results_aic->setAlternatingRowColors(true);
        table_results_aic->setSortingEnabled(true);

        verticalLayout_10->addWidget(table_results_aic);

        frame_imp_aic = new QFrame(tab_results_aic);
        frame_imp_aic->setObjectName(QString::fromUtf8("frame_imp_aic"));
        frame_imp_aic->setMinimumSize(QSize(0, 20));
        frame_imp_aic->setFrameShape(QFrame::StyledPanel);
        frame_imp_aic->setFrameShadow(QFrame::Raised);
        gridLayout_6 = new QGridLayout(frame_imp_aic);
        gridLayout_6->setObjectName(QString::fromUtf8("gridLayout_6"));
        gridLayout_6->setVerticalSpacing(6);
        lbl_imp_aic = new QLabel(frame_imp_aic);
        lbl_imp_aic->setObjectName(QString::fromUtf8("lbl_imp_aic"));
        sizePolicy.setHeightForWidth(lbl_imp_aic->sizePolicy().hasHeightForWidth());
        lbl_imp_aic->setSizePolicy(sizePolicy);
        lbl_imp_aic->setMinimumSize(QSize(0, 0));
        lbl_imp_aic->setFont(font2);
        lbl_imp_aic->setLineWidth(1);

        gridLayout_6->addWidget(lbl_imp_aic, 0, 0, 1, 2, Qt::AlignHCenter);

        lbl_imp_gamma_aic = new QLabel(frame_imp_aic);
        lbl_imp_gamma_aic->setObjectName(QString::fromUtf8("lbl_imp_gamma_aic"));
        lbl_imp_gamma_aic->setMinimumSize(QSize(100, 0));

        gridLayout_6->addWidget(lbl_imp_gamma_aic, 3, 0, 1, 1);

        spacer_imp_aic = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_6->addItem(spacer_imp_aic, 3, 2, 1, 1);

        lbl_imp_freqs_aic = new QLabel(frame_imp_aic);
        lbl_imp_freqs_aic->setObjectName(QString::fromUtf8("lbl_imp_freqs_aic"));
        lbl_imp_freqs_aic->setMinimumSize(QSize(100, 0));

        gridLayout_6->addWidget(lbl_imp_freqs_aic, 4, 0, 1, 1);

        txt_imp_freqs_aic = new QLabel(frame_imp_aic);
        txt_imp_freqs_aic->setObjectName(QString::fromUtf8("txt_imp_freqs_aic"));
        txt_imp_freqs_aic->setMinimumSize(QSize(60, 0));

        gridLayout_6->addWidget(txt_imp_freqs_aic, 4, 1, 1, 1);

        txt_imp_gamma_aic = new QLabel(frame_imp_aic);
        txt_imp_gamma_aic->setObjectName(QString::fromUtf8("txt_imp_gamma_aic"));
        txt_imp_gamma_aic->setMinimumSize(QSize(60, 0));

        gridLayout_6->addWidget(txt_imp_gamma_aic, 3, 1, 1, 1);

        lbl_imp_inv_aic = new QLabel(frame_imp_aic);
        lbl_imp_inv_aic->setObjectName(QString::fromUtf8("lbl_imp_inv_aic"));
        lbl_imp_inv_aic->setMinimumSize(QSize(100, 0));

        gridLayout_6->addWidget(lbl_imp_inv_aic, 2, 0, 1, 1);

        txt_imp_inv_aic = new QLabel(frame_imp_aic);
        txt_imp_inv_aic->setObjectName(QString::fromUtf8("txt_imp_inv_aic"));
        txt_imp_inv_aic->setMinimumSize(QSize(60, 0));

        gridLayout_6->addWidget(txt_imp_inv_aic, 2, 1, 1, 1);

        lbl_imp_invgamma_aic = new QLabel(frame_imp_aic);
        lbl_imp_invgamma_aic->setObjectName(QString::fromUtf8("lbl_imp_invgamma_aic"));

        gridLayout_6->addWidget(lbl_imp_invgamma_aic, 1, 0, 1, 1);

        txt_imp_invgamma_aic = new QLabel(frame_imp_aic);
        txt_imp_invgamma_aic->setObjectName(QString::fromUtf8("txt_imp_invgamma_aic"));

        gridLayout_6->addWidget(txt_imp_invgamma_aic, 1, 1, 1, 1);


        verticalLayout_10->addWidget(frame_imp_aic);

        results_content->addTab(tab_results_aic, QString());
        tab_results_aicc = new QWidget();
        tab_results_aicc->setObjectName(QString::fromUtf8("tab_results_aicc"));
        verticalLayout_11 = new QVBoxLayout(tab_results_aicc);
        verticalLayout_11->setObjectName(QString::fromUtf8("verticalLayout_11"));
        table_results_aicc = new QTableView(tab_results_aicc);
        table_results_aicc->setObjectName(QString::fromUtf8("table_results_aicc"));
        table_results_aicc->setFont(font);
        table_results_aicc->setAlternatingRowColors(true);
        table_results_aicc->setSortingEnabled(true);

        verticalLayout_11->addWidget(table_results_aicc);

        frame_imp_aicc = new QFrame(tab_results_aicc);
        frame_imp_aicc->setObjectName(QString::fromUtf8("frame_imp_aicc"));
        frame_imp_aicc->setMinimumSize(QSize(0, 20));
        frame_imp_aicc->setFrameShape(QFrame::StyledPanel);
        frame_imp_aicc->setFrameShadow(QFrame::Raised);
        gridLayout_8 = new QGridLayout(frame_imp_aicc);
        gridLayout_8->setObjectName(QString::fromUtf8("gridLayout_8"));
        gridLayout_8->setVerticalSpacing(6);
        lbl_imp_aicc = new QLabel(frame_imp_aicc);
        lbl_imp_aicc->setObjectName(QString::fromUtf8("lbl_imp_aicc"));
        sizePolicy.setHeightForWidth(lbl_imp_aicc->sizePolicy().hasHeightForWidth());
        lbl_imp_aicc->setSizePolicy(sizePolicy);
        lbl_imp_aicc->setMinimumSize(QSize(0, 0));
        lbl_imp_aicc->setFont(font2);
        lbl_imp_aicc->setLineWidth(1);

        gridLayout_8->addWidget(lbl_imp_aicc, 0, 0, 1, 2, Qt::AlignHCenter);

        lbl_imp_gamma_aicc = new QLabel(frame_imp_aicc);
        lbl_imp_gamma_aicc->setObjectName(QString::fromUtf8("lbl_imp_gamma_aicc"));
        lbl_imp_gamma_aicc->setMinimumSize(QSize(100, 0));

        gridLayout_8->addWidget(lbl_imp_gamma_aicc, 3, 0, 1, 1);

        spacer_imp_aicc = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_8->addItem(spacer_imp_aicc, 3, 2, 1, 1);

        lbl_imp_freqs_aicc = new QLabel(frame_imp_aicc);
        lbl_imp_freqs_aicc->setObjectName(QString::fromUtf8("lbl_imp_freqs_aicc"));
        lbl_imp_freqs_aicc->setMinimumSize(QSize(100, 0));

        gridLayout_8->addWidget(lbl_imp_freqs_aicc, 4, 0, 1, 1);

        txt_imp_freqs_aicc = new QLabel(frame_imp_aicc);
        txt_imp_freqs_aicc->setObjectName(QString::fromUtf8("txt_imp_freqs_aicc"));
        txt_imp_freqs_aicc->setMinimumSize(QSize(60, 0));

        gridLayout_8->addWidget(txt_imp_freqs_aicc, 4, 1, 1, 1);

        txt_imp_gamma_aicc = new QLabel(frame_imp_aicc);
        txt_imp_gamma_aicc->setObjectName(QString::fromUtf8("txt_imp_gamma_aicc"));
        txt_imp_gamma_aicc->setMinimumSize(QSize(60, 0));

        gridLayout_8->addWidget(txt_imp_gamma_aicc, 3, 1, 1, 1);

        lbl_imp_inv_aicc = new QLabel(frame_imp_aicc);
        lbl_imp_inv_aicc->setObjectName(QString::fromUtf8("lbl_imp_inv_aicc"));
        lbl_imp_inv_aicc->setMinimumSize(QSize(100, 0));

        gridLayout_8->addWidget(lbl_imp_inv_aicc, 2, 0, 1, 1);

        txt_imp_inv_aicc = new QLabel(frame_imp_aicc);
        txt_imp_inv_aicc->setObjectName(QString::fromUtf8("txt_imp_inv_aicc"));
        txt_imp_inv_aicc->setMinimumSize(QSize(60, 0));

        gridLayout_8->addWidget(txt_imp_inv_aicc, 2, 1, 1, 1);

        lbl_imp_invgamma_aicc = new QLabel(frame_imp_aicc);
        lbl_imp_invgamma_aicc->setObjectName(QString::fromUtf8("lbl_imp_invgamma_aicc"));

        gridLayout_8->addWidget(lbl_imp_invgamma_aicc, 1, 0, 1, 1);

        txt_imp_invgamma_aicc = new QLabel(frame_imp_aicc);
        txt_imp_invgamma_aicc->setObjectName(QString::fromUtf8("txt_imp_invgamma_aicc"));

        gridLayout_8->addWidget(txt_imp_invgamma_aicc, 1, 1, 1, 1);


        verticalLayout_11->addWidget(frame_imp_aicc);

        results_content->addTab(tab_results_aicc, QString());
        tab_results_dt = new QWidget();
        tab_results_dt->setObjectName(QString::fromUtf8("tab_results_dt"));
        verticalLayout_13 = new QVBoxLayout(tab_results_dt);
        verticalLayout_13->setObjectName(QString::fromUtf8("verticalLayout_13"));
        table_results_dt = new QTableView(tab_results_dt);
        table_results_dt->setObjectName(QString::fromUtf8("table_results_dt"));
        table_results_dt->setFont(font);
        table_results_dt->setAlternatingRowColors(true);
        table_results_dt->setSortingEnabled(true);

        verticalLayout_13->addWidget(table_results_dt);

        frame_imp_dt = new QFrame(tab_results_dt);
        frame_imp_dt->setObjectName(QString::fromUtf8("frame_imp_dt"));
        frame_imp_dt->setMinimumSize(QSize(0, 20));
        frame_imp_dt->setFrameShape(QFrame::StyledPanel);
        frame_imp_dt->setFrameShadow(QFrame::Raised);
        gridLayout_9 = new QGridLayout(frame_imp_dt);
        gridLayout_9->setObjectName(QString::fromUtf8("gridLayout_9"));
        gridLayout_9->setVerticalSpacing(6);
        lbl_imp_dt = new QLabel(frame_imp_dt);
        lbl_imp_dt->setObjectName(QString::fromUtf8("lbl_imp_dt"));
        sizePolicy.setHeightForWidth(lbl_imp_dt->sizePolicy().hasHeightForWidth());
        lbl_imp_dt->setSizePolicy(sizePolicy);
        lbl_imp_dt->setMinimumSize(QSize(0, 0));
        lbl_imp_dt->setFont(font2);
        lbl_imp_dt->setLineWidth(1);

        gridLayout_9->addWidget(lbl_imp_dt, 0, 0, 1, 2, Qt::AlignHCenter);

        lbl_imp_gamma_dt = new QLabel(frame_imp_dt);
        lbl_imp_gamma_dt->setObjectName(QString::fromUtf8("lbl_imp_gamma_dt"));
        lbl_imp_gamma_dt->setMinimumSize(QSize(100, 0));

        gridLayout_9->addWidget(lbl_imp_gamma_dt, 3, 0, 1, 1);

        spacer_imp_dt = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_9->addItem(spacer_imp_dt, 3, 2, 1, 1);

        lbl_imp_freqs_dt = new QLabel(frame_imp_dt);
        lbl_imp_freqs_dt->setObjectName(QString::fromUtf8("lbl_imp_freqs_dt"));
        lbl_imp_freqs_dt->setMinimumSize(QSize(100, 0));

        gridLayout_9->addWidget(lbl_imp_freqs_dt, 4, 0, 1, 1);

        txt_imp_freqs_dt = new QLabel(frame_imp_dt);
        txt_imp_freqs_dt->setObjectName(QString::fromUtf8("txt_imp_freqs_dt"));
        txt_imp_freqs_dt->setMinimumSize(QSize(60, 0));

        gridLayout_9->addWidget(txt_imp_freqs_dt, 4, 1, 1, 1);

        txt_imp_gamma_dt = new QLabel(frame_imp_dt);
        txt_imp_gamma_dt->setObjectName(QString::fromUtf8("txt_imp_gamma_dt"));
        txt_imp_gamma_dt->setMinimumSize(QSize(60, 0));

        gridLayout_9->addWidget(txt_imp_gamma_dt, 3, 1, 1, 1);

        lbl_imp_inv_dt = new QLabel(frame_imp_dt);
        lbl_imp_inv_dt->setObjectName(QString::fromUtf8("lbl_imp_inv_dt"));
        lbl_imp_inv_dt->setMinimumSize(QSize(100, 0));

        gridLayout_9->addWidget(lbl_imp_inv_dt, 2, 0, 1, 1);

        txt_imp_inv_dt = new QLabel(frame_imp_dt);
        txt_imp_inv_dt->setObjectName(QString::fromUtf8("txt_imp_inv_dt"));
        txt_imp_inv_dt->setMinimumSize(QSize(60, 0));

        gridLayout_9->addWidget(txt_imp_inv_dt, 2, 1, 1, 1);

        lbl_imp_invgamma_dt = new QLabel(frame_imp_dt);
        lbl_imp_invgamma_dt->setObjectName(QString::fromUtf8("lbl_imp_invgamma_dt"));

        gridLayout_9->addWidget(lbl_imp_invgamma_dt, 1, 0, 1, 1);

        txt_imp_invgamma_dt = new QLabel(frame_imp_dt);
        txt_imp_invgamma_dt->setObjectName(QString::fromUtf8("txt_imp_invgamma_dt"));

        gridLayout_9->addWidget(txt_imp_invgamma_dt, 1, 1, 1, 1);


        verticalLayout_13->addWidget(frame_imp_dt);

        results_content->addTab(tab_results_dt, QString());

        verticalLayout_14->addWidget(results_content);


        verticalLayout->addWidget(frame_results_inner);


        retranslateUi(ResultsDialog);

        results_content->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(ResultsDialog);
    } // setupUi

    void retranslateUi(QWidget *ResultsDialog)
    {
        ResultsDialog->setWindowTitle(QApplication::translate("ResultsDialog", "Results", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("ResultsDialog", "Results", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        results_content->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        lbl_imp_bic->setText(QApplication::translate("ResultsDialog", "Importances", 0, QApplication::UnicodeUTF8));
        lbl_imp_gamma_bic->setText(QApplication::translate("ResultsDialog", "Gamma:", 0, QApplication::UnicodeUTF8));
        lbl_imp_freqs_bic->setText(QApplication::translate("ResultsDialog", "Frequencies:", 0, QApplication::UnicodeUTF8));
        txt_imp_freqs_bic->setText(QApplication::translate("ResultsDialog", "N/A", 0, QApplication::UnicodeUTF8));
        txt_imp_gamma_bic->setText(QApplication::translate("ResultsDialog", "N/A", 0, QApplication::UnicodeUTF8));
        lbl_imp_inv_bic->setText(QApplication::translate("ResultsDialog", "Prop.Inv:", 0, QApplication::UnicodeUTF8));
        txt_imp_inv_bic->setText(QApplication::translate("ResultsDialog", "N/A", 0, QApplication::UnicodeUTF8));
        lbl_imp_invgamma_bic->setText(QApplication::translate("ResultsDialog", "Inv+Gamma", 0, QApplication::UnicodeUTF8));
        txt_imp_invgamma_bic->setText(QApplication::translate("ResultsDialog", "N/A", 0, QApplication::UnicodeUTF8));
        results_content->setTabText(results_content->indexOf(tab_results_bic), QApplication::translate("ResultsDialog", "BIC", 0, QApplication::UnicodeUTF8));
        lbl_imp_aic->setText(QApplication::translate("ResultsDialog", "Importances", 0, QApplication::UnicodeUTF8));
        lbl_imp_gamma_aic->setText(QApplication::translate("ResultsDialog", "Gamma:", 0, QApplication::UnicodeUTF8));
        lbl_imp_freqs_aic->setText(QApplication::translate("ResultsDialog", "Frequencies:", 0, QApplication::UnicodeUTF8));
        txt_imp_freqs_aic->setText(QApplication::translate("ResultsDialog", "N/A", 0, QApplication::UnicodeUTF8));
        txt_imp_gamma_aic->setText(QApplication::translate("ResultsDialog", "N/A", 0, QApplication::UnicodeUTF8));
        lbl_imp_inv_aic->setText(QApplication::translate("ResultsDialog", "Prop.Inv:", 0, QApplication::UnicodeUTF8));
        txt_imp_inv_aic->setText(QApplication::translate("ResultsDialog", "N/A", 0, QApplication::UnicodeUTF8));
        lbl_imp_invgamma_aic->setText(QApplication::translate("ResultsDialog", "Inv+Gamma", 0, QApplication::UnicodeUTF8));
        txt_imp_invgamma_aic->setText(QApplication::translate("ResultsDialog", "N/A", 0, QApplication::UnicodeUTF8));
        results_content->setTabText(results_content->indexOf(tab_results_aic), QApplication::translate("ResultsDialog", "AIC", 0, QApplication::UnicodeUTF8));
        lbl_imp_aicc->setText(QApplication::translate("ResultsDialog", "Importances", 0, QApplication::UnicodeUTF8));
        lbl_imp_gamma_aicc->setText(QApplication::translate("ResultsDialog", "Gamma:", 0, QApplication::UnicodeUTF8));
        lbl_imp_freqs_aicc->setText(QApplication::translate("ResultsDialog", "Frequencies:", 0, QApplication::UnicodeUTF8));
        txt_imp_freqs_aicc->setText(QApplication::translate("ResultsDialog", "N/A", 0, QApplication::UnicodeUTF8));
        txt_imp_gamma_aicc->setText(QApplication::translate("ResultsDialog", "N/A", 0, QApplication::UnicodeUTF8));
        lbl_imp_inv_aicc->setText(QApplication::translate("ResultsDialog", "Prop.Inv:", 0, QApplication::UnicodeUTF8));
        txt_imp_inv_aicc->setText(QApplication::translate("ResultsDialog", "N/A", 0, QApplication::UnicodeUTF8));
        lbl_imp_invgamma_aicc->setText(QApplication::translate("ResultsDialog", "Inv+Gamma", 0, QApplication::UnicodeUTF8));
        txt_imp_invgamma_aicc->setText(QApplication::translate("ResultsDialog", "N/A", 0, QApplication::UnicodeUTF8));
        results_content->setTabText(results_content->indexOf(tab_results_aicc), QApplication::translate("ResultsDialog", "AICc", 0, QApplication::UnicodeUTF8));
        lbl_imp_dt->setText(QApplication::translate("ResultsDialog", "Importances", 0, QApplication::UnicodeUTF8));
        lbl_imp_gamma_dt->setText(QApplication::translate("ResultsDialog", "Gamma:", 0, QApplication::UnicodeUTF8));
        lbl_imp_freqs_dt->setText(QApplication::translate("ResultsDialog", "Frequencies:", 0, QApplication::UnicodeUTF8));
        txt_imp_freqs_dt->setText(QApplication::translate("ResultsDialog", "N/A", 0, QApplication::UnicodeUTF8));
        txt_imp_gamma_dt->setText(QApplication::translate("ResultsDialog", "N/A", 0, QApplication::UnicodeUTF8));
        lbl_imp_inv_dt->setText(QApplication::translate("ResultsDialog", "Prop.Inv:", 0, QApplication::UnicodeUTF8));
        txt_imp_inv_dt->setText(QApplication::translate("ResultsDialog", "N/A", 0, QApplication::UnicodeUTF8));
        lbl_imp_invgamma_dt->setText(QApplication::translate("ResultsDialog", "Inv+Gamma", 0, QApplication::UnicodeUTF8));
        txt_imp_invgamma_dt->setText(QApplication::translate("ResultsDialog", "N/A", 0, QApplication::UnicodeUTF8));
        results_content->setTabText(results_content->indexOf(tab_results_dt), QApplication::translate("ResultsDialog", "DT", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ResultsDialog: public Ui_ResultsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RESULTSDIALOG_H
