/********************************************************************************
** Form generated from reading UI file 'xmodeltest.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_XMODELTEST_H
#define UI_XMODELTEST_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QRadioButton>
#include <QtGui/QScrollArea>
#include <QtGui/QSlider>
#include <QtGui/QSpacerItem>
#include <QtGui/QSplitter>
#include <QtGui/QStatusBar>
#include <QtGui/QTextBrowser>
#include <QtGui/QToolBar>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_xmodeltest
{
public:
    QAction *act_open_msa;
    QAction *act_open_tree;
    QAction *act_open_parts;
    QAction *mnu_open_msa;
    QAction *mnu_open_tree;
    QAction *mnu_open_parts;
    QAction *act_toggle_settings;
    QAction *mnu_toggle_settings;
    QAction *act_run;
    QAction *act_results;
    QAction *act_reset;
    QAction *mnu_copy;
    QAction *mnu_about;
    QAction *mnu_index;
    QAction *mnu_reset;
    QAction *mnu_run;
    QAction *mnu_results;
    QAction *mnu_view_datainfo;
    QAction *mnu_models;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QFrame *frame_top;
    QVBoxLayout *verticalLayout_2;
    QFrame *frame_tools_1;
    QHBoxLayout *horizontalLayout_3;
    QToolButton *tool_open_msa;
    QToolButton *tool_open_tree;
    QToolButton *tool_open_parts;
    QSpacerItem *horizontalSpacer;
    QToolButton *tool_results;
    QFrame *frame_tools_2;
    QHBoxLayout *horizontalLayout_4;
    QToolButton *tool_settings;
    QToolButton *tool_run;
    QToolButton *tool_reset;
    QSpacerItem *horizontalSpacer_2;
    QToolButton *tool_help;
    QFrame *frame_header;
    QHBoxLayout *horizontalLayout;
    QLabel *mt_icon;
    QFrame *frame_data;
    QGridLayout *gridLayout;
    QLabel *lbl_i_tree;
    QLabel *lbl_msa;
    QLabel *lbl_i_msa;
    QLabel *lbl_mem;
    QLabel *lbl_i_parts;
    QLabel *lbl_parts;
    QLabel *lbl_i_mem;
    QLabel *lbl_tree;
    QFrame *frame_middle;
    QHBoxLayout *horizontalLayout_2;
    QSplitter *splitter;
    QFrame *frame_console;
    QVBoxLayout *verticalLayout_7;
    QSplitter *splitter_2;
    QScrollArea *frame_settings;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout_9;
    QFrame *frameData;
    QVBoxLayout *verticalLayout_5;
    QFrame *frame_12;
    QHBoxLayout *horizontalLayout_10;
    QLabel *label_8;
    QRadioButton *radDatatypeDna;
    QRadioButton *radDatatypeProt;
    QSpacerItem *horizontalSpacer_11;
    QFrame *frame_settings_3;
    QHBoxLayout *horizontalLayout_14;
    QLabel *label_9;
    QRadioButton *radSetModelTest;
    QRadioButton *radSetMrbayes;
    QRadioButton *radSetRaxml;
    QRadioButton *radSetPhyml;
    QRadioButton *radSetPAUP;
    QSpacerItem *horizontalSpacer_12;
    QLabel *label_10;
    QFrame *frame_settings_4;
    QHBoxLayout *horizontalLayout_11;
    QRadioButton *radTopoFixedMp;
    QRadioButton *radTopoFixedJc;
    QRadioButton *radTopoFixedGtr;
    QRadioButton *radTopoML;
    QRadioButton *radTopoU;
    QSpacerItem *horizontalSpacer_13;
    QFrame *line_3;
    QLabel *label_11;
    QFrame *frameModels;
    QVBoxLayout *verticalLayout_12;
    QFrame *frame_models;
    QHBoxLayout *horizontalLayout_5;
    QFrame *frame_modeldefs;
    QVBoxLayout *verticalLayout_3;
    QFrame *grpSubstSchemes;
    QHBoxLayout *horizontalLayout_12;
    QRadioButton *radSchemes3;
    QRadioButton *radSchemes5;
    QRadioButton *radSchemes7;
    QRadioButton *radSchemes11;
    QRadioButton *radSchemes203;
    QRadioButton *radSchemesUser;
    QSpacerItem *horizontalSpacer_14;
    QLabel *label_12;
    QFrame *frame_13;
    QGridLayout *gridLayout_7;
    QCheckBox *cbNoRateVarModels;
    QCheckBox *cbIGModels;
    QCheckBox *cbIModels;
    QSpacerItem *horizontalSpacer_15;
    QLabel *lblNCat;
    QCheckBox *cbGModels;
    QSlider *sliderNCat;
    QLabel *label_13;
    QFrame *frame_14;
    QHBoxLayout *horizontalLayout_15;
    QCheckBox *cbEqualFreq;
    QCheckBox *cbMlFreq;
    QSpacerItem *horizontalSpacer_16;
    QFrame *frame_nmodels;
    QHBoxLayout *horizontalLayout_16;
    QLabel *lblTextNModels;
    QLabel *lblNumModels;
    QSpacerItem *horizontalSpacer_17;
    QCheckBox *cbShowMatrices;
    QListWidget *modelsListView;
    QSpacerItem *horizontalSpacer_3;
    QFrame *line_4;
    QCheckBox *cbAdvanced;
    QFrame *frame_advanced;
    QGridLayout *gridLayout_3;
    QLineEdit *txtOptEpsilon;
    QLineEdit *txtParEpsilon;
    QLabel *lblParEpsText;
    QLabel *lblOptEpsText;
    QLabel *lblNThreads;
    QSlider *sliderNThreads;
    QLabel *lblNThreadText;
    QSpacerItem *horizontalSpacer_18;
    QFrame *grpConsoles;
    QVBoxLayout *verticalLayout_4;
    QTextBrowser *consoleRun;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuEdit;
    QMenu *menuRun;
    QMenu *menuHelp;
    QMenu *menuView;
    QStatusBar *statusbar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *xmodeltest)
    {
        if (xmodeltest->objectName().isEmpty())
            xmodeltest->setObjectName(QString::fromUtf8("xmodeltest"));
        xmodeltest->resize(877, 800);
        xmodeltest->setMinimumSize(QSize(740, 0));
        xmodeltest->setWindowTitle(QString::fromUtf8("ModelTest Light"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/IMG/IMG/mt-icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        xmodeltest->setWindowIcon(icon);
        xmodeltest->setStyleSheet(QString::fromUtf8(""));
        act_open_msa = new QAction(xmodeltest);
        act_open_msa->setObjectName(QString::fromUtf8("act_open_msa"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/IMG/IMG/icon-openMSA.png"), QSize(), QIcon::Normal, QIcon::Off);
        act_open_msa->setIcon(icon1);
        act_open_tree = new QAction(xmodeltest);
        act_open_tree->setObjectName(QString::fromUtf8("act_open_tree"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/IMG/IMG/icon-opentree.png"), QSize(), QIcon::Normal, QIcon::Off);
        act_open_tree->setIcon(icon2);
        act_open_parts = new QAction(xmodeltest);
        act_open_parts->setObjectName(QString::fromUtf8("act_open_parts"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/IMG/IMG/icon-openparts.png"), QSize(), QIcon::Normal, QIcon::Off);
        act_open_parts->setIcon(icon3);
        mnu_open_msa = new QAction(xmodeltest);
        mnu_open_msa->setObjectName(QString::fromUtf8("mnu_open_msa"));
        mnu_open_msa->setIcon(icon1);
        mnu_open_tree = new QAction(xmodeltest);
        mnu_open_tree->setObjectName(QString::fromUtf8("mnu_open_tree"));
        mnu_open_tree->setIcon(icon2);
        mnu_open_parts = new QAction(xmodeltest);
        mnu_open_parts->setObjectName(QString::fromUtf8("mnu_open_parts"));
        mnu_open_parts->setIcon(icon3);
        act_toggle_settings = new QAction(xmodeltest);
        act_toggle_settings->setObjectName(QString::fromUtf8("act_toggle_settings"));
        act_toggle_settings->setCheckable(true);
        act_toggle_settings->setChecked(false);
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/IMG/IMG/icon-settings.png"), QSize(), QIcon::Normal, QIcon::Off);
        act_toggle_settings->setIcon(icon4);
        mnu_toggle_settings = new QAction(xmodeltest);
        mnu_toggle_settings->setObjectName(QString::fromUtf8("mnu_toggle_settings"));
        mnu_toggle_settings->setCheckable(true);
        mnu_toggle_settings->setChecked(false);
        mnu_toggle_settings->setIcon(icon4);
        act_run = new QAction(xmodeltest);
        act_run->setObjectName(QString::fromUtf8("act_run"));
        act_run->setEnabled(true);
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/IMG/IMG/run-icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        act_run->setIcon(icon5);
        act_results = new QAction(xmodeltest);
        act_results->setObjectName(QString::fromUtf8("act_results"));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/IMG/IMG/results-icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        act_results->setIcon(icon6);
        act_reset = new QAction(xmodeltest);
        act_reset->setObjectName(QString::fromUtf8("act_reset"));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/IMG/IMG/icon-reset.png"), QSize(), QIcon::Normal, QIcon::Off);
        act_reset->setIcon(icon7);
        mnu_copy = new QAction(xmodeltest);
        mnu_copy->setObjectName(QString::fromUtf8("mnu_copy"));
        mnu_about = new QAction(xmodeltest);
        mnu_about->setObjectName(QString::fromUtf8("mnu_about"));
        mnu_index = new QAction(xmodeltest);
        mnu_index->setObjectName(QString::fromUtf8("mnu_index"));
        mnu_reset = new QAction(xmodeltest);
        mnu_reset->setObjectName(QString::fromUtf8("mnu_reset"));
        mnu_reset->setIcon(icon7);
        mnu_run = new QAction(xmodeltest);
        mnu_run->setObjectName(QString::fromUtf8("mnu_run"));
        mnu_run->setIcon(icon5);
        mnu_results = new QAction(xmodeltest);
        mnu_results->setObjectName(QString::fromUtf8("mnu_results"));
        mnu_view_datainfo = new QAction(xmodeltest);
        mnu_view_datainfo->setObjectName(QString::fromUtf8("mnu_view_datainfo"));
        mnu_models = new QAction(xmodeltest);
        mnu_models->setObjectName(QString::fromUtf8("mnu_models"));
        centralwidget = new QWidget(xmodeltest);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        frame_top = new QFrame(centralwidget);
        frame_top->setObjectName(QString::fromUtf8("frame_top"));
        frame_top->setMaximumSize(QSize(16777215, 70));
        frame_top->setFrameShape(QFrame::StyledPanel);
        frame_top->setFrameShadow(QFrame::Raised);
        verticalLayout_2 = new QVBoxLayout(frame_top);
        verticalLayout_2->setSpacing(5);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(-1, 5, -1, 5);
        frame_tools_1 = new QFrame(frame_top);
        frame_tools_1->setObjectName(QString::fromUtf8("frame_tools_1"));
        frame_tools_1->setMaximumSize(QSize(16777215, 30));
        frame_tools_1->setFrameShape(QFrame::NoFrame);
        frame_tools_1->setFrameShadow(QFrame::Raised);
        horizontalLayout_3 = new QHBoxLayout(frame_tools_1);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(-1, 0, -1, 0);
        tool_open_msa = new QToolButton(frame_tools_1);
        tool_open_msa->setObjectName(QString::fromUtf8("tool_open_msa"));
        QFont font;
        font.setFamily(QString::fromUtf8("Courier 10 Pitch"));
        font.setPointSize(10);
        font.setBold(true);
        font.setWeight(75);
        tool_open_msa->setFont(font);
        tool_open_msa->setAutoFillBackground(false);
        tool_open_msa->setStyleSheet(QString::fromUtf8(""));

        horizontalLayout_3->addWidget(tool_open_msa);

        tool_open_tree = new QToolButton(frame_tools_1);
        tool_open_tree->setObjectName(QString::fromUtf8("tool_open_tree"));
        tool_open_tree->setFont(font);

        horizontalLayout_3->addWidget(tool_open_tree);

        tool_open_parts = new QToolButton(frame_tools_1);
        tool_open_parts->setObjectName(QString::fromUtf8("tool_open_parts"));
        tool_open_parts->setFont(font);

        horizontalLayout_3->addWidget(tool_open_parts);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);

        tool_results = new QToolButton(frame_tools_1);
        tool_results->setObjectName(QString::fromUtf8("tool_results"));
        tool_results->setFont(font);
        tool_results->setCheckable(false);

        horizontalLayout_3->addWidget(tool_results);


        verticalLayout_2->addWidget(frame_tools_1);

        frame_tools_2 = new QFrame(frame_top);
        frame_tools_2->setObjectName(QString::fromUtf8("frame_tools_2"));
        frame_tools_2->setMaximumSize(QSize(16777215, 30));
        frame_tools_2->setFrameShape(QFrame::NoFrame);
        frame_tools_2->setFrameShadow(QFrame::Raised);
        horizontalLayout_4 = new QHBoxLayout(frame_tools_2);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(-1, 0, -1, 0);
        tool_settings = new QToolButton(frame_tools_2);
        tool_settings->setObjectName(QString::fromUtf8("tool_settings"));
        tool_settings->setFont(font);
        tool_settings->setCheckable(true);
        tool_settings->setChecked(false);

        horizontalLayout_4->addWidget(tool_settings);

        tool_run = new QToolButton(frame_tools_2);
        tool_run->setObjectName(QString::fromUtf8("tool_run"));
        tool_run->setFont(font);

        horizontalLayout_4->addWidget(tool_run);

        tool_reset = new QToolButton(frame_tools_2);
        tool_reset->setObjectName(QString::fromUtf8("tool_reset"));
        tool_reset->setFont(font);

        horizontalLayout_4->addWidget(tool_reset);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_2);

        tool_help = new QToolButton(frame_tools_2);
        tool_help->setObjectName(QString::fromUtf8("tool_help"));
        tool_help->setFont(font);

        horizontalLayout_4->addWidget(tool_help);


        verticalLayout_2->addWidget(frame_tools_2);


        verticalLayout->addWidget(frame_top);

        frame_header = new QFrame(centralwidget);
        frame_header->setObjectName(QString::fromUtf8("frame_header"));
        frame_header->setMaximumSize(QSize(16777215, 100));
        frame_header->setFrameShape(QFrame::NoFrame);
        frame_header->setFrameShadow(QFrame::Raised);
        horizontalLayout = new QHBoxLayout(frame_header);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        mt_icon = new QLabel(frame_header);
        mt_icon->setObjectName(QString::fromUtf8("mt_icon"));
        mt_icon->setMaximumSize(QSize(80, 80));
        mt_icon->setPixmap(QPixmap(QString::fromUtf8(":/IMG/IMG/mt-icon.png")));
        mt_icon->setScaledContents(true);

        horizontalLayout->addWidget(mt_icon);

        frame_data = new QFrame(frame_header);
        frame_data->setObjectName(QString::fromUtf8("frame_data"));
        frame_data->setFrameShape(QFrame::StyledPanel);
        frame_data->setFrameShadow(QFrame::Raised);
        gridLayout = new QGridLayout(frame_data);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        lbl_i_tree = new QLabel(frame_data);
        lbl_i_tree->setObjectName(QString::fromUtf8("lbl_i_tree"));
        lbl_i_tree->setMinimumSize(QSize(100, 0));
        lbl_i_tree->setMaximumSize(QSize(100, 16777215));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Courier 10 Pitch"));
        font1.setPointSize(10);
        lbl_i_tree->setFont(font1);

        gridLayout->addWidget(lbl_i_tree, 1, 0, 1, 1);

        lbl_msa = new QLabel(frame_data);
        lbl_msa->setObjectName(QString::fromUtf8("lbl_msa"));
        lbl_msa->setFont(font1);

        gridLayout->addWidget(lbl_msa, 0, 1, 1, 1);

        lbl_i_msa = new QLabel(frame_data);
        lbl_i_msa->setObjectName(QString::fromUtf8("lbl_i_msa"));
        lbl_i_msa->setMinimumSize(QSize(100, 0));
        lbl_i_msa->setMaximumSize(QSize(100, 16777215));
        lbl_i_msa->setFont(font1);

        gridLayout->addWidget(lbl_i_msa, 0, 0, 1, 1);

        lbl_mem = new QLabel(frame_data);
        lbl_mem->setObjectName(QString::fromUtf8("lbl_mem"));
        lbl_mem->setFont(font1);

        gridLayout->addWidget(lbl_mem, 3, 1, 1, 1);

        lbl_i_parts = new QLabel(frame_data);
        lbl_i_parts->setObjectName(QString::fromUtf8("lbl_i_parts"));
        lbl_i_parts->setMinimumSize(QSize(100, 0));
        lbl_i_parts->setMaximumSize(QSize(100, 16777215));
        lbl_i_parts->setFont(font1);

        gridLayout->addWidget(lbl_i_parts, 2, 0, 1, 1);

        lbl_parts = new QLabel(frame_data);
        lbl_parts->setObjectName(QString::fromUtf8("lbl_parts"));
        lbl_parts->setFont(font1);

        gridLayout->addWidget(lbl_parts, 2, 1, 1, 1);

        lbl_i_mem = new QLabel(frame_data);
        lbl_i_mem->setObjectName(QString::fromUtf8("lbl_i_mem"));
        lbl_i_mem->setFont(font1);

        gridLayout->addWidget(lbl_i_mem, 3, 0, 1, 1);

        lbl_tree = new QLabel(frame_data);
        lbl_tree->setObjectName(QString::fromUtf8("lbl_tree"));
        lbl_tree->setFont(font1);

        gridLayout->addWidget(lbl_tree, 1, 1, 1, 1);


        horizontalLayout->addWidget(frame_data);


        verticalLayout->addWidget(frame_header);

        frame_middle = new QFrame(centralwidget);
        frame_middle->setObjectName(QString::fromUtf8("frame_middle"));
        frame_middle->setFrameShape(QFrame::NoFrame);
        frame_middle->setFrameShadow(QFrame::Raised);
        horizontalLayout_2 = new QHBoxLayout(frame_middle);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        splitter = new QSplitter(frame_middle);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Horizontal);
        frame_console = new QFrame(splitter);
        frame_console->setObjectName(QString::fromUtf8("frame_console"));
        frame_console->setFrameShape(QFrame::StyledPanel);
        frame_console->setFrameShadow(QFrame::Raised);
        verticalLayout_7 = new QVBoxLayout(frame_console);
        verticalLayout_7->setSpacing(0);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        verticalLayout_7->setContentsMargins(0, 0, 0, 0);
        splitter_2 = new QSplitter(frame_console);
        splitter_2->setObjectName(QString::fromUtf8("splitter_2"));
        splitter_2->setMinimumSize(QSize(610, 0));
        splitter_2->setOrientation(Qt::Vertical);
        frame_settings = new QScrollArea(splitter_2);
        frame_settings->setObjectName(QString::fromUtf8("frame_settings"));
        QFont font2;
        font2.setFamily(QString::fromUtf8("Arial"));
        font2.setPointSize(10);
        frame_settings->setFont(font2);
        frame_settings->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, -60, 821, 583));
        verticalLayout_9 = new QVBoxLayout(scrollAreaWidgetContents);
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        frameData = new QFrame(scrollAreaWidgetContents);
        frameData->setObjectName(QString::fromUtf8("frameData"));
        frameData->setMinimumSize(QSize(0, 100));
        frameData->setMaximumSize(QSize(16777215, 140));
        frameData->setFont(font1);
        frameData->setFrameShape(QFrame::NoFrame);
        frameData->setFrameShadow(QFrame::Plain);
        frameData->setLineWidth(0);
        verticalLayout_5 = new QVBoxLayout(frameData);
        verticalLayout_5->setSpacing(2);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        verticalLayout_5->setContentsMargins(2, 2, 2, 2);
        frame_12 = new QFrame(frameData);
        frame_12->setObjectName(QString::fromUtf8("frame_12"));
        frame_12->setMinimumSize(QSize(0, 20));
        frame_12->setMaximumSize(QSize(16777215, 30));
        frame_12->setFrameShape(QFrame::NoFrame);
        frame_12->setFrameShadow(QFrame::Raised);
        frame_12->setLineWidth(0);
        horizontalLayout_10 = new QHBoxLayout(frame_12);
        horizontalLayout_10->setObjectName(QString::fromUtf8("horizontalLayout_10"));
        horizontalLayout_10->setContentsMargins(-1, 0, -1, 0);
        label_8 = new QLabel(frame_12);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setMinimumSize(QSize(100, 0));
        QFont font3;
        font3.setFamily(QString::fromUtf8("Courier 10 Pitch"));
        font3.setPointSize(10);
        font3.setBold(false);
        font3.setWeight(50);
        label_8->setFont(font3);

        horizontalLayout_10->addWidget(label_8);

        radDatatypeDna = new QRadioButton(frame_12);
        radDatatypeDna->setObjectName(QString::fromUtf8("radDatatypeDna"));
        QFont font4;
        font4.setFamily(QString::fromUtf8("Courier 10 Pitch"));
        font4.setPointSize(10);
        font4.setBold(false);
        font4.setItalic(false);
        font4.setWeight(50);
        radDatatypeDna->setFont(font4);
        radDatatypeDna->setCursor(QCursor(Qt::PointingHandCursor));
        radDatatypeDna->setChecked(true);

        horizontalLayout_10->addWidget(radDatatypeDna);

        radDatatypeProt = new QRadioButton(frame_12);
        radDatatypeProt->setObjectName(QString::fromUtf8("radDatatypeProt"));
        radDatatypeProt->setFont(font4);
        radDatatypeProt->setCursor(QCursor(Qt::PointingHandCursor));

        horizontalLayout_10->addWidget(radDatatypeProt);

        horizontalSpacer_11 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_10->addItem(horizontalSpacer_11);


        verticalLayout_5->addWidget(frame_12);

        frame_settings_3 = new QFrame(frameData);
        frame_settings_3->setObjectName(QString::fromUtf8("frame_settings_3"));
        frame_settings_3->setMinimumSize(QSize(0, 20));
        frame_settings_3->setMaximumSize(QSize(16777215, 30));
        frame_settings_3->setFrameShape(QFrame::NoFrame);
        frame_settings_3->setFrameShadow(QFrame::Raised);
        horizontalLayout_14 = new QHBoxLayout(frame_settings_3);
        horizontalLayout_14->setObjectName(QString::fromUtf8("horizontalLayout_14"));
        horizontalLayout_14->setContentsMargins(-1, 0, -1, 0);
        label_9 = new QLabel(frame_settings_3);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setMinimumSize(QSize(100, 0));
        label_9->setFont(font3);

        horizontalLayout_14->addWidget(label_9);

        radSetModelTest = new QRadioButton(frame_settings_3);
        radSetModelTest->setObjectName(QString::fromUtf8("radSetModelTest"));
        radSetModelTest->setFont(font4);
        radSetModelTest->setStyleSheet(QString::fromUtf8("border: none;"));
        radSetModelTest->setChecked(true);

        horizontalLayout_14->addWidget(radSetModelTest);

        radSetMrbayes = new QRadioButton(frame_settings_3);
        radSetMrbayes->setObjectName(QString::fromUtf8("radSetMrbayes"));
        radSetMrbayes->setFont(font4);
        radSetMrbayes->setStyleSheet(QString::fromUtf8("border: none;"));

        horizontalLayout_14->addWidget(radSetMrbayes);

        radSetRaxml = new QRadioButton(frame_settings_3);
        radSetRaxml->setObjectName(QString::fromUtf8("radSetRaxml"));
        radSetRaxml->setFont(font4);
        radSetRaxml->setStyleSheet(QString::fromUtf8("border: none;"));

        horizontalLayout_14->addWidget(radSetRaxml);

        radSetPhyml = new QRadioButton(frame_settings_3);
        radSetPhyml->setObjectName(QString::fromUtf8("radSetPhyml"));
        radSetPhyml->setFont(font4);
        radSetPhyml->setStyleSheet(QString::fromUtf8("border: none;"));

        horizontalLayout_14->addWidget(radSetPhyml);

        radSetPAUP = new QRadioButton(frame_settings_3);
        radSetPAUP->setObjectName(QString::fromUtf8("radSetPAUP"));
        radSetPAUP->setFont(font4);
        radSetPAUP->setStyleSheet(QString::fromUtf8("border: none;"));

        horizontalLayout_14->addWidget(radSetPAUP);

        horizontalSpacer_12 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_14->addItem(horizontalSpacer_12);


        verticalLayout_5->addWidget(frame_settings_3);

        label_10 = new QLabel(frameData);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setMinimumSize(QSize(0, 20));
        label_10->setMaximumSize(QSize(16777215, 30));
        QFont font5;
        font5.setFamily(QString::fromUtf8("Courier 10 Pitch"));
        font5.setPointSize(10);
        font5.setBold(false);
        font5.setItalic(true);
        font5.setUnderline(true);
        font5.setWeight(50);
        font5.setKerning(true);
        label_10->setFont(font5);

        verticalLayout_5->addWidget(label_10);

        frame_settings_4 = new QFrame(frameData);
        frame_settings_4->setObjectName(QString::fromUtf8("frame_settings_4"));
        frame_settings_4->setMinimumSize(QSize(0, 20));
        frame_settings_4->setMaximumSize(QSize(16777215, 30));
        frame_settings_4->setFrameShape(QFrame::NoFrame);
        frame_settings_4->setFrameShadow(QFrame::Raised);
        frame_settings_4->setLineWidth(0);
        horizontalLayout_11 = new QHBoxLayout(frame_settings_4);
        horizontalLayout_11->setObjectName(QString::fromUtf8("horizontalLayout_11"));
        horizontalLayout_11->setContentsMargins(-1, 0, -1, 0);
        radTopoFixedMp = new QRadioButton(frame_settings_4);
        radTopoFixedMp->setObjectName(QString::fromUtf8("radTopoFixedMp"));
        radTopoFixedMp->setFont(font4);

        horizontalLayout_11->addWidget(radTopoFixedMp);

        radTopoFixedJc = new QRadioButton(frame_settings_4);
        radTopoFixedJc->setObjectName(QString::fromUtf8("radTopoFixedJc"));
        radTopoFixedJc->setFont(font4);

        horizontalLayout_11->addWidget(radTopoFixedJc);

        radTopoFixedGtr = new QRadioButton(frame_settings_4);
        radTopoFixedGtr->setObjectName(QString::fromUtf8("radTopoFixedGtr"));
        radTopoFixedGtr->setFont(font4);
        radTopoFixedGtr->setChecked(true);

        horizontalLayout_11->addWidget(radTopoFixedGtr);

        radTopoML = new QRadioButton(frame_settings_4);
        radTopoML->setObjectName(QString::fromUtf8("radTopoML"));
        radTopoML->setFont(font4);
        radTopoML->setChecked(false);

        horizontalLayout_11->addWidget(radTopoML);

        radTopoU = new QRadioButton(frame_settings_4);
        radTopoU->setObjectName(QString::fromUtf8("radTopoU"));
        radTopoU->setFont(font4);

        horizontalLayout_11->addWidget(radTopoU);

        horizontalSpacer_13 = new QSpacerItem(162, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_11->addItem(horizontalSpacer_13);


        verticalLayout_5->addWidget(frame_settings_4);


        verticalLayout_9->addWidget(frameData);

        line_3 = new QFrame(scrollAreaWidgetContents);
        line_3->setObjectName(QString::fromUtf8("line_3"));
        line_3->setMinimumSize(QSize(0, 10));
        line_3->setFrameShadow(QFrame::Sunken);
        line_3->setLineWidth(1);
        line_3->setFrameShape(QFrame::HLine);

        verticalLayout_9->addWidget(line_3);

        label_11 = new QLabel(scrollAreaWidgetContents);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setMinimumSize(QSize(0, 20));
        label_11->setMaximumSize(QSize(16777215, 20));
        QFont font6;
        font6.setFamily(QString::fromUtf8("Courier 10 Pitch"));
        font6.setPointSize(11);
        font6.setBold(true);
        font6.setItalic(true);
        font6.setUnderline(false);
        font6.setWeight(75);
        label_11->setFont(font6);
        label_11->setStyleSheet(QString::fromUtf8("border-bottom: 1px dotted"));

        verticalLayout_9->addWidget(label_11);

        frameModels = new QFrame(scrollAreaWidgetContents);
        frameModels->setObjectName(QString::fromUtf8("frameModels"));
        frameModels->setStyleSheet(QString::fromUtf8("background: #fee;"));
        verticalLayout_12 = new QVBoxLayout(frameModels);
        verticalLayout_12->setSpacing(2);
        verticalLayout_12->setObjectName(QString::fromUtf8("verticalLayout_12"));
        verticalLayout_12->setContentsMargins(2, 2, 2, 2);
        frame_models = new QFrame(frameModels);
        frame_models->setObjectName(QString::fromUtf8("frame_models"));
        frame_models->setMinimumSize(QSize(0, 270));
        frame_models->setFrameShape(QFrame::NoFrame);
        frame_models->setFrameShadow(QFrame::Plain);
        horizontalLayout_5 = new QHBoxLayout(frame_models);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        horizontalLayout_5->setContentsMargins(0, 0, 0, 0);
        frame_modeldefs = new QFrame(frame_models);
        frame_modeldefs->setObjectName(QString::fromUtf8("frame_modeldefs"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(frame_modeldefs->sizePolicy().hasHeightForWidth());
        frame_modeldefs->setSizePolicy(sizePolicy);
        frame_modeldefs->setMinimumSize(QSize(420, 250));
        frame_modeldefs->setMaximumSize(QSize(420, 16777215));
        frame_modeldefs->setFrameShape(QFrame::NoFrame);
        frame_modeldefs->setFrameShadow(QFrame::Plain);
        verticalLayout_3 = new QVBoxLayout(frame_modeldefs);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(2, 2, 2, 2);
        grpSubstSchemes = new QFrame(frame_modeldefs);
        grpSubstSchemes->setObjectName(QString::fromUtf8("grpSubstSchemes"));
        grpSubstSchemes->setEnabled(true);
        grpSubstSchemes->setMinimumSize(QSize(0, 20));
        grpSubstSchemes->setMaximumSize(QSize(16777215, 20));
        grpSubstSchemes->setFrameShape(QFrame::NoFrame);
        grpSubstSchemes->setFrameShadow(QFrame::Plain);
        grpSubstSchemes->setLineWidth(0);
        horizontalLayout_12 = new QHBoxLayout(grpSubstSchemes);
        horizontalLayout_12->setObjectName(QString::fromUtf8("horizontalLayout_12"));
        horizontalLayout_12->setContentsMargins(-1, 2, -1, 2);
        radSchemes3 = new QRadioButton(grpSubstSchemes);
        radSchemes3->setObjectName(QString::fromUtf8("radSchemes3"));
        radSchemes3->setFont(font4);

        horizontalLayout_12->addWidget(radSchemes3);

        radSchemes5 = new QRadioButton(grpSubstSchemes);
        radSchemes5->setObjectName(QString::fromUtf8("radSchemes5"));
        radSchemes5->setFont(font4);

        horizontalLayout_12->addWidget(radSchemes5);

        radSchemes7 = new QRadioButton(grpSubstSchemes);
        radSchemes7->setObjectName(QString::fromUtf8("radSchemes7"));
        radSchemes7->setFont(font4);

        horizontalLayout_12->addWidget(radSchemes7);

        radSchemes11 = new QRadioButton(grpSubstSchemes);
        radSchemes11->setObjectName(QString::fromUtf8("radSchemes11"));
        radSchemes11->setFont(font4);
        radSchemes11->setChecked(true);

        horizontalLayout_12->addWidget(radSchemes11);

        radSchemes203 = new QRadioButton(grpSubstSchemes);
        radSchemes203->setObjectName(QString::fromUtf8("radSchemes203"));
        radSchemes203->setFont(font4);
        radSchemes203->setChecked(false);

        horizontalLayout_12->addWidget(radSchemes203);

        radSchemesUser = new QRadioButton(grpSubstSchemes);
        radSchemesUser->setObjectName(QString::fromUtf8("radSchemesUser"));
        radSchemesUser->setFont(font4);
        radSchemesUser->setChecked(false);

        horizontalLayout_12->addWidget(radSchemesUser);

        horizontalSpacer_14 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_12->addItem(horizontalSpacer_14);


        verticalLayout_3->addWidget(grpSubstSchemes);

        label_12 = new QLabel(frame_modeldefs);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setMinimumSize(QSize(0, 20));
        label_12->setMaximumSize(QSize(16777215, 20));
        QFont font7;
        font7.setFamily(QString::fromUtf8("Courier 10 Pitch"));
        font7.setPointSize(10);
        font7.setBold(false);
        font7.setItalic(true);
        font7.setUnderline(true);
        font7.setWeight(50);
        label_12->setFont(font7);

        verticalLayout_3->addWidget(label_12);

        frame_13 = new QFrame(frame_modeldefs);
        frame_13->setObjectName(QString::fromUtf8("frame_13"));
        frame_13->setMinimumSize(QSize(0, 50));
        frame_13->setMaximumSize(QSize(16777215, 50));
        frame_13->setFrameShape(QFrame::NoFrame);
        frame_13->setFrameShadow(QFrame::Plain);
        frame_13->setLineWidth(0);
        gridLayout_7 = new QGridLayout(frame_13);
        gridLayout_7->setObjectName(QString::fromUtf8("gridLayout_7"));
        gridLayout_7->setContentsMargins(-1, 2, -1, 2);
        cbNoRateVarModels = new QCheckBox(frame_13);
        cbNoRateVarModels->setObjectName(QString::fromUtf8("cbNoRateVarModels"));
        cbNoRateVarModels->setMinimumSize(QSize(0, 22));
        cbNoRateVarModels->setMaximumSize(QSize(160, 16777215));
        cbNoRateVarModels->setFont(font4);
        cbNoRateVarModels->setChecked(true);

        gridLayout_7->addWidget(cbNoRateVarModels, 0, 0, 1, 1);

        cbIGModels = new QCheckBox(frame_13);
        cbIGModels->setObjectName(QString::fromUtf8("cbIGModels"));
        cbIGModels->setMinimumSize(QSize(0, 22));
        cbIGModels->setFont(font4);
        cbIGModels->setChecked(true);

        gridLayout_7->addWidget(cbIGModels, 0, 3, 1, 1);

        cbIModels = new QCheckBox(frame_13);
        cbIModels->setObjectName(QString::fromUtf8("cbIModels"));
        cbIModels->setMinimumSize(QSize(0, 22));
        cbIModels->setMaximumSize(QSize(180, 16777215));
        cbIModels->setFont(font4);
        cbIModels->setChecked(true);

        gridLayout_7->addWidget(cbIModels, 0, 2, 1, 1);

        horizontalSpacer_15 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_7->addItem(horizontalSpacer_15, 0, 6, 1, 1);

        lblNCat = new QLabel(frame_13);
        lblNCat->setObjectName(QString::fromUtf8("lblNCat"));
        lblNCat->setMinimumSize(QSize(100, 20));
        lblNCat->setMaximumSize(QSize(100, 20));
        lblNCat->setFont(font4);

        gridLayout_7->addWidget(lblNCat, 1, 2, 1, 1);

        cbGModels = new QCheckBox(frame_13);
        cbGModels->setObjectName(QString::fromUtf8("cbGModels"));
        cbGModels->setMinimumSize(QSize(0, 22));
        cbGModels->setMaximumSize(QSize(220, 16777215));
        cbGModels->setFont(font4);
        cbGModels->setChecked(true);

        gridLayout_7->addWidget(cbGModels, 0, 1, 1, 1);

        sliderNCat = new QSlider(frame_13);
        sliderNCat->setObjectName(QString::fromUtf8("sliderNCat"));
        sliderNCat->setMinimumSize(QSize(100, 20));
        sliderNCat->setMaximumSize(QSize(100, 20));
        sliderNCat->setFont(font4);
        sliderNCat->setAutoFillBackground(false);
        sliderNCat->setMinimum(2);
        sliderNCat->setMaximum(20);
        sliderNCat->setValue(4);
        sliderNCat->setOrientation(Qt::Horizontal);

        gridLayout_7->addWidget(sliderNCat, 1, 1, 1, 1);


        verticalLayout_3->addWidget(frame_13);

        label_13 = new QLabel(frame_modeldefs);
        label_13->setObjectName(QString::fromUtf8("label_13"));
        label_13->setMinimumSize(QSize(0, 20));
        label_13->setMaximumSize(QSize(16777215, 20));
        label_13->setFont(font7);

        verticalLayout_3->addWidget(label_13);

        frame_14 = new QFrame(frame_modeldefs);
        frame_14->setObjectName(QString::fromUtf8("frame_14"));
        frame_14->setMinimumSize(QSize(0, 25));
        frame_14->setMaximumSize(QSize(16777215, 25));
        frame_14->setFrameShape(QFrame::NoFrame);
        frame_14->setFrameShadow(QFrame::Plain);
        frame_14->setLineWidth(0);
        horizontalLayout_15 = new QHBoxLayout(frame_14);
        horizontalLayout_15->setObjectName(QString::fromUtf8("horizontalLayout_15"));
        horizontalLayout_15->setContentsMargins(-1, 2, -1, 2);
        cbEqualFreq = new QCheckBox(frame_14);
        cbEqualFreq->setObjectName(QString::fromUtf8("cbEqualFreq"));
        cbEqualFreq->setMinimumSize(QSize(180, 22));
        cbEqualFreq->setMaximumSize(QSize(180, 16777215));
        cbEqualFreq->setFont(font4);
        cbEqualFreq->setChecked(true);

        horizontalLayout_15->addWidget(cbEqualFreq);

        cbMlFreq = new QCheckBox(frame_14);
        cbMlFreq->setObjectName(QString::fromUtf8("cbMlFreq"));
        cbMlFreq->setMinimumSize(QSize(200, 22));
        cbMlFreq->setMaximumSize(QSize(200, 16777215));
        cbMlFreq->setFont(font4);
        cbMlFreq->setChecked(true);

        horizontalLayout_15->addWidget(cbMlFreq);

        horizontalSpacer_16 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_15->addItem(horizontalSpacer_16);


        verticalLayout_3->addWidget(frame_14);

        frame_nmodels = new QFrame(frame_modeldefs);
        frame_nmodels->setObjectName(QString::fromUtf8("frame_nmodels"));
        frame_nmodels->setMinimumSize(QSize(0, 30));
        frame_nmodels->setMaximumSize(QSize(16777215, 30));
        horizontalLayout_16 = new QHBoxLayout(frame_nmodels);
        horizontalLayout_16->setSpacing(0);
        horizontalLayout_16->setObjectName(QString::fromUtf8("horizontalLayout_16"));
        horizontalLayout_16->setContentsMargins(-1, 2, -1, 2);
        lblTextNModels = new QLabel(frame_nmodels);
        lblTextNModels->setObjectName(QString::fromUtf8("lblTextNModels"));
        lblTextNModels->setMinimumSize(QSize(150, 22));
        lblTextNModels->setMaximumSize(QSize(150, 22));
        lblTextNModels->setFont(font4);

        horizontalLayout_16->addWidget(lblTextNModels);

        lblNumModels = new QLabel(frame_nmodels);
        lblNumModels->setObjectName(QString::fromUtf8("lblNumModels"));
        lblNumModels->setMinimumSize(QSize(100, 22));
        lblNumModels->setMaximumSize(QSize(16777215, 22));
        lblNumModels->setFont(font4);

        horizontalLayout_16->addWidget(lblNumModels);

        horizontalSpacer_17 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_16->addItem(horizontalSpacer_17);


        verticalLayout_3->addWidget(frame_nmodels);

        cbShowMatrices = new QCheckBox(frame_modeldefs);
        cbShowMatrices->setObjectName(QString::fromUtf8("cbShowMatrices"));
        cbShowMatrices->setMinimumSize(QSize(0, 20));
        cbShowMatrices->setMaximumSize(QSize(16777215, 20));
        cbShowMatrices->setFont(font4);

        verticalLayout_3->addWidget(cbShowMatrices);

        grpSubstSchemes->raise();
        frame_nmodels->raise();
        frame_13->raise();
        frame_14->raise();
        label_13->raise();
        label_12->raise();
        cbShowMatrices->raise();

        horizontalLayout_5->addWidget(frame_modeldefs);

        modelsListView = new QListWidget(frame_models);
        modelsListView->setObjectName(QString::fromUtf8("modelsListView"));
        modelsListView->setMinimumSize(QSize(220, 0));
        modelsListView->setMaximumSize(QSize(220, 250));
        QFont font8;
        font8.setFamily(QString::fromUtf8("Courier"));
        font8.setPointSize(10);
        font8.setBold(true);
        font8.setWeight(75);
        modelsListView->setFont(font8);
        modelsListView->setFrameShape(QFrame::NoFrame);
        modelsListView->setFrameShadow(QFrame::Plain);
        modelsListView->setLineWidth(0);
        modelsListView->setSelectionMode(QAbstractItemView::NoSelection);

        horizontalLayout_5->addWidget(modelsListView);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_3);


        verticalLayout_12->addWidget(frame_models);


        verticalLayout_9->addWidget(frameModels);

        line_4 = new QFrame(scrollAreaWidgetContents);
        line_4->setObjectName(QString::fromUtf8("line_4"));
        line_4->setFrameShadow(QFrame::Sunken);
        line_4->setFrameShape(QFrame::HLine);

        verticalLayout_9->addWidget(line_4);

        cbAdvanced = new QCheckBox(scrollAreaWidgetContents);
        cbAdvanced->setObjectName(QString::fromUtf8("cbAdvanced"));
        cbAdvanced->setMinimumSize(QSize(205, 22));
        cbAdvanced->setMaximumSize(QSize(16777215, 22));
        QFont font9;
        font9.setFamily(QString::fromUtf8("Courier"));
        font9.setPointSize(10);
        font9.setBold(true);
        font9.setItalic(true);
        font9.setWeight(75);
        cbAdvanced->setFont(font9);

        verticalLayout_9->addWidget(cbAdvanced);

        frame_advanced = new QFrame(scrollAreaWidgetContents);
        frame_advanced->setObjectName(QString::fromUtf8("frame_advanced"));
        frame_advanced->setMinimumSize(QSize(0, 100));
        frame_advanced->setMaximumSize(QSize(16777215, 135));
        frame_advanced->setFont(font2);
        frame_advanced->setStyleSheet(QString::fromUtf8("background-color: #99d;"));
        gridLayout_3 = new QGridLayout(frame_advanced);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        gridLayout_3->setVerticalSpacing(0);
        gridLayout_3->setContentsMargins(-1, 0, -1, 0);
        txtOptEpsilon = new QLineEdit(frame_advanced);
        txtOptEpsilon->setObjectName(QString::fromUtf8("txtOptEpsilon"));
        txtOptEpsilon->setMinimumSize(QSize(0, 20));
        txtOptEpsilon->setMaximumSize(QSize(150, 25));
        QFont font10;
        font10.setFamily(QString::fromUtf8("Courier"));
        font10.setPointSize(10);
        font10.setBold(true);
        font10.setItalic(false);
        font10.setWeight(75);
        txtOptEpsilon->setFont(font10);
        txtOptEpsilon->setText(QString::fromUtf8("0.01"));
        txtOptEpsilon->setMaxLength(10);
        txtOptEpsilon->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_3->addWidget(txtOptEpsilon, 2, 1, 1, 1);

        txtParEpsilon = new QLineEdit(frame_advanced);
        txtParEpsilon->setObjectName(QString::fromUtf8("txtParEpsilon"));
        txtParEpsilon->setMinimumSize(QSize(0, 20));
        txtParEpsilon->setMaximumSize(QSize(150, 25));
        txtParEpsilon->setFont(font10);
        txtParEpsilon->setText(QString::fromUtf8("0.001"));
        txtParEpsilon->setMaxLength(10);
        txtParEpsilon->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_3->addWidget(txtParEpsilon, 1, 1, 1, 1);

        lblParEpsText = new QLabel(frame_advanced);
        lblParEpsText->setObjectName(QString::fromUtf8("lblParEpsText"));
        lblParEpsText->setMinimumSize(QSize(0, 20));
        lblParEpsText->setMaximumSize(QSize(16777215, 25));
        lblParEpsText->setFont(font10);

        gridLayout_3->addWidget(lblParEpsText, 1, 0, 1, 1);

        lblOptEpsText = new QLabel(frame_advanced);
        lblOptEpsText->setObjectName(QString::fromUtf8("lblOptEpsText"));
        lblOptEpsText->setMinimumSize(QSize(0, 20));
        lblOptEpsText->setMaximumSize(QSize(16777215, 25));
        lblOptEpsText->setFont(font10);

        gridLayout_3->addWidget(lblOptEpsText, 2, 0, 1, 1);

        lblNThreads = new QLabel(frame_advanced);
        lblNThreads->setObjectName(QString::fromUtf8("lblNThreads"));
        lblNThreads->setMinimumSize(QSize(100, 20));
        lblNThreads->setMaximumSize(QSize(16777215, 25));
        lblNThreads->setFont(font8);

        gridLayout_3->addWidget(lblNThreads, 0, 2, 1, 1);

        sliderNThreads = new QSlider(frame_advanced);
        sliderNThreads->setObjectName(QString::fromUtf8("sliderNThreads"));
        sliderNThreads->setMinimumSize(QSize(150, 20));
        sliderNThreads->setMaximumSize(QSize(150, 25));
        sliderNThreads->setFont(font2);
        sliderNThreads->setAutoFillBackground(false);
        sliderNThreads->setMinimum(1);
        sliderNThreads->setMaximum(10);
        sliderNThreads->setValue(2);
        sliderNThreads->setOrientation(Qt::Horizontal);

        gridLayout_3->addWidget(sliderNThreads, 0, 1, 1, 1);

        lblNThreadText = new QLabel(frame_advanced);
        lblNThreadText->setObjectName(QString::fromUtf8("lblNThreadText"));
        lblNThreadText->setMinimumSize(QSize(0, 20));
        lblNThreadText->setMaximumSize(QSize(16777215, 25));
        lblNThreadText->setFont(font10);

        gridLayout_3->addWidget(lblNThreadText, 0, 0, 1, 1);

        horizontalSpacer_18 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_3->addItem(horizontalSpacer_18, 1, 2, 1, 1);


        verticalLayout_9->addWidget(frame_advanced);

        frame_settings->setWidget(scrollAreaWidgetContents);
        splitter_2->addWidget(frame_settings);
        grpConsoles = new QFrame(splitter_2);
        grpConsoles->setObjectName(QString::fromUtf8("grpConsoles"));
        grpConsoles->setMinimumSize(QSize(0, 40));
        grpConsoles->setFrameShape(QFrame::StyledPanel);
        grpConsoles->setFrameShadow(QFrame::Raised);
        verticalLayout_4 = new QVBoxLayout(grpConsoles);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        consoleRun = new QTextBrowser(grpConsoles);
        consoleRun->setObjectName(QString::fromUtf8("consoleRun"));
        consoleRun->setMinimumSize(QSize(600, 0));
        consoleRun->setMaximumSize(QSize(16777215, 16777215));
        consoleRun->setBaseSize(QSize(0, 0));
        QFont font11;
        font11.setFamily(QString::fromUtf8("Courier 10 Pitch"));
        font11.setPointSize(9);
        consoleRun->setFont(font11);
        consoleRun->setLineWrapMode(QTextEdit::NoWrap);

        verticalLayout_4->addWidget(consoleRun);

        splitter_2->addWidget(grpConsoles);

        verticalLayout_7->addWidget(splitter_2);

        splitter->addWidget(frame_console);

        horizontalLayout_2->addWidget(splitter);


        verticalLayout->addWidget(frame_middle);

        xmodeltest->setCentralWidget(centralwidget);
        menubar = new QMenuBar(xmodeltest);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 877, 25));
        menubar->setStyleSheet(QString::fromUtf8(""));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuFile->setAutoFillBackground(false);
        menuFile->setStyleSheet(QString::fromUtf8(""));
        menuEdit = new QMenu(menubar);
        menuEdit->setObjectName(QString::fromUtf8("menuEdit"));
        menuRun = new QMenu(menubar);
        menuRun->setObjectName(QString::fromUtf8("menuRun"));
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        menuView = new QMenu(menubar);
        menuView->setObjectName(QString::fromUtf8("menuView"));
        xmodeltest->setMenuBar(menubar);
        statusbar = new QStatusBar(xmodeltest);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        xmodeltest->setStatusBar(statusbar);
        toolBar = new QToolBar(xmodeltest);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        toolBar->setStyleSheet(QString::fromUtf8(""));
        toolBar->setIconSize(QSize(23, 24));
        xmodeltest->addToolBar(Qt::TopToolBarArea, toolBar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuEdit->menuAction());
        menubar->addAction(menuRun->menuAction());
        menubar->addAction(menuView->menuAction());
        menubar->addAction(menuHelp->menuAction());
        menuFile->addAction(mnu_open_msa);
        menuFile->addAction(mnu_open_tree);
        menuFile->addAction(mnu_open_parts);
        menuFile->addAction(mnu_reset);
        menuEdit->addAction(mnu_copy);
        menuRun->addAction(mnu_toggle_settings);
        menuRun->addAction(mnu_run);
        menuHelp->addAction(mnu_index);
        menuHelp->addAction(mnu_about);
        menuView->addAction(mnu_view_datainfo);
        menuView->addAction(mnu_models);
        menuView->addAction(mnu_results);
        toolBar->addAction(act_open_msa);
        toolBar->addAction(act_open_tree);
        toolBar->addAction(act_open_parts);
        toolBar->addSeparator();
        toolBar->addAction(act_toggle_settings);
        toolBar->addAction(act_run);
        toolBar->addAction(act_results);
        toolBar->addAction(act_reset);

        retranslateUi(xmodeltest);

        QMetaObject::connectSlotsByName(xmodeltest);
    } // setupUi

    void retranslateUi(QMainWindow *xmodeltest)
    {
        act_open_msa->setText(QApplication::translate("xmodeltest", "Test", 0, QApplication::UnicodeUTF8));
        act_open_tree->setText(QApplication::translate("xmodeltest", "Load tree", 0, QApplication::UnicodeUTF8));
        act_open_parts->setText(QApplication::translate("xmodeltest", "Load Partitions", 0, QApplication::UnicodeUTF8));
        mnu_open_msa->setText(QApplication::translate("xmodeltest", "Load MSA", 0, QApplication::UnicodeUTF8));
        mnu_open_msa->setShortcut(QApplication::translate("xmodeltest", "Ctrl+O", 0, QApplication::UnicodeUTF8));
        mnu_open_tree->setText(QApplication::translate("xmodeltest", "Load Tree", 0, QApplication::UnicodeUTF8));
        mnu_open_tree->setShortcut(QApplication::translate("xmodeltest", "Ctrl+T", 0, QApplication::UnicodeUTF8));
        mnu_open_parts->setText(QApplication::translate("xmodeltest", "Load Partitions", 0, QApplication::UnicodeUTF8));
        mnu_open_parts->setShortcut(QApplication::translate("xmodeltest", "Ctrl+P", 0, QApplication::UnicodeUTF8));
        act_toggle_settings->setText(QApplication::translate("xmodeltest", "Settings", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        act_toggle_settings->setToolTip(QApplication::translate("xmodeltest", "Settings", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        mnu_toggle_settings->setText(QApplication::translate("xmodeltest", "Toggle Settings", 0, QApplication::UnicodeUTF8));
        mnu_toggle_settings->setShortcut(QApplication::translate("xmodeltest", "Alt+S", 0, QApplication::UnicodeUTF8));
        act_run->setText(QApplication::translate("xmodeltest", "Run", 0, QApplication::UnicodeUTF8));
        act_results->setText(QApplication::translate("xmodeltest", "Results", 0, QApplication::UnicodeUTF8));
        act_reset->setText(QApplication::translate("xmodeltest", "Reset", 0, QApplication::UnicodeUTF8));
        mnu_copy->setText(QApplication::translate("xmodeltest", "Copy", 0, QApplication::UnicodeUTF8));
        mnu_about->setText(QApplication::translate("xmodeltest", "About", 0, QApplication::UnicodeUTF8));
        mnu_index->setText(QApplication::translate("xmodeltest", "Index", 0, QApplication::UnicodeUTF8));
        mnu_reset->setText(QApplication::translate("xmodeltest", "Reset", 0, QApplication::UnicodeUTF8));
        mnu_reset->setShortcut(QApplication::translate("xmodeltest", "Alt+R", 0, QApplication::UnicodeUTF8));
        mnu_run->setText(QApplication::translate("xmodeltest", "Run", 0, QApplication::UnicodeUTF8));
        mnu_run->setShortcut(QApplication::translate("xmodeltest", "Ctrl+R", 0, QApplication::UnicodeUTF8));
        mnu_results->setText(QApplication::translate("xmodeltest", "Results", 0, QApplication::UnicodeUTF8));
        mnu_results->setShortcut(QApplication::translate("xmodeltest", "Shift+R", 0, QApplication::UnicodeUTF8));
        mnu_view_datainfo->setText(QApplication::translate("xmodeltest", "Data info", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        mnu_view_datainfo->setToolTip(QApplication::translate("xmodeltest", "View information about the input data", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        mnu_view_datainfo->setShortcut(QApplication::translate("xmodeltest", "Shift+I", 0, QApplication::UnicodeUTF8));
        mnu_models->setText(QApplication::translate("xmodeltest", "Models", 0, QApplication::UnicodeUTF8));
        mnu_models->setShortcut(QApplication::translate("xmodeltest", "Shift+M", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        tool_open_msa->setToolTip(QApplication::translate("xmodeltest", "Load MSA file (FASTA)", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        tool_open_msa->setStatusTip(QApplication::translate("xmodeltest", "Load MSA file (FASTA)", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        tool_open_msa->setText(QApplication::translate("xmodeltest", "msa", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        tool_open_tree->setToolTip(QApplication::translate("xmodeltest", "Load tree (NEWICK)", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        tool_open_tree->setStatusTip(QApplication::translate("xmodeltest", "Load tree (NEWICK)", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        tool_open_tree->setText(QApplication::translate("xmodeltest", "tree", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        tool_open_parts->setToolTip(QApplication::translate("xmodeltest", "Load partitions", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        tool_open_parts->setStatusTip(QApplication::translate("xmodeltest", "Load partitions", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        tool_open_parts->setText(QApplication::translate("xmodeltest", "partitions", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        tool_results->setToolTip(QApplication::translate("xmodeltest", "View results", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        tool_results->setStatusTip(QApplication::translate("xmodeltest", "View results", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        tool_results->setText(QApplication::translate("xmodeltest", "results", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        tool_settings->setToolTip(QApplication::translate("xmodeltest", "Configure model selection", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        tool_settings->setStatusTip(QApplication::translate("xmodeltest", "Configure model selection", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        tool_settings->setText(QApplication::translate("xmodeltest", "settings", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        tool_run->setToolTip(QApplication::translate("xmodeltest", "Run model selection", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        tool_run->setStatusTip(QApplication::translate("xmodeltest", "Run model selection", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        tool_run->setText(QApplication::translate("xmodeltest", "run", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        tool_reset->setToolTip(QApplication::translate("xmodeltest", "Reset everything", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        tool_reset->setStatusTip(QApplication::translate("xmodeltest", "Reset everything", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        tool_reset->setText(QApplication::translate("xmodeltest", "reset", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        tool_help->setToolTip(QApplication::translate("xmodeltest", "Help me, please", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        tool_help->setStatusTip(QApplication::translate("xmodeltest", "Help me, please", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        tool_help->setText(QApplication::translate("xmodeltest", "help!", 0, QApplication::UnicodeUTF8));
        mt_icon->setText(QString());
        lbl_i_tree->setText(QApplication::translate("xmodeltest", "Tree:", 0, QApplication::UnicodeUTF8));
        lbl_msa->setText(QApplication::translate("xmodeltest", "-", 0, QApplication::UnicodeUTF8));
        lbl_i_msa->setText(QApplication::translate("xmodeltest", "MSA:", 0, QApplication::UnicodeUTF8));
        lbl_mem->setText(QApplication::translate("xmodeltest", "-", 0, QApplication::UnicodeUTF8));
        lbl_i_parts->setText(QApplication::translate("xmodeltest", "Partitions:", 0, QApplication::UnicodeUTF8));
        lbl_parts->setText(QApplication::translate("xmodeltest", "-", 0, QApplication::UnicodeUTF8));
        lbl_i_mem->setText(QApplication::translate("xmodeltest", "Memory:", 0, QApplication::UnicodeUTF8));
        lbl_tree->setText(QApplication::translate("xmodeltest", "-", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("xmodeltest", "Data type:", 0, QApplication::UnicodeUTF8));
        radDatatypeDna->setText(QApplication::translate("xmodeltest", "DNA", 0, QApplication::UnicodeUTF8));
        radDatatypeProt->setText(QApplication::translate("xmodeltest", "Protein", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("xmodeltest", "Templates:", 0, QApplication::UnicodeUTF8));
        radSetModelTest->setText(QApplication::translate("xmodeltest", "ModelTest", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        radSetMrbayes->setToolTip(QApplication::translate("xmodeltest", "<html><head/><body><p>Select default options for MrBAYES</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        radSetMrbayes->setText(QApplication::translate("xmodeltest", "MrBAYES", 0, QApplication::UnicodeUTF8));
        radSetRaxml->setText(QApplication::translate("xmodeltest", "RAxML", 0, QApplication::UnicodeUTF8));
        radSetPhyml->setText(QApplication::translate("xmodeltest", "PhyML", 0, QApplication::UnicodeUTF8));
        radSetPAUP->setText(QApplication::translate("xmodeltest", "PAUP*", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("xmodeltest", "Starting topology", 0, QApplication::UnicodeUTF8));
        radTopoFixedMp->setText(QApplication::translate("xmodeltest", "Fixed MP", 0, QApplication::UnicodeUTF8));
        radTopoFixedJc->setText(QApplication::translate("xmodeltest", "Fixed ML (JC)", 0, QApplication::UnicodeUTF8));
        radTopoFixedGtr->setText(QApplication::translate("xmodeltest", "Fixed ML (GTR)", 0, QApplication::UnicodeUTF8));
        radTopoML->setText(QApplication::translate("xmodeltest", "ML", 0, QApplication::UnicodeUTF8));
        radTopoU->setText(QApplication::translate("xmodeltest", "Custom", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("xmodeltest", "Models", 0, QApplication::UnicodeUTF8));
        radSchemes3->setText(QApplication::translate("xmodeltest", "3", 0, QApplication::UnicodeUTF8));
        radSchemes5->setText(QApplication::translate("xmodeltest", "5", 0, QApplication::UnicodeUTF8));
        radSchemes7->setText(QApplication::translate("xmodeltest", "7", 0, QApplication::UnicodeUTF8));
        radSchemes11->setText(QApplication::translate("xmodeltest", "11", 0, QApplication::UnicodeUTF8));
        radSchemes203->setText(QApplication::translate("xmodeltest", "203", 0, QApplication::UnicodeUTF8));
        radSchemesUser->setText(QApplication::translate("xmodeltest", "User defined", 0, QApplication::UnicodeUTF8));
        label_12->setText(QApplication::translate("xmodeltest", "Rate variation", 0, QApplication::UnicodeUTF8));
        cbNoRateVarModels->setText(QApplication::translate("xmodeltest", "uniform", 0, QApplication::UnicodeUTF8));
        cbIGModels->setText(QApplication::translate("xmodeltest", "+I+G", 0, QApplication::UnicodeUTF8));
        cbIModels->setText(QApplication::translate("xmodeltest", "pinv (+I)", 0, QApplication::UnicodeUTF8));
        lblNCat->setText(QApplication::translate("xmodeltest", "4 categories", 0, QApplication::UnicodeUTF8));
        cbGModels->setText(QApplication::translate("xmodeltest", "gamma (+G)", 0, QApplication::UnicodeUTF8));
        label_13->setText(QApplication::translate("xmodeltest", "Frequencies", 0, QApplication::UnicodeUTF8));
        cbEqualFreq->setText(QApplication::translate("xmodeltest", "Equal frequencies", 0, QApplication::UnicodeUTF8));
        cbMlFreq->setText(QApplication::translate("xmodeltest", "ML frequencies", 0, QApplication::UnicodeUTF8));
        lblTextNModels->setText(QApplication::translate("xmodeltest", "Number of models:", 0, QApplication::UnicodeUTF8));
        lblNumModels->setText(QApplication::translate("xmodeltest", "0", 0, QApplication::UnicodeUTF8));
        cbShowMatrices->setText(QApplication::translate("xmodeltest", "Show model matrices", 0, QApplication::UnicodeUTF8));
        cbAdvanced->setText(QApplication::translate("xmodeltest", "Show advanced options", 0, QApplication::UnicodeUTF8));
        lblParEpsText->setText(QApplication::translate("xmodeltest", "Parameter epsilon:", 0, QApplication::UnicodeUTF8));
        lblOptEpsText->setText(QApplication::translate("xmodeltest", "Optimization epsilon:", 0, QApplication::UnicodeUTF8));
        lblNThreads->setText(QApplication::translate("xmodeltest", "4", 0, QApplication::UnicodeUTF8));
        lblNThreadText->setText(QApplication::translate("xmodeltest", "Number of threads:", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("xmodeltest", "File", 0, QApplication::UnicodeUTF8));
        menuEdit->setTitle(QApplication::translate("xmodeltest", "Edit", 0, QApplication::UnicodeUTF8));
        menuRun->setTitle(QApplication::translate("xmodeltest", "Run", 0, QApplication::UnicodeUTF8));
        menuHelp->setTitle(QApplication::translate("xmodeltest", "Help", 0, QApplication::UnicodeUTF8));
        menuView->setTitle(QApplication::translate("xmodeltest", "View", 0, QApplication::UnicodeUTF8));
        toolBar->setWindowTitle(QApplication::translate("xmodeltest", "toolBar", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(xmodeltest);
    } // retranslateUi

};

namespace Ui {
    class xmodeltest: public Ui_xmodeltest {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_XMODELTEST_H
