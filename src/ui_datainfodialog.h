/********************************************************************************
** Form generated from reading UI file 'datainfodialog.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DATAINFODIALOG_H
#define UI_DATAINFODIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QTextBrowser>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DataInfoDialog
{
public:
    QVBoxLayout *verticalLayout;
    QTextBrowser *text;

    void setupUi(QWidget *DataInfoDialog)
    {
        if (DataInfoDialog->objectName().isEmpty())
            DataInfoDialog->setObjectName(QString::fromUtf8("DataInfoDialog"));
        DataInfoDialog->resize(800, 300);
        DataInfoDialog->setMinimumSize(QSize(800, 300));
        verticalLayout = new QVBoxLayout(DataInfoDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        text = new QTextBrowser(DataInfoDialog);
        text->setObjectName(QString::fromUtf8("text"));
        QFont font;
        font.setFamily(QString::fromUtf8("Courier 10 Pitch"));
        text->setFont(font);
        text->setLineWrapMode(QTextEdit::NoWrap);

        verticalLayout->addWidget(text);


        retranslateUi(DataInfoDialog);

        QMetaObject::connectSlotsByName(DataInfoDialog);
    } // setupUi

    void retranslateUi(QWidget *DataInfoDialog)
    {
        DataInfoDialog->setWindowTitle(QApplication::translate("DataInfoDialog", "Data info", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class DataInfoDialog: public Ui_DataInfoDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DATAINFODIALOG_H
