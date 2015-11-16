/********************************************************************************
** Form generated from reading UI file 'modeltest_gui.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MODELTEST_GUI_H
#define UI_MODELTEST_GUI_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QScrollArea>
#include <QtGui/QSlider>
#include <QtGui/QSpacerItem>
#include <QtGui/QSplitter>
#include <QtGui/QStatusBar>
#include <QtGui/QTabWidget>
#include <QtGui/QTableView>
#include <QtGui/QTextBrowser>
#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_jModelTest
{
public:
    QAction *menuFileLoad;
    QAction *actionQuit;
    QAction *menuFileQuit;
    QAction *menuSaveData;
    QAction *actionReset;
    QAction *menuTreeLoad;
    QAction *actionNewMenu;
    QAction *actionConsole;
    QAction *actionConfigure;
    QAction *actionProgress;
    QAction *actionResults;
    QWidget *layout_main;
    QVBoxLayout *verticalLayout_2;
    QGroupBox *groupBox_3;
    QHBoxLayout *horizontalLayout_8;
    QLabel *lblGear;
    QGridLayout *gridLayout_5;
    QLabel *lblAlignment;
    QLabel *lblTree;
    QLabel *lblLoadAlignText;
    QLabel *lblParts;
    QLabel *lblLoadTreeText;
    QLabel *lblLoadPartsText;
    QPushButton *btnLoadParts;
    QPushButton *btnLoadTree;
    QPushButton *btnLoadAlignment;
    QSplitter *splitter;
    QWidget *widget_mid;
    QHBoxLayout *horizontalLayout_11;
    QTabWidget *tabView;
    QWidget *tab_configure;
    QVBoxLayout *verticalLayout_10;
    QScrollArea *grpOptions;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout_8;
    QGroupBox *grpSetOptions;
    QHBoxLayout *horizontalLayout_4;
    QRadioButton *radSetModelTest;
    QRadioButton *radSetMrbayes;
    QRadioButton *radSetRaxml;
    QRadioButton *radSetPhyml;
    QRadioButton *radSetPAUP;
    QSpacerItem *horizontalSpacer_2;
    QGroupBox *grpTopology;
    QHBoxLayout *horizontalLayout_5;
    QRadioButton *radTopoFixedMp;
    QRadioButton *radTopoFixedJc;
    QRadioButton *radTopoFixedGtr;
    QRadioButton *radTopoML;
    QRadioButton *radTopoU;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_9;
    QGroupBox *grpDataType;
    QRadioButton *radDatatypeDna;
    QRadioButton *radDatatypeProt;
    QGroupBox *grpSubstSchemes;
    QRadioButton *radSchemes3;
    QRadioButton *radSchemes5;
    QRadioButton *radSchemes7;
    QRadioButton *radSchemes11;
    QRadioButton *radSchemes203;
    QRadioButton *radSchemesUser;
    QHBoxLayout *horizontalLayout_3;
    QLabel *lblTextNModels;
    QLabel *lblNumModels;
    QCheckBox *cbShowMatrices;
    QSpacerItem *horizontalSpacer_3;
    QHBoxLayout *horizontalLayout_7;
    QListWidget *listMatrices;
    QGroupBox *grpModelSets;
    QGridLayout *gridLayout;
    QCheckBox *cbGModels;
    QCheckBox *cbNoRateVarModels;
    QSlider *sliderNCat;
    QLabel *lblNCat;
    QCheckBox *cbIModels;
    QCheckBox *cbIGModels;
    QCheckBox *cbEqualFreq;
    QCheckBox *cbMlFreq;
    QCheckBox *cbAdvanced;
    QGroupBox *grpAdvanced;
    QGridLayout *gridLayout_2;
    QLineEdit *txtOptEpsilon;
    QLineEdit *txtParEpsilon;
    QLabel *lblParEpsText;
    QLabel *lblOptEpsText;
    QLabel *lblNThreads;
    QSlider *sliderNThreads;
    QLabel *lblNThreadText;
    QSpacerItem *horizontalSpacer_4;
    QVBoxLayout *verticalLayout_9;
    QGridLayout *gridLayout_3;
    QLabel *lblTextEstimatedMem2;
    QLabel *lblEstimatedMem;
    QLabel *lblTextEstimatedMem;
    QGridLayout *gridLayout_4;
    QPushButton *btnResetConfig;
    QPushButton *btnRun;
    QWidget *tab_runsingle;
    QVBoxLayout *verticalLayout_3;
    QGroupBox *groupBox_4;
    QCheckBox *cbShowHetParams;
    QCheckBox *cbShowRates;
    QCheckBox *cbShowFreqs;
    QCheckBox *cbShowSelection;
    QTableView *tblModels;
    QWidget *tab_results;
    QHBoxLayout *horizontalLayout;
    QTabWidget *results_content;
    QWidget *tabResultsBic;
    QVBoxLayout *verticalLayout_4;
    QTableView *tblResultsBic;
    QWidget *tabResultsAic;
    QVBoxLayout *verticalLayout_5;
    QTableView *tblResultsAic;
    QWidget *tabResultsAicc;
    QVBoxLayout *verticalLayout_6;
    QTableView *tblResultsAicc;
    QWidget *tabResultsDt;
    QVBoxLayout *verticalLayout_7;
    QTableView *tblResultsDt;
    QWidget *tab_console;
    QHBoxLayout *horizontalLayout_2;
    QWidget *widget_footer;
    QHBoxLayout *horizontalLayout_10;
    QTextBrowser *txt_messages;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuWhat_is_this;
    QMenu *menuWindow;
    QMenu *menuTab;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *jModelTest)
    {
        if (jModelTest->objectName().isEmpty())
            jModelTest->setObjectName(QString::fromUtf8("jModelTest"));
        jModelTest->resize(989, 967);
        jModelTest->setMinimumSize(QSize(500, 500));
        QFont font;
        font.setFamily(QString::fromUtf8("Arial"));
        jModelTest->setFont(font);
        QIcon icon;
        icon.addFile(QString::fromUtf8("IMG/mt-icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        jModelTest->setWindowIcon(icon);
        jModelTest->setAutoFillBackground(false);
        jModelTest->setStyleSheet(QString::fromUtf8("background-color: #EEE;\n"
"color: #222;"));
        menuFileLoad = new QAction(jModelTest);
        menuFileLoad->setObjectName(QString::fromUtf8("menuFileLoad"));
        menuFileLoad->setFont(font);
        actionQuit = new QAction(jModelTest);
        actionQuit->setObjectName(QString::fromUtf8("actionQuit"));
        menuFileQuit = new QAction(jModelTest);
        menuFileQuit->setObjectName(QString::fromUtf8("menuFileQuit"));
        menuFileQuit->setFont(font);
        menuSaveData = new QAction(jModelTest);
        menuSaveData->setObjectName(QString::fromUtf8("menuSaveData"));
        menuSaveData->setFont(font);
        actionReset = new QAction(jModelTest);
        actionReset->setObjectName(QString::fromUtf8("actionReset"));
        actionReset->setFont(font);
        menuTreeLoad = new QAction(jModelTest);
        menuTreeLoad->setObjectName(QString::fromUtf8("menuTreeLoad"));
        menuTreeLoad->setFont(font);
        actionNewMenu = new QAction(jModelTest);
        actionNewMenu->setObjectName(QString::fromUtf8("actionNewMenu"));
        actionNewMenu->setFont(font);
        actionConsole = new QAction(jModelTest);
        actionConsole->setObjectName(QString::fromUtf8("actionConsole"));
        actionConfigure = new QAction(jModelTest);
        actionConfigure->setObjectName(QString::fromUtf8("actionConfigure"));
        actionProgress = new QAction(jModelTest);
        actionProgress->setObjectName(QString::fromUtf8("actionProgress"));
        actionResults = new QAction(jModelTest);
        actionResults->setObjectName(QString::fromUtf8("actionResults"));
        layout_main = new QWidget(jModelTest);
        layout_main->setObjectName(QString::fromUtf8("layout_main"));
        verticalLayout_2 = new QVBoxLayout(layout_main);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        groupBox_3 = new QGroupBox(layout_main);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setMinimumSize(QSize(0, 130));
        groupBox_3->setMaximumSize(QSize(16777215, 130));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Arial"));
        font1.setPointSize(10);
        groupBox_3->setFont(font1);
        groupBox_3->setStyleSheet(QString::fromUtf8("background: #aaa;"));
        horizontalLayout_8 = new QHBoxLayout(groupBox_3);
        horizontalLayout_8->setSpacing(6);
        horizontalLayout_8->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        horizontalLayout_8->setContentsMargins(0, 0, -1, 10);
        lblGear = new QLabel(groupBox_3);
        lblGear->setObjectName(QString::fromUtf8("lblGear"));
        lblGear->setMinimumSize(QSize(100, 100));
        lblGear->setMaximumSize(QSize(100, 100));
        lblGear->setFont(font1);
        lblGear->setStyleSheet(QString::fromUtf8("background: #fff;"));
        lblGear->setFrameShape(QFrame::NoFrame);
        lblGear->setFrameShadow(QFrame::Plain);

        horizontalLayout_8->addWidget(lblGear);

        gridLayout_5 = new QGridLayout();
        gridLayout_5->setSpacing(6);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        lblAlignment = new QLabel(groupBox_3);
        lblAlignment->setObjectName(QString::fromUtf8("lblAlignment"));
        QFont font2;
        font2.setFamily(QString::fromUtf8("Arial"));
        font2.setPointSize(10);
        font2.setBold(false);
        font2.setItalic(false);
        font2.setWeight(50);
        lblAlignment->setFont(font2);
        lblAlignment->setStyleSheet(QString::fromUtf8("background: #99d;"));
        lblAlignment->setMargin(5);

        gridLayout_5->addWidget(lblAlignment, 0, 1, 1, 1);

        lblTree = new QLabel(groupBox_3);
        lblTree->setObjectName(QString::fromUtf8("lblTree"));
        lblTree->setFont(font2);
        lblTree->setStyleSheet(QString::fromUtf8("background: #99d;"));
        lblTree->setMargin(5);

        gridLayout_5->addWidget(lblTree, 1, 1, 1, 1);

        lblLoadAlignText = new QLabel(groupBox_3);
        lblLoadAlignText->setObjectName(QString::fromUtf8("lblLoadAlignText"));
        lblLoadAlignText->setMaximumSize(QSize(70, 16777215));
        lblLoadAlignText->setFont(font2);
        lblLoadAlignText->setStyleSheet(QString::fromUtf8("color: #060;"));

        gridLayout_5->addWidget(lblLoadAlignText, 0, 0, 1, 1);

        lblParts = new QLabel(groupBox_3);
        lblParts->setObjectName(QString::fromUtf8("lblParts"));
        lblParts->setFont(font2);
        lblParts->setStyleSheet(QString::fromUtf8("background: #99d;"));
        lblParts->setMargin(5);

        gridLayout_5->addWidget(lblParts, 2, 1, 1, 1);

        lblLoadTreeText = new QLabel(groupBox_3);
        lblLoadTreeText->setObjectName(QString::fromUtf8("lblLoadTreeText"));
        lblLoadTreeText->setMaximumSize(QSize(70, 16777215));
        lblLoadTreeText->setFont(font2);
        lblLoadTreeText->setStyleSheet(QString::fromUtf8("color: #060;"));

        gridLayout_5->addWidget(lblLoadTreeText, 1, 0, 1, 1);

        lblLoadPartsText = new QLabel(groupBox_3);
        lblLoadPartsText->setObjectName(QString::fromUtf8("lblLoadPartsText"));
        lblLoadPartsText->setMaximumSize(QSize(70, 16777215));
        lblLoadPartsText->setFont(font2);
        lblLoadPartsText->setStyleSheet(QString::fromUtf8("color: #060;"));

        gridLayout_5->addWidget(lblLoadPartsText, 2, 0, 1, 1);

        btnLoadParts = new QPushButton(groupBox_3);
        btnLoadParts->setObjectName(QString::fromUtf8("btnLoadParts"));
        btnLoadParts->setEnabled(true);
        btnLoadParts->setMinimumSize(QSize(30, 0));
        btnLoadParts->setMaximumSize(QSize(30, 16777215));
        QFont font3;
        font3.setFamily(QString::fromUtf8("Arial"));
        font3.setPointSize(10);
        font3.setItalic(false);
        btnLoadParts->setFont(font3);
        btnLoadParts->setStyleSheet(QString::fromUtf8(""));

        gridLayout_5->addWidget(btnLoadParts, 2, 2, 1, 1);

        btnLoadTree = new QPushButton(groupBox_3);
        btnLoadTree->setObjectName(QString::fromUtf8("btnLoadTree"));
        btnLoadTree->setEnabled(true);
        btnLoadTree->setMinimumSize(QSize(30, 0));
        btnLoadTree->setMaximumSize(QSize(30, 16777215));
        btnLoadTree->setFont(font3);

        gridLayout_5->addWidget(btnLoadTree, 1, 2, 1, 1);

        btnLoadAlignment = new QPushButton(groupBox_3);
        btnLoadAlignment->setObjectName(QString::fromUtf8("btnLoadAlignment"));
        btnLoadAlignment->setEnabled(true);
        btnLoadAlignment->setMinimumSize(QSize(30, 0));
        btnLoadAlignment->setMaximumSize(QSize(30, 16777215));
        btnLoadAlignment->setFont(font3);
        btnLoadAlignment->setStyleSheet(QString::fromUtf8(""));

        gridLayout_5->addWidget(btnLoadAlignment, 0, 2, 1, 1);


        horizontalLayout_8->addLayout(gridLayout_5);


        verticalLayout_2->addWidget(groupBox_3);

        splitter = new QSplitter(layout_main);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Vertical);
        widget_mid = new QWidget(splitter);
        widget_mid->setObjectName(QString::fromUtf8("widget_mid"));
        widget_mid->setMinimumSize(QSize(0, 300));
        horizontalLayout_11 = new QHBoxLayout(widget_mid);
        horizontalLayout_11->setSpacing(6);
        horizontalLayout_11->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_11->setObjectName(QString::fromUtf8("horizontalLayout_11"));
        horizontalLayout_11->setContentsMargins(9, 9, 9, -1);
        tabView = new QTabWidget(widget_mid);
        tabView->setObjectName(QString::fromUtf8("tabView"));
        tabView->setEnabled(true);
        tabView->setFont(font1);
        tabView->setStyleSheet(QString::fromUtf8(""));
        tabView->setTabShape(QTabWidget::Rounded);
        tabView->setIconSize(QSize(24, 24));
        tabView->setElideMode(Qt::ElideNone);
        tab_configure = new QWidget();
        tab_configure->setObjectName(QString::fromUtf8("tab_configure"));
        verticalLayout_10 = new QVBoxLayout(tab_configure);
        verticalLayout_10->setSpacing(6);
        verticalLayout_10->setContentsMargins(11, 11, 11, 11);
        verticalLayout_10->setObjectName(QString::fromUtf8("verticalLayout_10"));
        grpOptions = new QScrollArea(tab_configure);
        grpOptions->setObjectName(QString::fromUtf8("grpOptions"));
        grpOptions->setFont(font1);
        grpOptions->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 913, 773));
        verticalLayout_8 = new QVBoxLayout(scrollAreaWidgetContents);
        verticalLayout_8->setSpacing(6);
        verticalLayout_8->setContentsMargins(11, 11, 11, 11);
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        grpSetOptions = new QGroupBox(scrollAreaWidgetContents);
        grpSetOptions->setObjectName(QString::fromUtf8("grpSetOptions"));
        grpSetOptions->setMinimumSize(QSize(0, 61));
        grpSetOptions->setMaximumSize(QSize(16777215, 61));
        grpSetOptions->setFont(font3);
        grpSetOptions->setStyleSheet(QString::fromUtf8("background-color: #99d; "));
        grpSetOptions->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        horizontalLayout_4 = new QHBoxLayout(grpSetOptions);
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(-1, 20, -1, -1);
        radSetModelTest = new QRadioButton(grpSetOptions);
        radSetModelTest->setObjectName(QString::fromUtf8("radSetModelTest"));
        radSetModelTest->setFont(font3);
        radSetModelTest->setStyleSheet(QString::fromUtf8("border: none;"));
        radSetModelTest->setChecked(true);

        horizontalLayout_4->addWidget(radSetModelTest);

        radSetMrbayes = new QRadioButton(grpSetOptions);
        radSetMrbayes->setObjectName(QString::fromUtf8("radSetMrbayes"));
        radSetMrbayes->setFont(font3);
        radSetMrbayes->setStyleSheet(QString::fromUtf8("border: none;"));

        horizontalLayout_4->addWidget(radSetMrbayes);

        radSetRaxml = new QRadioButton(grpSetOptions);
        radSetRaxml->setObjectName(QString::fromUtf8("radSetRaxml"));
        radSetRaxml->setFont(font3);
        radSetRaxml->setStyleSheet(QString::fromUtf8("border: none;"));

        horizontalLayout_4->addWidget(radSetRaxml);

        radSetPhyml = new QRadioButton(grpSetOptions);
        radSetPhyml->setObjectName(QString::fromUtf8("radSetPhyml"));
        radSetPhyml->setFont(font3);
        radSetPhyml->setStyleSheet(QString::fromUtf8("border: none;"));

        horizontalLayout_4->addWidget(radSetPhyml);

        radSetPAUP = new QRadioButton(grpSetOptions);
        radSetPAUP->setObjectName(QString::fromUtf8("radSetPAUP"));
        radSetPAUP->setFont(font3);
        radSetPAUP->setStyleSheet(QString::fromUtf8("border: none;"));

        horizontalLayout_4->addWidget(radSetPAUP);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_2);


        verticalLayout_8->addWidget(grpSetOptions);

        grpTopology = new QGroupBox(scrollAreaWidgetContents);
        grpTopology->setObjectName(QString::fromUtf8("grpTopology"));
        grpTopology->setMinimumSize(QSize(0, 61));
        grpTopology->setMaximumSize(QSize(16777215, 61));
        grpTopology->setFont(font3);
        horizontalLayout_5 = new QHBoxLayout(grpTopology);
        horizontalLayout_5->setSpacing(20);
        horizontalLayout_5->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        horizontalLayout_5->setContentsMargins(-1, 10, -1, -1);
        radTopoFixedMp = new QRadioButton(grpTopology);
        radTopoFixedMp->setObjectName(QString::fromUtf8("radTopoFixedMp"));
        radTopoFixedMp->setFont(font3);

        horizontalLayout_5->addWidget(radTopoFixedMp);

        radTopoFixedJc = new QRadioButton(grpTopology);
        radTopoFixedJc->setObjectName(QString::fromUtf8("radTopoFixedJc"));
        radTopoFixedJc->setFont(font3);

        horizontalLayout_5->addWidget(radTopoFixedJc);

        radTopoFixedGtr = new QRadioButton(grpTopology);
        radTopoFixedGtr->setObjectName(QString::fromUtf8("radTopoFixedGtr"));
        radTopoFixedGtr->setFont(font3);
        radTopoFixedGtr->setChecked(true);

        horizontalLayout_5->addWidget(radTopoFixedGtr);

        radTopoML = new QRadioButton(grpTopology);
        radTopoML->setObjectName(QString::fromUtf8("radTopoML"));
        radTopoML->setFont(font3);
        radTopoML->setChecked(false);

        horizontalLayout_5->addWidget(radTopoML);

        radTopoU = new QRadioButton(grpTopology);
        radTopoU->setObjectName(QString::fromUtf8("radTopoU"));
        radTopoU->setFont(font3);

        horizontalLayout_5->addWidget(radTopoU);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer);


        verticalLayout_8->addWidget(grpTopology);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setSpacing(6);
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        horizontalLayout_9->setContentsMargins(-1, 0, -1, -1);
        grpDataType = new QGroupBox(scrollAreaWidgetContents);
        grpDataType->setObjectName(QString::fromUtf8("grpDataType"));
        grpDataType->setMinimumSize(QSize(0, 61));
        grpDataType->setMaximumSize(QSize(200, 61));
        grpDataType->setFont(font3);
        radDatatypeDna = new QRadioButton(grpDataType);
        radDatatypeDna->setObjectName(QString::fromUtf8("radDatatypeDna"));
        radDatatypeDna->setGeometry(QRect(0, 30, 91, 22));
        radDatatypeDna->setFont(font3);
        radDatatypeDna->setChecked(true);
        radDatatypeProt = new QRadioButton(grpDataType);
        radDatatypeProt->setObjectName(QString::fromUtf8("radDatatypeProt"));
        radDatatypeProt->setGeometry(QRect(90, 30, 91, 22));
        radDatatypeProt->setFont(font3);

        horizontalLayout_9->addWidget(grpDataType);

        grpSubstSchemes = new QGroupBox(scrollAreaWidgetContents);
        grpSubstSchemes->setObjectName(QString::fromUtf8("grpSubstSchemes"));
        grpSubstSchemes->setMinimumSize(QSize(0, 61));
        grpSubstSchemes->setMaximumSize(QSize(16777215, 61));
        grpSubstSchemes->setFont(font3);
        radSchemes3 = new QRadioButton(grpSubstSchemes);
        radSchemes3->setObjectName(QString::fromUtf8("radSchemes3"));
        radSchemes3->setGeometry(QRect(0, 30, 51, 22));
        radSchemes3->setFont(font3);
        radSchemes5 = new QRadioButton(grpSubstSchemes);
        radSchemes5->setObjectName(QString::fromUtf8("radSchemes5"));
        radSchemes5->setGeometry(QRect(60, 30, 51, 22));
        radSchemes5->setFont(font3);
        radSchemes7 = new QRadioButton(grpSubstSchemes);
        radSchemes7->setObjectName(QString::fromUtf8("radSchemes7"));
        radSchemes7->setGeometry(QRect(120, 30, 51, 22));
        radSchemes7->setFont(font3);
        radSchemes11 = new QRadioButton(grpSubstSchemes);
        radSchemes11->setObjectName(QString::fromUtf8("radSchemes11"));
        radSchemes11->setGeometry(QRect(180, 30, 51, 22));
        radSchemes11->setFont(font3);
        radSchemes11->setChecked(true);
        radSchemes203 = new QRadioButton(grpSubstSchemes);
        radSchemes203->setObjectName(QString::fromUtf8("radSchemes203"));
        radSchemes203->setGeometry(QRect(240, 30, 61, 22));
        radSchemes203->setFont(font3);
        radSchemes203->setChecked(false);
        radSchemesUser = new QRadioButton(grpSubstSchemes);
        radSchemesUser->setObjectName(QString::fromUtf8("radSchemesUser"));
        radSchemesUser->setGeometry(QRect(310, 30, 131, 22));
        radSchemesUser->setFont(font3);
        radSchemesUser->setChecked(false);

        horizontalLayout_9->addWidget(grpSubstSchemes);


        verticalLayout_8->addLayout(horizontalLayout_9);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(0);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        lblTextNModels = new QLabel(scrollAreaWidgetContents);
        lblTextNModels->setObjectName(QString::fromUtf8("lblTextNModels"));
        lblTextNModels->setMinimumSize(QSize(150, 22));
        lblTextNModels->setMaximumSize(QSize(150, 22));
        lblTextNModels->setFont(font3);

        horizontalLayout_3->addWidget(lblTextNModels);

        lblNumModels = new QLabel(scrollAreaWidgetContents);
        lblNumModels->setObjectName(QString::fromUtf8("lblNumModels"));
        lblNumModels->setMinimumSize(QSize(100, 22));
        lblNumModels->setMaximumSize(QSize(16777215, 22));
        lblNumModels->setFont(font3);

        horizontalLayout_3->addWidget(lblNumModels);

        cbShowMatrices = new QCheckBox(scrollAreaWidgetContents);
        cbShowMatrices->setObjectName(QString::fromUtf8("cbShowMatrices"));
        QFont font4;
        font4.setFamily(QString::fromUtf8("Arial"));
        font4.setPointSize(10);
        font4.setItalic(true);
        cbShowMatrices->setFont(font4);

        horizontalLayout_3->addWidget(cbShowMatrices);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);


        verticalLayout_8->addLayout(horizontalLayout_3);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setSpacing(6);
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        horizontalLayout_7->setContentsMargins(-1, 5, -1, 5);
        listMatrices = new QListWidget(scrollAreaWidgetContents);
        new QListWidgetItem(listMatrices);
        new QListWidgetItem(listMatrices);
        new QListWidgetItem(listMatrices);
        new QListWidgetItem(listMatrices);
        new QListWidgetItem(listMatrices);
        new QListWidgetItem(listMatrices);
        new QListWidgetItem(listMatrices);
        new QListWidgetItem(listMatrices);
        new QListWidgetItem(listMatrices);
        new QListWidgetItem(listMatrices);
        new QListWidgetItem(listMatrices);
        listMatrices->setObjectName(QString::fromUtf8("listMatrices"));
        listMatrices->setMinimumSize(QSize(200, 212));
        listMatrices->setMaximumSize(QSize(200, 212));
        listMatrices->setFont(font3);
        listMatrices->setProperty("showDropIndicator", QVariant(false));
        listMatrices->setSelectionMode(QAbstractItemView::MultiSelection);

        horizontalLayout_7->addWidget(listMatrices);


        verticalLayout_8->addLayout(horizontalLayout_7);

        grpModelSets = new QGroupBox(scrollAreaWidgetContents);
        grpModelSets->setObjectName(QString::fromUtf8("grpModelSets"));
        grpModelSets->setMinimumSize(QSize(755, 125));
        grpModelSets->setMaximumSize(QSize(16777215, 125));
        grpModelSets->setFont(font1);
        gridLayout = new QGridLayout(grpModelSets);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        cbGModels = new QCheckBox(grpModelSets);
        cbGModels->setObjectName(QString::fromUtf8("cbGModels"));
        cbGModels->setMinimumSize(QSize(220, 22));
        cbGModels->setMaximumSize(QSize(220, 16777215));
        cbGModels->setFont(font1);
        cbGModels->setChecked(true);

        gridLayout->addWidget(cbGModels, 2, 2, 1, 1);

        cbNoRateVarModels = new QCheckBox(grpModelSets);
        cbNoRateVarModels->setObjectName(QString::fromUtf8("cbNoRateVarModels"));
        cbNoRateVarModels->setMinimumSize(QSize(160, 22));
        cbNoRateVarModels->setMaximumSize(QSize(160, 16777215));
        cbNoRateVarModels->setFont(font1);
        cbNoRateVarModels->setChecked(true);

        gridLayout->addWidget(cbNoRateVarModels, 2, 0, 1, 1);

        sliderNCat = new QSlider(grpModelSets);
        sliderNCat->setObjectName(QString::fromUtf8("sliderNCat"));
        sliderNCat->setMinimumSize(QSize(220, 30));
        sliderNCat->setMaximumSize(QSize(220, 16777215));
        sliderNCat->setFont(font1);
        sliderNCat->setAutoFillBackground(false);
        sliderNCat->setMinimum(2);
        sliderNCat->setMaximum(20);
        sliderNCat->setValue(4);
        sliderNCat->setOrientation(Qt::Horizontal);

        gridLayout->addWidget(sliderNCat, 3, 2, 1, 1);

        lblNCat = new QLabel(grpModelSets);
        lblNCat->setObjectName(QString::fromUtf8("lblNCat"));
        lblNCat->setMinimumSize(QSize(100, 26));
        lblNCat->setMaximumSize(QSize(100, 16777215));
        lblNCat->setFont(font1);

        gridLayout->addWidget(lblNCat, 3, 3, 1, 1);

        cbIModels = new QCheckBox(grpModelSets);
        cbIModels->setObjectName(QString::fromUtf8("cbIModels"));
        cbIModels->setMinimumSize(QSize(160, 22));
        cbIModels->setMaximumSize(QSize(180, 16777215));
        cbIModels->setFont(font1);
        cbIModels->setChecked(true);

        gridLayout->addWidget(cbIModels, 2, 1, 1, 1);

        cbIGModels = new QCheckBox(grpModelSets);
        cbIGModels->setObjectName(QString::fromUtf8("cbIGModels"));
        cbIGModels->setMinimumSize(QSize(100, 22));
        cbIGModels->setFont(font1);
        cbIGModels->setChecked(true);

        gridLayout->addWidget(cbIGModels, 2, 3, 1, 1);

        cbEqualFreq = new QCheckBox(grpModelSets);
        cbEqualFreq->setObjectName(QString::fromUtf8("cbEqualFreq"));
        cbEqualFreq->setMinimumSize(QSize(160, 22));
        cbEqualFreq->setMaximumSize(QSize(160, 16777215));
        cbEqualFreq->setFont(font1);
        cbEqualFreq->setChecked(true);

        gridLayout->addWidget(cbEqualFreq, 1, 0, 1, 1);

        cbMlFreq = new QCheckBox(grpModelSets);
        cbMlFreq->setObjectName(QString::fromUtf8("cbMlFreq"));
        cbMlFreq->setMinimumSize(QSize(160, 22));
        cbMlFreq->setMaximumSize(QSize(180, 16777215));
        cbMlFreq->setFont(font1);
        cbMlFreq->setChecked(true);

        gridLayout->addWidget(cbMlFreq, 1, 1, 1, 1);


        verticalLayout_8->addWidget(grpModelSets);

        cbAdvanced = new QCheckBox(scrollAreaWidgetContents);
        cbAdvanced->setObjectName(QString::fromUtf8("cbAdvanced"));
        cbAdvanced->setMinimumSize(QSize(205, 22));
        cbAdvanced->setMaximumSize(QSize(16777215, 22));
        cbAdvanced->setFont(font4);

        verticalLayout_8->addWidget(cbAdvanced);

        grpAdvanced = new QGroupBox(scrollAreaWidgetContents);
        grpAdvanced->setObjectName(QString::fromUtf8("grpAdvanced"));
        grpAdvanced->setMinimumSize(QSize(0, 135));
        grpAdvanced->setMaximumSize(QSize(16777215, 135));
        grpAdvanced->setFont(font1);
        grpAdvanced->setStyleSheet(QString::fromUtf8("background-color: #99d;"));
        gridLayout_2 = new QGridLayout(grpAdvanced);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setContentsMargins(-1, 0, -1, 10);
        txtOptEpsilon = new QLineEdit(grpAdvanced);
        txtOptEpsilon->setObjectName(QString::fromUtf8("txtOptEpsilon"));
        txtOptEpsilon->setMinimumSize(QSize(0, 27));
        txtOptEpsilon->setMaximumSize(QSize(150, 16777215));
        txtOptEpsilon->setFont(font1);
        txtOptEpsilon->setMaxLength(10);
        txtOptEpsilon->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(txtOptEpsilon, 2, 1, 1, 1);

        txtParEpsilon = new QLineEdit(grpAdvanced);
        txtParEpsilon->setObjectName(QString::fromUtf8("txtParEpsilon"));
        txtParEpsilon->setMinimumSize(QSize(0, 27));
        txtParEpsilon->setMaximumSize(QSize(150, 16777215));
        txtParEpsilon->setFont(font1);
        txtParEpsilon->setMaxLength(10);
        txtParEpsilon->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(txtParEpsilon, 1, 1, 1, 1);

        lblParEpsText = new QLabel(grpAdvanced);
        lblParEpsText->setObjectName(QString::fromUtf8("lblParEpsText"));
        lblParEpsText->setMinimumSize(QSize(0, 27));
        lblParEpsText->setFont(font1);

        gridLayout_2->addWidget(lblParEpsText, 1, 0, 1, 1);

        lblOptEpsText = new QLabel(grpAdvanced);
        lblOptEpsText->setObjectName(QString::fromUtf8("lblOptEpsText"));
        lblOptEpsText->setMinimumSize(QSize(0, 27));
        lblOptEpsText->setFont(font1);

        gridLayout_2->addWidget(lblOptEpsText, 2, 0, 1, 1);

        lblNThreads = new QLabel(grpAdvanced);
        lblNThreads->setObjectName(QString::fromUtf8("lblNThreads"));
        lblNThreads->setMinimumSize(QSize(100, 0));
        lblNThreads->setFont(font1);

        gridLayout_2->addWidget(lblNThreads, 0, 2, 1, 1);

        sliderNThreads = new QSlider(grpAdvanced);
        sliderNThreads->setObjectName(QString::fromUtf8("sliderNThreads"));
        sliderNThreads->setMaximumSize(QSize(300, 16777215));
        sliderNThreads->setFont(font1);
        sliderNThreads->setAutoFillBackground(false);
        sliderNThreads->setMinimum(1);
        sliderNThreads->setMaximum(10);
        sliderNThreads->setValue(2);
        sliderNThreads->setOrientation(Qt::Horizontal);

        gridLayout_2->addWidget(sliderNThreads, 0, 1, 1, 1);

        lblNThreadText = new QLabel(grpAdvanced);
        lblNThreadText->setObjectName(QString::fromUtf8("lblNThreadText"));
        lblNThreadText->setFont(font1);

        gridLayout_2->addWidget(lblNThreadText, 0, 0, 1, 1);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_4, 1, 2, 1, 1);


        verticalLayout_8->addWidget(grpAdvanced);

        grpOptions->setWidget(scrollAreaWidgetContents);

        verticalLayout_10->addWidget(grpOptions);

        verticalLayout_9 = new QVBoxLayout();
        verticalLayout_9->setSpacing(6);
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        gridLayout_3 = new QGridLayout();
        gridLayout_3->setSpacing(6);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        lblTextEstimatedMem2 = new QLabel(tab_configure);
        lblTextEstimatedMem2->setObjectName(QString::fromUtf8("lblTextEstimatedMem2"));
        lblTextEstimatedMem2->setMinimumSize(QSize(0, 30));
        lblTextEstimatedMem2->setMaximumSize(QSize(16777215, 30));
        lblTextEstimatedMem2->setFont(font3);

        gridLayout_3->addWidget(lblTextEstimatedMem2, 0, 2, 1, 1);

        lblEstimatedMem = new QLabel(tab_configure);
        lblEstimatedMem->setObjectName(QString::fromUtf8("lblEstimatedMem"));
        lblEstimatedMem->setMinimumSize(QSize(60, 30));
        lblEstimatedMem->setMaximumSize(QSize(60, 30));
        lblEstimatedMem->setFont(font3);
        lblEstimatedMem->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_3->addWidget(lblEstimatedMem, 0, 1, 1, 1);

        lblTextEstimatedMem = new QLabel(tab_configure);
        lblTextEstimatedMem->setObjectName(QString::fromUtf8("lblTextEstimatedMem"));
        lblTextEstimatedMem->setMinimumSize(QSize(200, 30));
        lblTextEstimatedMem->setMaximumSize(QSize(200, 30));
        lblTextEstimatedMem->setFont(font3);

        gridLayout_3->addWidget(lblTextEstimatedMem, 0, 0, 1, 1);


        verticalLayout_9->addLayout(gridLayout_3);

        gridLayout_4 = new QGridLayout();
        gridLayout_4->setSpacing(6);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        btnResetConfig = new QPushButton(tab_configure);
        btnResetConfig->setObjectName(QString::fromUtf8("btnResetConfig"));
        btnResetConfig->setMinimumSize(QSize(0, 30));
        btnResetConfig->setMaximumSize(QSize(150, 30));
        btnResetConfig->setFont(font3);

        gridLayout_4->addWidget(btnResetConfig, 0, 0, 1, 1);

        btnRun = new QPushButton(tab_configure);
        btnRun->setObjectName(QString::fromUtf8("btnRun"));
        btnRun->setEnabled(true);
        btnRun->setMinimumSize(QSize(0, 30));
        btnRun->setMaximumSize(QSize(150, 30));
        btnRun->setFont(font3);
        btnRun->setIconSize(QSize(60, 60));

        gridLayout_4->addWidget(btnRun, 0, 1, 1, 1);


        verticalLayout_9->addLayout(gridLayout_4);


        verticalLayout_10->addLayout(verticalLayout_9);

        QIcon icon1;
        icon1.addFile(QString::fromUtf8("IMG/tool-icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        tabView->addTab(tab_configure, icon1, QString());
        tab_runsingle = new QWidget();
        tab_runsingle->setObjectName(QString::fromUtf8("tab_runsingle"));
        verticalLayout_3 = new QVBoxLayout(tab_runsingle);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        groupBox_4 = new QGroupBox(tab_runsingle);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        cbShowHetParams = new QCheckBox(groupBox_4);
        cbShowHetParams->setObjectName(QString::fromUtf8("cbShowHetParams"));
        cbShowHetParams->setGeometry(QRect(135, 0, 121, 22));
        QFont font5;
        font5.setFamily(QString::fromUtf8("Arial"));
        font5.setItalic(false);
        cbShowHetParams->setFont(font5);
        cbShowRates = new QCheckBox(groupBox_4);
        cbShowRates->setObjectName(QString::fromUtf8("cbShowRates"));
        cbShowRates->setGeometry(QRect(385, 0, 121, 22));
        cbShowRates->setFont(font5);
        cbShowFreqs = new QCheckBox(groupBox_4);
        cbShowFreqs->setObjectName(QString::fromUtf8("cbShowFreqs"));
        cbShowFreqs->setGeometry(QRect(260, 0, 121, 22));
        cbShowFreqs->setFont(font5);
        cbShowSelection = new QCheckBox(groupBox_4);
        cbShowSelection->setObjectName(QString::fromUtf8("cbShowSelection"));
        cbShowSelection->setGeometry(QRect(10, 0, 121, 22));
        cbShowSelection->setFont(font5);
        cbShowSelection->setChecked(true);

        verticalLayout_3->addWidget(groupBox_4);

        tblModels = new QTableView(tab_runsingle);
        tblModels->setObjectName(QString::fromUtf8("tblModels"));
        QFont font6;
        font6.setFamily(QString::fromUtf8("Courier 10 Pitch"));
        font6.setPointSize(10);
        tblModels->setFont(font6);
        tblModels->setAlternatingRowColors(true);
        tblModels->setSortingEnabled(true);

        verticalLayout_3->addWidget(tblModels);

        QIcon icon2;
        icon2.addFile(QString::fromUtf8("IMG/run-icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        tabView->addTab(tab_runsingle, icon2, QString());
        tab_results = new QWidget();
        tab_results->setObjectName(QString::fromUtf8("tab_results"));
        horizontalLayout = new QHBoxLayout(tab_results);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        results_content = new QTabWidget(tab_results);
        results_content->setObjectName(QString::fromUtf8("results_content"));
        results_content->setFont(font);
        tabResultsBic = new QWidget();
        tabResultsBic->setObjectName(QString::fromUtf8("tabResultsBic"));
        verticalLayout_4 = new QVBoxLayout(tabResultsBic);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        tblResultsBic = new QTableView(tabResultsBic);
        tblResultsBic->setObjectName(QString::fromUtf8("tblResultsBic"));
        tblResultsBic->setFont(font6);
        tblResultsBic->setAlternatingRowColors(true);
        tblResultsBic->setSortingEnabled(true);

        verticalLayout_4->addWidget(tblResultsBic);

        results_content->addTab(tabResultsBic, QString());
        tabResultsAic = new QWidget();
        tabResultsAic->setObjectName(QString::fromUtf8("tabResultsAic"));
        verticalLayout_5 = new QVBoxLayout(tabResultsAic);
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setContentsMargins(11, 11, 11, 11);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        tblResultsAic = new QTableView(tabResultsAic);
        tblResultsAic->setObjectName(QString::fromUtf8("tblResultsAic"));
        tblResultsAic->setFont(font6);
        tblResultsAic->setAlternatingRowColors(true);
        tblResultsAic->setSortingEnabled(true);

        verticalLayout_5->addWidget(tblResultsAic);

        results_content->addTab(tabResultsAic, QString());
        tabResultsAicc = new QWidget();
        tabResultsAicc->setObjectName(QString::fromUtf8("tabResultsAicc"));
        verticalLayout_6 = new QVBoxLayout(tabResultsAicc);
        verticalLayout_6->setSpacing(6);
        verticalLayout_6->setContentsMargins(11, 11, 11, 11);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        tblResultsAicc = new QTableView(tabResultsAicc);
        tblResultsAicc->setObjectName(QString::fromUtf8("tblResultsAicc"));
        tblResultsAicc->setFont(font6);
        tblResultsAicc->setAlternatingRowColors(true);
        tblResultsAicc->setSortingEnabled(true);

        verticalLayout_6->addWidget(tblResultsAicc);

        results_content->addTab(tabResultsAicc, QString());
        tabResultsDt = new QWidget();
        tabResultsDt->setObjectName(QString::fromUtf8("tabResultsDt"));
        verticalLayout_7 = new QVBoxLayout(tabResultsDt);
        verticalLayout_7->setSpacing(6);
        verticalLayout_7->setContentsMargins(11, 11, 11, 11);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        tblResultsDt = new QTableView(tabResultsDt);
        tblResultsDt->setObjectName(QString::fromUtf8("tblResultsDt"));
        tblResultsDt->setFont(font6);
        tblResultsDt->setAlternatingRowColors(true);
        tblResultsDt->setSortingEnabled(true);

        verticalLayout_7->addWidget(tblResultsDt);

        results_content->addTab(tabResultsDt, QString());

        horizontalLayout->addWidget(results_content);

        QIcon icon3;
        icon3.addFile(QString::fromUtf8("IMG/results-icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        tabView->addTab(tab_results, icon3, QString());
        tab_console = new QWidget();
        tab_console->setObjectName(QString::fromUtf8("tab_console"));
        horizontalLayout_2 = new QHBoxLayout(tab_console);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8("IMG/console-icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        tabView->addTab(tab_console, icon4, QString());

        horizontalLayout_11->addWidget(tabView);

        splitter->addWidget(widget_mid);
        widget_footer = new QWidget(splitter);
        widget_footer->setObjectName(QString::fromUtf8("widget_footer"));
        widget_footer->setMinimumSize(QSize(0, 100));
        horizontalLayout_10 = new QHBoxLayout(widget_footer);
        horizontalLayout_10->setSpacing(6);
        horizontalLayout_10->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_10->setObjectName(QString::fromUtf8("horizontalLayout_10"));
        horizontalLayout_10->setSizeConstraint(QLayout::SetMinimumSize);
        horizontalLayout_10->setContentsMargins(9, 9, 9, 9);
        txt_messages = new QTextBrowser(widget_footer);
        txt_messages->setObjectName(QString::fromUtf8("txt_messages"));
        txt_messages->setMinimumSize(QSize(400, 100));
        txt_messages->setMaximumSize(QSize(10000, 10000));
        txt_messages->setFont(font1);

        horizontalLayout_10->addWidget(txt_messages);

        splitter->addWidget(widget_footer);

        verticalLayout_2->addWidget(splitter);

        jModelTest->setCentralWidget(layout_main);
        menuBar = new QMenuBar(jModelTest);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 989, 25));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuWhat_is_this = new QMenu(menuBar);
        menuWhat_is_this->setObjectName(QString::fromUtf8("menuWhat_is_this"));
        menuWindow = new QMenu(menuBar);
        menuWindow->setObjectName(QString::fromUtf8("menuWindow"));
        menuTab = new QMenu(menuWindow);
        menuTab->setObjectName(QString::fromUtf8("menuTab"));
        menuTab->setFont(font);
        jModelTest->setMenuBar(menuBar);
        mainToolBar = new QToolBar(jModelTest);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        jModelTest->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(jModelTest);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        jModelTest->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuWindow->menuAction());
        menuBar->addAction(menuWhat_is_this->menuAction());
        menuFile->addAction(menuFileLoad);
        menuFile->addAction(menuTreeLoad);
        menuFile->addAction(menuSaveData);
        menuFile->addAction(actionReset);
        menuFile->addSeparator();
        menuFile->addAction(menuFileQuit);
        menuWhat_is_this->addAction(actionNewMenu);
        menuWindow->addAction(menuTab->menuAction());
        menuTab->addAction(actionConsole);
        menuTab->addAction(actionConfigure);
        menuTab->addAction(actionProgress);
        menuTab->addAction(actionResults);

        retranslateUi(jModelTest);

        tabView->setCurrentIndex(0);
        results_content->setCurrentIndex(3);


        QMetaObject::connectSlotsByName(jModelTest);
    } // setupUi

    void retranslateUi(QMainWindow *jModelTest)
    {
        jModelTest->setWindowTitle(QApplication::translate("jModelTest", "jModelTest", 0, QApplication::UnicodeUTF8));
        menuFileLoad->setText(QApplication::translate("jModelTest", "Load alignment", 0, QApplication::UnicodeUTF8));
        menuFileLoad->setShortcut(QApplication::translate("jModelTest", "Ctrl+O", 0, QApplication::UnicodeUTF8));
        actionQuit->setText(QApplication::translate("jModelTest", "Quit", 0, QApplication::UnicodeUTF8));
        menuFileQuit->setText(QApplication::translate("jModelTest", "Quit", 0, QApplication::UnicodeUTF8));
        menuFileQuit->setShortcut(QApplication::translate("jModelTest", "Ctrl+Q", 0, QApplication::UnicodeUTF8));
        menuSaveData->setText(QApplication::translate("jModelTest", "Save data", 0, QApplication::UnicodeUTF8));
        menuSaveData->setShortcut(QApplication::translate("jModelTest", "Ctrl+S", 0, QApplication::UnicodeUTF8));
        actionReset->setText(QApplication::translate("jModelTest", "Reset", 0, QApplication::UnicodeUTF8));
        actionReset->setShortcut(QApplication::translate("jModelTest", "Ctrl+R", 0, QApplication::UnicodeUTF8));
        menuTreeLoad->setText(QApplication::translate("jModelTest", "Load tree", 0, QApplication::UnicodeUTF8));
        menuTreeLoad->setShortcut(QApplication::translate("jModelTest", "Ctrl+T", 0, QApplication::UnicodeUTF8));
        actionNewMenu->setText(QApplication::translate("jModelTest", "NewMenu", 0, QApplication::UnicodeUTF8));
        actionConsole->setText(QApplication::translate("jModelTest", "Console", 0, QApplication::UnicodeUTF8));
        actionConsole->setShortcut(QApplication::translate("jModelTest", "Alt+1", 0, QApplication::UnicodeUTF8));
        actionConfigure->setText(QApplication::translate("jModelTest", "Configure", 0, QApplication::UnicodeUTF8));
        actionConfigure->setShortcut(QApplication::translate("jModelTest", "Alt+2", 0, QApplication::UnicodeUTF8));
        actionProgress->setText(QApplication::translate("jModelTest", "Progress", 0, QApplication::UnicodeUTF8));
        actionProgress->setShortcut(QApplication::translate("jModelTest", "Alt+3", 0, QApplication::UnicodeUTF8));
        actionResults->setText(QApplication::translate("jModelTest", "Results", 0, QApplication::UnicodeUTF8));
        actionResults->setShortcut(QApplication::translate("jModelTest", "Alt+4", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QString());
        lblGear->setText(QString());
        lblAlignment->setText(QString());
        lblTree->setText(QString());
        lblLoadAlignText->setText(QApplication::translate("jModelTest", "MSA:", 0, QApplication::UnicodeUTF8));
        lblParts->setText(QString());
        lblLoadTreeText->setText(QApplication::translate("jModelTest", "Tree:", 0, QApplication::UnicodeUTF8));
        lblLoadPartsText->setText(QApplication::translate("jModelTest", "Partitions:", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        btnLoadParts->setToolTip(QApplication::translate("jModelTest", "<html><head/><body><p>Load tree file in <span style=\" font-weight:600;\">NEWICK</span> format</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        btnLoadParts->setText(QApplication::translate("jModelTest", "...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        btnLoadTree->setToolTip(QApplication::translate("jModelTest", "<html><head/><body><p>Load tree file in <span style=\" font-weight:600;\">NEWICK</span> format</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        btnLoadTree->setText(QApplication::translate("jModelTest", "...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        btnLoadAlignment->setToolTip(QApplication::translate("jModelTest", "<html><head/><body><p>Load multiple sequence alignment in <span style=\" font-weight:600;\">FASTA</span> format</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        btnLoadAlignment->setText(QApplication::translate("jModelTest", "...", 0, QApplication::UnicodeUTF8));
        grpSetOptions->setTitle(QApplication::translate("jModelTest", "Options for...", 0, QApplication::UnicodeUTF8));
        radSetModelTest->setText(QApplication::translate("jModelTest", "ModelTest", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        radSetMrbayes->setToolTip(QApplication::translate("jModelTest", "<html><head/><body><p>Select default options for MrBAYES</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        radSetMrbayes->setText(QApplication::translate("jModelTest", "MrBAYES", 0, QApplication::UnicodeUTF8));
        radSetRaxml->setText(QApplication::translate("jModelTest", "RAxML", 0, QApplication::UnicodeUTF8));
        radSetPhyml->setText(QApplication::translate("jModelTest", "PhyML", 0, QApplication::UnicodeUTF8));
        radSetPAUP->setText(QApplication::translate("jModelTest", "PAUP*", 0, QApplication::UnicodeUTF8));
        grpTopology->setTitle(QApplication::translate("jModelTest", "Starting topology", 0, QApplication::UnicodeUTF8));
        radTopoFixedMp->setText(QApplication::translate("jModelTest", "Fixed MP", 0, QApplication::UnicodeUTF8));
        radTopoFixedJc->setText(QApplication::translate("jModelTest", "Fixed ML (JC)", 0, QApplication::UnicodeUTF8));
        radTopoFixedGtr->setText(QApplication::translate("jModelTest", "Fixed ML (GTR)", 0, QApplication::UnicodeUTF8));
        radTopoML->setText(QApplication::translate("jModelTest", "Maximum-Likelihood", 0, QApplication::UnicodeUTF8));
        radTopoU->setText(QApplication::translate("jModelTest", "User defined", 0, QApplication::UnicodeUTF8));
        grpDataType->setTitle(QApplication::translate("jModelTest", "Data type", 0, QApplication::UnicodeUTF8));
        radDatatypeDna->setText(QApplication::translate("jModelTest", "DNA", 0, QApplication::UnicodeUTF8));
        radDatatypeProt->setText(QApplication::translate("jModelTest", "Protein", 0, QApplication::UnicodeUTF8));
        grpSubstSchemes->setTitle(QApplication::translate("jModelTest", "Substitution Schemes", 0, QApplication::UnicodeUTF8));
        radSchemes3->setText(QApplication::translate("jModelTest", "3", 0, QApplication::UnicodeUTF8));
        radSchemes5->setText(QApplication::translate("jModelTest", "5", 0, QApplication::UnicodeUTF8));
        radSchemes7->setText(QApplication::translate("jModelTest", "7", 0, QApplication::UnicodeUTF8));
        radSchemes11->setText(QApplication::translate("jModelTest", "11", 0, QApplication::UnicodeUTF8));
        radSchemes203->setText(QApplication::translate("jModelTest", "203", 0, QApplication::UnicodeUTF8));
        radSchemesUser->setText(QApplication::translate("jModelTest", "User defined", 0, QApplication::UnicodeUTF8));
        lblTextNModels->setText(QApplication::translate("jModelTest", "Number of models:", 0, QApplication::UnicodeUTF8));
        lblNumModels->setText(QApplication::translate("jModelTest", "0", 0, QApplication::UnicodeUTF8));
        cbShowMatrices->setText(QApplication::translate("jModelTest", "Show model matrices", 0, QApplication::UnicodeUTF8));

        const bool __sortingEnabled = listMatrices->isSortingEnabled();
        listMatrices->setSortingEnabled(false);
        QListWidgetItem *___qlistwidgetitem = listMatrices->item(0);
        ___qlistwidgetitem->setText(QApplication::translate("jModelTest", "000000  JC / F81", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem1 = listMatrices->item(1);
        ___qlistwidgetitem1->setText(QApplication::translate("jModelTest", "010010  K80 / HKY85", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem2 = listMatrices->item(2);
        ___qlistwidgetitem2->setText(QApplication::translate("jModelTest", "010020  TrNef / TrN", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem3 = listMatrices->item(3);
        ___qlistwidgetitem3->setText(QApplication::translate("jModelTest", "012210  TPM1 / TPM1uf", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem4 = listMatrices->item(4);
        ___qlistwidgetitem4->setText(QApplication::translate("jModelTest", "010212  TPM2 / TPM2uf", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem5 = listMatrices->item(5);
        ___qlistwidgetitem5->setText(QApplication::translate("jModelTest", "012012  TPM3 / TPM3uf", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem6 = listMatrices->item(6);
        ___qlistwidgetitem6->setText(QApplication::translate("jModelTest", "012230  TIM1ef / TIM1", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem7 = listMatrices->item(7);
        ___qlistwidgetitem7->setText(QApplication::translate("jModelTest", "010232  TIM2ef / TIM2", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem8 = listMatrices->item(8);
        ___qlistwidgetitem8->setText(QApplication::translate("jModelTest", "012032  TIM3ef / TIM3", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem9 = listMatrices->item(9);
        ___qlistwidgetitem9->setText(QApplication::translate("jModelTest", "012314  TVMef / TVM", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem10 = listMatrices->item(10);
        ___qlistwidgetitem10->setText(QApplication::translate("jModelTest", "012345  SYM / GTR", 0, QApplication::UnicodeUTF8));
        listMatrices->setSortingEnabled(__sortingEnabled);

        grpModelSets->setTitle(QApplication::translate("jModelTest", "Sets", 0, QApplication::UnicodeUTF8));
        cbGModels->setText(QApplication::translate("jModelTest", "+G - discrete Gamma rates", 0, QApplication::UnicodeUTF8));
        cbNoRateVarModels->setText(QApplication::translate("jModelTest", "no rate variation", 0, QApplication::UnicodeUTF8));
        lblNCat->setText(QApplication::translate("jModelTest", "4 categories", 0, QApplication::UnicodeUTF8));
        cbIModels->setText(QApplication::translate("jModelTest", "+I - invariant sites", 0, QApplication::UnicodeUTF8));
        cbIGModels->setText(QApplication::translate("jModelTest", "+I+G", 0, QApplication::UnicodeUTF8));
        cbEqualFreq->setText(QApplication::translate("jModelTest", "Equal frequencies", 0, QApplication::UnicodeUTF8));
        cbMlFreq->setText(QApplication::translate("jModelTest", "ML frequencies", 0, QApplication::UnicodeUTF8));
        cbAdvanced->setText(QApplication::translate("jModelTest", "Show advanced options", 0, QApplication::UnicodeUTF8));
        grpAdvanced->setTitle(QString());
        txtOptEpsilon->setText(QApplication::translate("jModelTest", "0.01", 0, QApplication::UnicodeUTF8));
        txtParEpsilon->setText(QApplication::translate("jModelTest", "0.001", 0, QApplication::UnicodeUTF8));
        lblParEpsText->setText(QApplication::translate("jModelTest", "Parameter epsilon:", 0, QApplication::UnicodeUTF8));
        lblOptEpsText->setText(QApplication::translate("jModelTest", "Optimization epsilon:", 0, QApplication::UnicodeUTF8));
        lblNThreads->setText(QApplication::translate("jModelTest", "4", 0, QApplication::UnicodeUTF8));
        lblNThreadText->setText(QApplication::translate("jModelTest", "Number of threads:", 0, QApplication::UnicodeUTF8));
        lblTextEstimatedMem2->setText(QApplication::translate("jModelTest", "MB", 0, QApplication::UnicodeUTF8));
        lblEstimatedMem->setText(QApplication::translate("jModelTest", "6000", 0, QApplication::UnicodeUTF8));
        lblTextEstimatedMem->setText(QApplication::translate("jModelTest", "Estimated memory required", 0, QApplication::UnicodeUTF8));
        btnResetConfig->setText(QApplication::translate("jModelTest", "Reset", 0, QApplication::UnicodeUTF8));
        btnRun->setText(QApplication::translate("jModelTest", "Run", 0, QApplication::UnicodeUTF8));
        tabView->setTabText(tabView->indexOf(tab_configure), QApplication::translate("jModelTest", "Configure", 0, QApplication::UnicodeUTF8));
        groupBox_4->setTitle(QString());
        cbShowHetParams->setText(QApplication::translate("jModelTest", "Het. params", 0, QApplication::UnicodeUTF8));
        cbShowRates->setText(QApplication::translate("jModelTest", "Rates", 0, QApplication::UnicodeUTF8));
        cbShowFreqs->setText(QApplication::translate("jModelTest", "Frequencies", 0, QApplication::UnicodeUTF8));
        cbShowSelection->setText(QApplication::translate("jModelTest", "Selection", 0, QApplication::UnicodeUTF8));
        tabView->setTabText(tabView->indexOf(tab_runsingle), QApplication::translate("jModelTest", "Progress", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        results_content->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        results_content->setTabText(results_content->indexOf(tabResultsBic), QApplication::translate("jModelTest", "BIC", 0, QApplication::UnicodeUTF8));
        results_content->setTabText(results_content->indexOf(tabResultsAic), QApplication::translate("jModelTest", "AIC", 0, QApplication::UnicodeUTF8));
        results_content->setTabText(results_content->indexOf(tabResultsAicc), QApplication::translate("jModelTest", "AICc", 0, QApplication::UnicodeUTF8));
        results_content->setTabText(results_content->indexOf(tabResultsDt), QApplication::translate("jModelTest", "DT", 0, QApplication::UnicodeUTF8));
        tabView->setTabText(tabView->indexOf(tab_results), QApplication::translate("jModelTest", "Results", 0, QApplication::UnicodeUTF8));
        tabView->setTabText(tabView->indexOf(tab_console), QApplication::translate("jModelTest", "Debug Console", 0, QApplication::UnicodeUTF8));
        txt_messages->setHtml(QApplication::translate("jModelTest", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Arial'; font-size:10pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-family:'Ubuntu'; font-size:11pt;\"><br /></p></body></html>", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("jModelTest", "File", 0, QApplication::UnicodeUTF8));
        menuWhat_is_this->setTitle(QApplication::translate("jModelTest", "What is this?", 0, QApplication::UnicodeUTF8));
        menuWindow->setTitle(QApplication::translate("jModelTest", "Window", 0, QApplication::UnicodeUTF8));
        menuTab->setTitle(QApplication::translate("jModelTest", "Tab", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class jModelTest: public Ui_jModelTest {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MODELTEST_GUI_H
