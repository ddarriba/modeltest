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
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QTextBrowser>

QT_BEGIN_NAMESPACE

class Ui_ProgressDialog
{
public:
    QProgressBar *progressBar;
    QLabel *label;
    QTextBrowser *textBrowser;
    QPushButton *btnStop;

    void setupUi(QDialog *ProgressDialog)
    {
        if (ProgressDialog->objectName().isEmpty())
            ProgressDialog->setObjectName(QString::fromUtf8("ProgressDialog"));
        ProgressDialog->resize(488, 325);
        progressBar = new QProgressBar(ProgressDialog);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setGeometry(QRect(10, 30, 471, 23));
        progressBar->setValue(24);
        label = new QLabel(ProgressDialog);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 10, 451, 17));
        textBrowser = new QTextBrowser(ProgressDialog);
        textBrowser->setObjectName(QString::fromUtf8("textBrowser"));
        textBrowser->setGeometry(QRect(10, 60, 461, 211));
        QFont font;
        font.setFamily(QString::fromUtf8("Ubuntu Mono"));
        textBrowser->setFont(font);
        btnStop = new QPushButton(ProgressDialog);
        btnStop->setObjectName(QString::fromUtf8("btnStop"));
        btnStop->setGeometry(QRect(370, 280, 99, 27));

        retranslateUi(ProgressDialog);

        QMetaObject::connectSlotsByName(ProgressDialog);
    } // setupUi

    void retranslateUi(QDialog *ProgressDialog)
    {
        ProgressDialog->setWindowTitle(QApplication::translate("ProgressDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("ProgressDialog", "TextLabel", 0, QApplication::UnicodeUTF8));
        btnStop->setText(QApplication::translate("ProgressDialog", "Stop", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ProgressDialog: public Ui_ProgressDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PROGRESSDIALOG_H
