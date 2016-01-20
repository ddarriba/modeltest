/********************************************************************************
** Form generated from reading UI file 'progressdialog.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PROGRESSDIALOG_H
#define UI_PROGRESSDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_ProgressDialog
{
public:
    QVBoxLayout *verticalLayout;
    QFrame *frame_main;
    QVBoxLayout *verticalLayout_2;
    QProgressBar *progress_bar;
    QFrame *frame_tjread1;
    QHBoxLayout *horizontalLayout;
    QLabel *lbl_thread1;
    QLabel *lbl_model1;
    QLabel *lbl_delta1;
    QPushButton *btn_stop;

    void setupUi(QDialog *ProgressDialog)
    {
        if (ProgressDialog->objectName().isEmpty())
            ProgressDialog->setObjectName(QString::fromUtf8("ProgressDialog"));
        ProgressDialog->resize(490, 137);
        verticalLayout = new QVBoxLayout(ProgressDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        frame_main = new QFrame(ProgressDialog);
        frame_main->setObjectName(QString::fromUtf8("frame_main"));
        frame_main->setFrameShape(QFrame::StyledPanel);
        frame_main->setFrameShadow(QFrame::Raised);
        verticalLayout_2 = new QVBoxLayout(frame_main);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        progress_bar = new QProgressBar(frame_main);
        progress_bar->setObjectName(QString::fromUtf8("progress_bar"));
        progress_bar->setValue(24);

        verticalLayout_2->addWidget(progress_bar);

        frame_tjread1 = new QFrame(frame_main);
        frame_tjread1->setObjectName(QString::fromUtf8("frame_tjread1"));
        frame_tjread1->setMinimumSize(QSize(0, 35));
        frame_tjread1->setMaximumSize(QSize(16777215, 35));
        frame_tjread1->setFrameShape(QFrame::StyledPanel);
        frame_tjread1->setFrameShadow(QFrame::Raised);
        horizontalLayout = new QHBoxLayout(frame_tjread1);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        lbl_thread1 = new QLabel(frame_tjread1);
        lbl_thread1->setObjectName(QString::fromUtf8("lbl_thread1"));
        lbl_thread1->setMinimumSize(QSize(120, 0));
        lbl_thread1->setMaximumSize(QSize(120, 16777215));

        horizontalLayout->addWidget(lbl_thread1);

        lbl_model1 = new QLabel(frame_tjread1);
        lbl_model1->setObjectName(QString::fromUtf8("lbl_model1"));
        lbl_model1->setMinimumSize(QSize(150, 0));
        lbl_model1->setMaximumSize(QSize(150, 16777215));

        horizontalLayout->addWidget(lbl_model1);

        lbl_delta1 = new QLabel(frame_tjread1);
        lbl_delta1->setObjectName(QString::fromUtf8("lbl_delta1"));
        lbl_delta1->setMinimumSize(QSize(150, 0));

        horizontalLayout->addWidget(lbl_delta1);


        verticalLayout_2->addWidget(frame_tjread1);

        btn_stop = new QPushButton(frame_main);
        btn_stop->setObjectName(QString::fromUtf8("btn_stop"));

        verticalLayout_2->addWidget(btn_stop);


        verticalLayout->addWidget(frame_main);


        retranslateUi(ProgressDialog);

        QMetaObject::connectSlotsByName(ProgressDialog);
    } // setupUi

    void retranslateUi(QDialog *ProgressDialog)
    {
        ProgressDialog->setWindowTitle(QApplication::translate("ProgressDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        lbl_thread1->setText(QApplication::translate("ProgressDialog", "thread 1/1", 0, QApplication::UnicodeUTF8));
        lbl_model1->setText(QApplication::translate("ProgressDialog", "GTR+I+G", 0, QApplication::UnicodeUTF8));
        lbl_delta1->setText(QApplication::translate("ProgressDialog", "10.1023", 0, QApplication::UnicodeUTF8));
        btn_stop->setText(QApplication::translate("ProgressDialog", "Stop", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ProgressDialog: public Ui_ProgressDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PROGRESSDIALOG_H
