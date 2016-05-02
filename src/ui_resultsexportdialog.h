/********************************************************************************
** Form generated from reading UI file 'resultsexportdialog.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RESULTSEXPORTDIALOG_H
#define UI_RESULTSEXPORTDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QRadioButton>
#include <QtGui/QSlider>
#include <QtGui/QSpacerItem>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_ResultsExportDialog
{
public:
    QVBoxLayout *verticalLayout;
    QFrame *frame_options;
    QGridLayout *gridLayout;
    QLabel *lbl_nmodels;
    QFrame *frame_criteria;
    QHBoxLayout *horizontalLayout;
    QCheckBox *cb_aic;
    QCheckBox *cb_bic;
    QCheckBox *cb_aicc;
    QCheckBox *cb_dt;
    QLabel *lbl_criteria;
    QFrame *frame_nmodels;
    QHBoxLayout *horizontalLayout_2;
    QLabel *lbl_slidervalue;
    QSlider *slider_nmodels;
    QFrame *frame_type;
    QHBoxLayout *horizontalLayout_4;
    QSpacerItem *spacer_type_left;
    QRadioButton *rad_human;
    QRadioButton *rad_xml;
    QSpacerItem *spacer_type_right;
    QLabel *lbl_outfile;
    QFrame *frame_outfile;
    QHBoxLayout *horizontalLayout_3;
    QLineEdit *txt_outfile;
    QToolButton *tool_outfile;
    QFrame *frame_whereto;
    QHBoxLayout *horizontalLayout_5;
    QCheckBox *cb_toconsole;
    QCheckBox *cb_tofile;
    QSpacerItem *horizontalSpacer;
    QDialogButtonBox *box_buttons;

    void setupUi(QDialog *ResultsExportDialog)
    {
        if (ResultsExportDialog->objectName().isEmpty())
            ResultsExportDialog->setObjectName(QString::fromUtf8("ResultsExportDialog"));
        ResultsExportDialog->resize(575, 216);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(ResultsExportDialog->sizePolicy().hasHeightForWidth());
        ResultsExportDialog->setSizePolicy(sizePolicy);
        ResultsExportDialog->setStyleSheet(QString::fromUtf8("background: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 #3c3b37, stop:0.55 #104BA9, stop:0.98 #000015, stop:1 #000015);\n"
"QToolTip { color: #000s; border: none; }"));
        verticalLayout = new QVBoxLayout(ResultsExportDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setSizeConstraint(QLayout::SetFixedSize);
        frame_options = new QFrame(ResultsExportDialog);
        frame_options->setObjectName(QString::fromUtf8("frame_options"));
        frame_options->setStyleSheet(QString::fromUtf8("background: #fff2db; border-radius: 5px;"));
        frame_options->setFrameShape(QFrame::StyledPanel);
        frame_options->setFrameShadow(QFrame::Raised);
        gridLayout = new QGridLayout(frame_options);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        lbl_nmodels = new QLabel(frame_options);
        lbl_nmodels->setObjectName(QString::fromUtf8("lbl_nmodels"));
        lbl_nmodels->setMinimumSize(QSize(160, 0));
        lbl_nmodels->setMaximumSize(QSize(160, 100));
        QFont font;
        font.setFamily(QString::fromUtf8("Courier 10 Pitch"));
        font.setPointSize(10);
        lbl_nmodels->setFont(font);

        gridLayout->addWidget(lbl_nmodels, 4, 0, 1, 1);

        frame_criteria = new QFrame(frame_options);
        frame_criteria->setObjectName(QString::fromUtf8("frame_criteria"));
        frame_criteria->setFont(font);
        frame_criteria->setFrameShape(QFrame::NoFrame);
        frame_criteria->setFrameShadow(QFrame::Raised);
        horizontalLayout = new QHBoxLayout(frame_criteria);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        cb_aic = new QCheckBox(frame_criteria);
        cb_aic->setObjectName(QString::fromUtf8("cb_aic"));
        cb_aic->setChecked(true);

        horizontalLayout->addWidget(cb_aic);

        cb_bic = new QCheckBox(frame_criteria);
        cb_bic->setObjectName(QString::fromUtf8("cb_bic"));
        cb_bic->setChecked(true);

        horizontalLayout->addWidget(cb_bic);

        cb_aicc = new QCheckBox(frame_criteria);
        cb_aicc->setObjectName(QString::fromUtf8("cb_aicc"));
        cb_aicc->setChecked(true);

        horizontalLayout->addWidget(cb_aicc);

        cb_dt = new QCheckBox(frame_criteria);
        cb_dt->setObjectName(QString::fromUtf8("cb_dt"));
        cb_dt->setChecked(true);

        horizontalLayout->addWidget(cb_dt);


        gridLayout->addWidget(frame_criteria, 2, 1, 1, 1);

        lbl_criteria = new QLabel(frame_options);
        lbl_criteria->setObjectName(QString::fromUtf8("lbl_criteria"));
        lbl_criteria->setMinimumSize(QSize(160, 0));
        lbl_criteria->setMaximumSize(QSize(160, 100));
        lbl_criteria->setFont(font);

        gridLayout->addWidget(lbl_criteria, 2, 0, 1, 1);

        frame_nmodels = new QFrame(frame_options);
        frame_nmodels->setObjectName(QString::fromUtf8("frame_nmodels"));
        frame_nmodels->setFont(font);
        frame_nmodels->setFrameShape(QFrame::NoFrame);
        frame_nmodels->setFrameShadow(QFrame::Raised);
        horizontalLayout_2 = new QHBoxLayout(frame_nmodels);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        lbl_slidervalue = new QLabel(frame_nmodels);
        lbl_slidervalue->setObjectName(QString::fromUtf8("lbl_slidervalue"));
        lbl_slidervalue->setMinimumSize(QSize(40, 0));
        lbl_slidervalue->setMaximumSize(QSize(40, 16777215));
        lbl_slidervalue->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lbl_slidervalue->setIndent(5);

        horizontalLayout_2->addWidget(lbl_slidervalue);

        slider_nmodels = new QSlider(frame_nmodels);
        slider_nmodels->setObjectName(QString::fromUtf8("slider_nmodels"));
        slider_nmodels->setValue(10);
        slider_nmodels->setOrientation(Qt::Horizontal);

        horizontalLayout_2->addWidget(slider_nmodels);


        gridLayout->addWidget(frame_nmodels, 4, 1, 1, 1);

        frame_type = new QFrame(frame_options);
        frame_type->setObjectName(QString::fromUtf8("frame_type"));
        frame_type->setFrameShape(QFrame::NoFrame);
        frame_type->setFrameShadow(QFrame::Raised);
        horizontalLayout_4 = new QHBoxLayout(frame_type);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        spacer_type_left = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(spacer_type_left);

        rad_human = new QRadioButton(frame_type);
        rad_human->setObjectName(QString::fromUtf8("rad_human"));
        rad_human->setMinimumSize(QSize(160, 0));
        rad_human->setMaximumSize(QSize(160, 16777215));
        rad_human->setFont(font);
        rad_human->setChecked(true);

        horizontalLayout_4->addWidget(rad_human);

        rad_xml = new QRadioButton(frame_type);
        rad_xml->setObjectName(QString::fromUtf8("rad_xml"));
        rad_xml->setFont(font);
        rad_xml->setChecked(false);

        horizontalLayout_4->addWidget(rad_xml);

        spacer_type_right = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(spacer_type_right);


        gridLayout->addWidget(frame_type, 5, 0, 1, 2);

        lbl_outfile = new QLabel(frame_options);
        lbl_outfile->setObjectName(QString::fromUtf8("lbl_outfile"));
        lbl_outfile->setFont(font);

        gridLayout->addWidget(lbl_outfile, 1, 0, 1, 1);

        frame_outfile = new QFrame(frame_options);
        frame_outfile->setObjectName(QString::fromUtf8("frame_outfile"));
        frame_outfile->setFrameShape(QFrame::NoFrame);
        frame_outfile->setFrameShadow(QFrame::Raised);
        horizontalLayout_3 = new QHBoxLayout(frame_outfile);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        txt_outfile = new QLineEdit(frame_outfile);
        txt_outfile->setObjectName(QString::fromUtf8("txt_outfile"));
        txt_outfile->setStyleSheet(QString::fromUtf8("background: #fff; border-radius: 5px; border: 1px solid #555 ;"));

        horizontalLayout_3->addWidget(txt_outfile);

        tool_outfile = new QToolButton(frame_outfile);
        tool_outfile->setObjectName(QString::fromUtf8("tool_outfile"));
        tool_outfile->setStyleSheet(QString::fromUtf8("border: 1px solid #555; background: #eee;"));

        horizontalLayout_3->addWidget(tool_outfile);


        gridLayout->addWidget(frame_outfile, 1, 1, 1, 1);

        frame_whereto = new QFrame(frame_options);
        frame_whereto->setObjectName(QString::fromUtf8("frame_whereto"));
        frame_whereto->setFrameShape(QFrame::NoFrame);
        frame_whereto->setFrameShadow(QFrame::Raised);
        horizontalLayout_5 = new QHBoxLayout(frame_whereto);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        horizontalLayout_5->setContentsMargins(0, 0, 0, 0);
        cb_toconsole = new QCheckBox(frame_whereto);
        cb_toconsole->setObjectName(QString::fromUtf8("cb_toconsole"));
        cb_toconsole->setMinimumSize(QSize(130, 0));
        cb_toconsole->setMaximumSize(QSize(130, 16777215));
        cb_toconsole->setFont(font);
        cb_toconsole->setChecked(true);

        horizontalLayout_5->addWidget(cb_toconsole);

        cb_tofile = new QCheckBox(frame_whereto);
        cb_tofile->setObjectName(QString::fromUtf8("cb_tofile"));
        cb_tofile->setMinimumSize(QSize(130, 0));
        cb_tofile->setMaximumSize(QSize(130, 16777215));
        cb_tofile->setFont(font);

        horizontalLayout_5->addWidget(cb_tofile);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer);


        gridLayout->addWidget(frame_whereto, 0, 1, 1, 1);


        verticalLayout->addWidget(frame_options);

        box_buttons = new QDialogButtonBox(ResultsExportDialog);
        box_buttons->setObjectName(QString::fromUtf8("box_buttons"));
        box_buttons->setStyleSheet(QString::fromUtf8("background: #ccc;"));
        box_buttons->setOrientation(Qt::Horizontal);
        box_buttons->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(box_buttons);


        retranslateUi(ResultsExportDialog);
        QObject::connect(box_buttons, SIGNAL(accepted()), ResultsExportDialog, SLOT(accept()));
        QObject::connect(box_buttons, SIGNAL(rejected()), ResultsExportDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(ResultsExportDialog);
    } // setupUi

    void retranslateUi(QDialog *ResultsExportDialog)
    {
        ResultsExportDialog->setWindowTitle(QApplication::translate("ResultsExportDialog", "Export options", 0, QApplication::UnicodeUTF8));
        lbl_nmodels->setText(QApplication::translate("ResultsExportDialog", "Shown models:", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        cb_aic->setToolTip(QApplication::translate("ResultsExportDialog", "Akaike Information Criterion", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        cb_aic->setText(QApplication::translate("ResultsExportDialog", "AIC", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        cb_bic->setToolTip(QApplication::translate("ResultsExportDialog", "Bayesian Information Criterion", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        cb_bic->setText(QApplication::translate("ResultsExportDialog", "BIC", 0, QApplication::UnicodeUTF8));
        cb_aicc->setText(QApplication::translate("ResultsExportDialog", "AICc", 0, QApplication::UnicodeUTF8));
        cb_dt->setText(QApplication::translate("ResultsExportDialog", "DT", 0, QApplication::UnicodeUTF8));
        lbl_criteria->setText(QApplication::translate("ResultsExportDialog", "Selection criteria:", 0, QApplication::UnicodeUTF8));
        lbl_slidervalue->setText(QApplication::translate("ResultsExportDialog", "10", 0, QApplication::UnicodeUTF8));
        rad_human->setText(QApplication::translate("ResultsExportDialog", "Human readable", 0, QApplication::UnicodeUTF8));
        rad_xml->setText(QApplication::translate("ResultsExportDialog", "XML", 0, QApplication::UnicodeUTF8));
        lbl_outfile->setText(QApplication::translate("ResultsExportDialog", "Output file:", 0, QApplication::UnicodeUTF8));
        tool_outfile->setText(QApplication::translate("ResultsExportDialog", "...", 0, QApplication::UnicodeUTF8));
        cb_toconsole->setText(QApplication::translate("ResultsExportDialog", "To console", 0, QApplication::UnicodeUTF8));
        cb_tofile->setText(QApplication::translate("ResultsExportDialog", "To file", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ResultsExportDialog: public Ui_ResultsExportDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RESULTSEXPORTDIALOG_H
