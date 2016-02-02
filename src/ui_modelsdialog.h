/********************************************************************************
** Form generated from reading UI file 'modelsdialog.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MODELSDIALOG_H
#define UI_MODELSDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSpacerItem>
#include <QtGui/QTableView>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ModelsDialog
{
public:
    QAction *action_close;
    QVBoxLayout *verticalLayout;
    QFrame *frame_header;
    QHBoxLayout *horizontalLayout_6;
    QComboBox *cmb_partition;
    QSpacerItem *horizontalSpacer;
    QLabel *label;
    QSpacerItem *spacer_header;
    QToolButton *toolButton;
    QTableView *table_models;

    void setupUi(QWidget *ModelsDialog)
    {
        if (ModelsDialog->objectName().isEmpty())
            ModelsDialog->setObjectName(QString::fromUtf8("ModelsDialog"));
        ModelsDialog->resize(859, 349);
        ModelsDialog->setStyleSheet(QString::fromUtf8("background: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 #3c3b37, stop:0.55 #104BA9, stop:0.98 #000015, stop:1 #000015);\n"
"QToolTip { color: #000s; border: none; }"));
        action_close = new QAction(ModelsDialog);
        action_close->setObjectName(QString::fromUtf8("action_close"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/IMG/IMG/gear.png"), QSize(), QIcon::Normal, QIcon::On);
        action_close->setIcon(icon);
        verticalLayout = new QVBoxLayout(ModelsDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        frame_header = new QFrame(ModelsDialog);
        frame_header->setObjectName(QString::fromUtf8("frame_header"));
        frame_header->setStyleSheet(QString::fromUtf8("background: #104BA9;"));
        frame_header->setFrameShape(QFrame::NoFrame);
        frame_header->setFrameShadow(QFrame::Raised);
        horizontalLayout_6 = new QHBoxLayout(frame_header);
        horizontalLayout_6->setSpacing(5);
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        horizontalLayout_6->setContentsMargins(5, 1, 5, 1);
        cmb_partition = new QComboBox(frame_header);
        cmb_partition->setObjectName(QString::fromUtf8("cmb_partition"));
        cmb_partition->setMinimumSize(QSize(200, 0));
        cmb_partition->setMaximumSize(QSize(200, 16777215));
        QFont font;
        font.setFamily(QString::fromUtf8("Courier 10 Pitch"));
        font.setPointSize(10);
        cmb_partition->setFont(font);
        cmb_partition->setStyleSheet(QString::fromUtf8("border: 1px; background: #fff2db; border-radius:5px;"));

        horizontalLayout_6->addWidget(cmb_partition);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer);

        label = new QLabel(frame_header);
        label->setObjectName(QString::fromUtf8("label"));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Courier 10 Pitch"));
        font1.setPointSize(12);
        font1.setBold(true);
        font1.setWeight(75);
        label->setFont(font1);
        label->setStyleSheet(QString::fromUtf8("color:#fff;"));

        horizontalLayout_6->addWidget(label);

        spacer_header = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(spacer_header);

        toolButton = new QToolButton(frame_header);
        toolButton->setObjectName(QString::fromUtf8("toolButton"));
        toolButton->setFont(font);
        toolButton->setStyleSheet(QString::fromUtf8("background: #fff2db;"));

        horizontalLayout_6->addWidget(toolButton);


        verticalLayout->addWidget(frame_header);

        table_models = new QTableView(ModelsDialog);
        table_models->setObjectName(QString::fromUtf8("table_models"));
        QFont font2;
        font2.setFamily(QString::fromUtf8("Courier 10 Pitch"));
        font2.setPointSize(9);
        font2.setBold(false);
        font2.setWeight(50);
        table_models->setFont(font2);
        table_models->setStyleSheet(QString::fromUtf8("background: #fff2db;"));
        table_models->setAlternatingRowColors(true);
        table_models->setSortingEnabled(true);

        verticalLayout->addWidget(table_models);


        retranslateUi(ModelsDialog);

        QMetaObject::connectSlotsByName(ModelsDialog);
    } // setupUi

    void retranslateUi(QWidget *ModelsDialog)
    {
        ModelsDialog->setWindowTitle(QApplication::translate("ModelsDialog", "Model parameters", 0, QApplication::UnicodeUTF8));
        action_close->setText(QApplication::translate("ModelsDialog", "Close", 0, QApplication::UnicodeUTF8));
        action_close->setShortcut(QApplication::translate("ModelsDialog", "Ctrl+W", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("ModelsDialog", "Models", 0, QApplication::UnicodeUTF8));
        toolButton->setText(QApplication::translate("ModelsDialog", "hide", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ModelsDialog: public Ui_ModelsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MODELSDIALOG_H
