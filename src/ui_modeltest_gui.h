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
#include <QtGui/QComboBox>
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
    QFrame *frameHeader;
    QHBoxLayout *horizontalLayout_7;
    QLabel *lblGear;
    QSpacerItem *horizontalSpacer_9;
    QGridLayout *gridData;
    QLabel *lblAlignment;
    QLabel *lblTree;
    QLabel *lblLoadAlignText;
    QLabel *lblParts;
    QLabel *lblLoadTreeText;
    QLabel *lblLoadPartsText;
    QPushButton *btnLoadAlignment;
    QPushButton *btnLoadTree;
    QPushButton *btnLoadParts;
    QSplitter *splitter;
    QWidget *widget_mid;
    QHBoxLayout *horizontalLayout_11;
    QTabWidget *tabView;
    QWidget *tab_configure;
    QVBoxLayout *verticalLayout_10;
    QScrollArea *grpOptions;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout_8;
    QFrame *frameData;
    QVBoxLayout *verticalLayout;
    QFrame *frame_5;
    QHBoxLayout *horizontalLayout_9;
    QLabel *label_6;
    QRadioButton *radDatatypeDna;
    QRadioButton *radDatatypeProt;
    QSpacerItem *horizontalSpacer_8;
    QFrame *frame_settings_1;
    QHBoxLayout *horizontalLayout_13;
    QLabel *label;
    QRadioButton *radSetModelTest;
    QRadioButton *radSetMrbayes;
    QRadioButton *radSetRaxml;
    QRadioButton *radSetPhyml;
    QRadioButton *radSetPAUP;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label_2;
    QFrame *frame_settings_2;
    QHBoxLayout *horizontalLayout_4;
    QRadioButton *radTopoFixedMp;
    QRadioButton *radTopoFixedJc;
    QRadioButton *radTopoFixedGtr;
    QRadioButton *radTopoML;
    QRadioButton *radTopoU;
    QSpacerItem *horizontalSpacer;
    QFrame *line;
    QLabel *label_3;
    QFrame *frameModels;
    QVBoxLayout *verticalLayout_11;
    QFrame *grpSubstSchemes;
    QHBoxLayout *horizontalLayout_5;
    QRadioButton *radSchemes3;
    QRadioButton *radSchemes5;
    QRadioButton *radSchemes7;
    QRadioButton *radSchemes11;
    QRadioButton *radSchemes203;
    QRadioButton *radSchemesUser;
    QSpacerItem *horizontalSpacer_5;
    QLabel *label_4;
    QFrame *frame_3;
    QGridLayout *gridLayout_6;
    QCheckBox *cbNoRateVarModels;
    QCheckBox *cbIGModels;
    QCheckBox *cbIModels;
    QSpacerItem *horizontalSpacer_6;
    QLabel *lblNCat;
    QCheckBox *cbGModels;
    QSlider *sliderNCat;
    QLabel *label_5;
    QFrame *frame_4;
    QHBoxLayout *horizontalLayout_6;
    QCheckBox *cbEqualFreq;
    QCheckBox *cbMlFreq;
    QSpacerItem *horizontalSpacer_7;
    QFrame *frame;
    QHBoxLayout *horizontalLayout_3;
    QLabel *lblTextNModels;
    QLabel *lblNumModels;
    QSpacerItem *horizontalSpacer_3;
    QCheckBox *cbShowMatrices;
    QListWidget *modelsListView;
    QFrame *line_2;
    QCheckBox *cbAdvanced;
    QFrame *grpAdvanced;
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
    QFrame *frameProgressHeader;
    QHBoxLayout *horizontalLayout_8;
    QCheckBox *cbShowSelection;
    QCheckBox *cbShowHetParams;
    QCheckBox *cbShowFreqs;
    QCheckBox *cbShowRates;
    QSpacerItem *horizontalSpacer_11;
    QComboBox *cmbProgressPartitions;
    QTableView *tblModels;
    QWidget *tab_results;
    QVBoxLayout *verticalLayout_12;
    QFrame *frameResultsHeader;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer_10;
    QComboBox *cmbResultsPartition;
    QTabWidget *results_content;
    QWidget *tabResultsBic;
    QVBoxLayout *verticalLayout_4;
    QTableView *tblResultsBic;
    QFrame *frame_2;
    QGridLayout *gridLayout_5;
    QLabel *lblImportances;
    QLabel *lblImpGamma;
    QSpacerItem *spacerImp;
    QLabel *lblImpFreqs;
    QLabel *txtImpFreqs;
    QLabel *txtImpGamma;
    QLabel *lblImpInv;
    QLabel *txtImpInv;
    QLabel *lblImpInvGamma;
    QLabel *txtImpInvGamma;
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
    QTextBrowser *debugConsole;
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
        jModelTest->resize(670, 800);
        jModelTest->setMinimumSize(QSize(670, 600));
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
        frameHeader = new QFrame(layout_main);
        frameHeader->setObjectName(QString::fromUtf8("frameHeader"));
        frameHeader->setMinimumSize(QSize(0, 100));
        frameHeader->setMaximumSize(QSize(16777215, 100));
        frameHeader->setFrameShape(QFrame::StyledPanel);
        frameHeader->setFrameShadow(QFrame::Raised);
        horizontalLayout_7 = new QHBoxLayout(frameHeader);
        horizontalLayout_7->setSpacing(6);
        horizontalLayout_7->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        horizontalLayout_7->setContentsMargins(5, 5, 5, 5);
        lblGear = new QLabel(frameHeader);
        lblGear->setObjectName(QString::fromUtf8("lblGear"));
        lblGear->setMinimumSize(QSize(90, 90));
        lblGear->setMaximumSize(QSize(90, 90));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Arial"));
        font1.setPointSize(10);
        lblGear->setFont(font1);
        lblGear->setStyleSheet(QString::fromUtf8(""));
        lblGear->setFrameShape(QFrame::NoFrame);
        lblGear->setFrameShadow(QFrame::Plain);
        lblGear->setLineWidth(0);
        lblGear->setAlignment(Qt::AlignCenter);

        horizontalLayout_7->addWidget(lblGear);

        horizontalSpacer_9 = new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer_9);

        gridData = new QGridLayout();
        gridData->setSpacing(6);
        gridData->setObjectName(QString::fromUtf8("gridData"));
        gridData->setVerticalSpacing(5);
        gridData->setContentsMargins(-1, 5, -1, 5);
        lblAlignment = new QLabel(frameHeader);
        lblAlignment->setObjectName(QString::fromUtf8("lblAlignment"));
        lblAlignment->setMinimumSize(QSize(0, 20));
        lblAlignment->setMaximumSize(QSize(16777215, 20));
        QFont font2;
        font2.setFamily(QString::fromUtf8("Arial"));
        font2.setPointSize(10);
        font2.setBold(false);
        font2.setItalic(false);
        font2.setWeight(50);
        lblAlignment->setFont(font2);
        lblAlignment->setStyleSheet(QString::fromUtf8(""));
        lblAlignment->setMargin(5);

        gridData->addWidget(lblAlignment, 0, 3, 1, 1);

        lblTree = new QLabel(frameHeader);
        lblTree->setObjectName(QString::fromUtf8("lblTree"));
        lblTree->setMinimumSize(QSize(0, 20));
        lblTree->setMaximumSize(QSize(16777215, 20));
        lblTree->setFont(font2);
        lblTree->setStyleSheet(QString::fromUtf8(""));
        lblTree->setMargin(5);

        gridData->addWidget(lblTree, 1, 3, 1, 1);

        lblLoadAlignText = new QLabel(frameHeader);
        lblLoadAlignText->setObjectName(QString::fromUtf8("lblLoadAlignText"));
        lblLoadAlignText->setMaximumSize(QSize(70, 16777215));
        lblLoadAlignText->setFont(font2);
        lblLoadAlignText->setStyleSheet(QString::fromUtf8("color: #060;"));
        lblLoadAlignText->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lblLoadAlignText->setMargin(5);

        gridData->addWidget(lblLoadAlignText, 0, 1, 1, 1);

        lblParts = new QLabel(frameHeader);
        lblParts->setObjectName(QString::fromUtf8("lblParts"));
        lblParts->setMinimumSize(QSize(0, 20));
        lblParts->setMaximumSize(QSize(16777215, 20));
        lblParts->setFont(font2);
        lblParts->setStyleSheet(QString::fromUtf8(""));
        lblParts->setMargin(5);

        gridData->addWidget(lblParts, 2, 3, 1, 1);

        lblLoadTreeText = new QLabel(frameHeader);
        lblLoadTreeText->setObjectName(QString::fromUtf8("lblLoadTreeText"));
        lblLoadTreeText->setMaximumSize(QSize(70, 16777215));
        lblLoadTreeText->setFont(font2);
        lblLoadTreeText->setStyleSheet(QString::fromUtf8("color: #060;"));
        lblLoadTreeText->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lblLoadTreeText->setMargin(5);

        gridData->addWidget(lblLoadTreeText, 1, 1, 1, 1);

        lblLoadPartsText = new QLabel(frameHeader);
        lblLoadPartsText->setObjectName(QString::fromUtf8("lblLoadPartsText"));
        lblLoadPartsText->setMaximumSize(QSize(70, 16777215));
        lblLoadPartsText->setFont(font2);
        lblLoadPartsText->setStyleSheet(QString::fromUtf8("color: #060;"));
        lblLoadPartsText->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        lblLoadPartsText->setMargin(5);

        gridData->addWidget(lblLoadPartsText, 2, 1, 1, 1);

        btnLoadAlignment = new QPushButton(frameHeader);
        btnLoadAlignment->setObjectName(QString::fromUtf8("btnLoadAlignment"));
        btnLoadAlignment->setEnabled(true);
        btnLoadAlignment->setMinimumSize(QSize(50, 0));
        btnLoadAlignment->setMaximumSize(QSize(50, 16777215));
        QFont font3;
        font3.setFamily(QString::fromUtf8("Arial"));
        font3.setPointSize(10);
        font3.setItalic(false);
        btnLoadAlignment->setFont(font3);
        btnLoadAlignment->setStyleSheet(QString::fromUtf8(""));

        gridData->addWidget(btnLoadAlignment, 0, 0, 1, 1);

        btnLoadTree = new QPushButton(frameHeader);
        btnLoadTree->setObjectName(QString::fromUtf8("btnLoadTree"));
        btnLoadTree->setEnabled(true);
        btnLoadTree->setMinimumSize(QSize(50, 0));
        btnLoadTree->setMaximumSize(QSize(50, 16777215));
        btnLoadTree->setFont(font3);

        gridData->addWidget(btnLoadTree, 1, 0, 1, 1);

        btnLoadParts = new QPushButton(frameHeader);
        btnLoadParts->setObjectName(QString::fromUtf8("btnLoadParts"));
        btnLoadParts->setEnabled(true);
        btnLoadParts->setMinimumSize(QSize(50, 0));
        btnLoadParts->setMaximumSize(QSize(50, 16777215));
        btnLoadParts->setFont(font3);
        btnLoadParts->setStyleSheet(QString::fromUtf8(""));

        gridData->addWidget(btnLoadParts, 2, 0, 1, 1);


        horizontalLayout_7->addLayout(gridData);


        verticalLayout_2->addWidget(frameHeader);

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
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 594, 558));
        verticalLayout_8 = new QVBoxLayout(scrollAreaWidgetContents);
        verticalLayout_8->setSpacing(6);
        verticalLayout_8->setContentsMargins(11, 11, 11, 11);
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        frameData = new QFrame(scrollAreaWidgetContents);
        frameData->setObjectName(QString::fromUtf8("frameData"));
        frameData->setMinimumSize(QSize(0, 100));
        frameData->setMaximumSize(QSize(16777215, 140));
        QFont font4;
        font4.setFamily(QString::fromUtf8("Courier 10 Pitch"));
        font4.setPointSize(10);
        frameData->setFont(font4);
        frameData->setFrameShape(QFrame::StyledPanel);
        frameData->setFrameShadow(QFrame::Plain);
        frameData->setLineWidth(0);
        verticalLayout = new QVBoxLayout(frameData);
        verticalLayout->setSpacing(2);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(2, 2, 2, 2);
        frame_5 = new QFrame(frameData);
        frame_5->setObjectName(QString::fromUtf8("frame_5"));
        frame_5->setMinimumSize(QSize(0, 20));
        frame_5->setMaximumSize(QSize(16777215, 30));
        frame_5->setFrameShape(QFrame::NoFrame);
        frame_5->setFrameShadow(QFrame::Raised);
        frame_5->setLineWidth(0);
        horizontalLayout_9 = new QHBoxLayout(frame_5);
        horizontalLayout_9->setSpacing(6);
        horizontalLayout_9->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        horizontalLayout_9->setContentsMargins(-1, 0, -1, 0);
        label_6 = new QLabel(frame_5);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setMinimumSize(QSize(100, 0));
        QFont font5;
        font5.setFamily(QString::fromUtf8("Courier 10 Pitch"));
        font5.setPointSize(10);
        font5.setBold(false);
        font5.setWeight(50);
        label_6->setFont(font5);

        horizontalLayout_9->addWidget(label_6);

        radDatatypeDna = new QRadioButton(frame_5);
        radDatatypeDna->setObjectName(QString::fromUtf8("radDatatypeDna"));
        QFont font6;
        font6.setFamily(QString::fromUtf8("Courier 10 Pitch"));
        font6.setPointSize(10);
        font6.setBold(false);
        font6.setItalic(false);
        font6.setWeight(50);
        radDatatypeDna->setFont(font6);
        radDatatypeDna->setCursor(QCursor(Qt::PointingHandCursor));
        radDatatypeDna->setChecked(true);

        horizontalLayout_9->addWidget(radDatatypeDna);

        radDatatypeProt = new QRadioButton(frame_5);
        radDatatypeProt->setObjectName(QString::fromUtf8("radDatatypeProt"));
        radDatatypeProt->setFont(font6);
        radDatatypeProt->setCursor(QCursor(Qt::PointingHandCursor));

        horizontalLayout_9->addWidget(radDatatypeProt);

        horizontalSpacer_8 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_9->addItem(horizontalSpacer_8);


        verticalLayout->addWidget(frame_5);

        frame_settings_1 = new QFrame(frameData);
        frame_settings_1->setObjectName(QString::fromUtf8("frame_settings_1"));
        frame_settings_1->setMinimumSize(QSize(0, 20));
        frame_settings_1->setMaximumSize(QSize(16777215, 30));
        frame_settings_1->setFrameShape(QFrame::NoFrame);
        frame_settings_1->setFrameShadow(QFrame::Raised);
        horizontalLayout_13 = new QHBoxLayout(frame_settings_1);
        horizontalLayout_13->setSpacing(6);
        horizontalLayout_13->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_13->setObjectName(QString::fromUtf8("horizontalLayout_13"));
        horizontalLayout_13->setContentsMargins(-1, 0, -1, 0);
        label = new QLabel(frame_settings_1);
        label->setObjectName(QString::fromUtf8("label"));
        label->setMinimumSize(QSize(100, 0));
        label->setFont(font5);

        horizontalLayout_13->addWidget(label);

        radSetModelTest = new QRadioButton(frame_settings_1);
        radSetModelTest->setObjectName(QString::fromUtf8("radSetModelTest"));
        radSetModelTest->setFont(font6);
        radSetModelTest->setStyleSheet(QString::fromUtf8("border: none;"));
        radSetModelTest->setChecked(true);

        horizontalLayout_13->addWidget(radSetModelTest);

        radSetMrbayes = new QRadioButton(frame_settings_1);
        radSetMrbayes->setObjectName(QString::fromUtf8("radSetMrbayes"));
        radSetMrbayes->setFont(font6);
        radSetMrbayes->setStyleSheet(QString::fromUtf8("border: none;"));

        horizontalLayout_13->addWidget(radSetMrbayes);

        radSetRaxml = new QRadioButton(frame_settings_1);
        radSetRaxml->setObjectName(QString::fromUtf8("radSetRaxml"));
        radSetRaxml->setFont(font6);
        radSetRaxml->setStyleSheet(QString::fromUtf8("border: none;"));

        horizontalLayout_13->addWidget(radSetRaxml);

        radSetPhyml = new QRadioButton(frame_settings_1);
        radSetPhyml->setObjectName(QString::fromUtf8("radSetPhyml"));
        radSetPhyml->setFont(font6);
        radSetPhyml->setStyleSheet(QString::fromUtf8("border: none;"));

        horizontalLayout_13->addWidget(radSetPhyml);

        radSetPAUP = new QRadioButton(frame_settings_1);
        radSetPAUP->setObjectName(QString::fromUtf8("radSetPAUP"));
        radSetPAUP->setFont(font6);
        radSetPAUP->setStyleSheet(QString::fromUtf8("border: none;"));

        horizontalLayout_13->addWidget(radSetPAUP);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_13->addItem(horizontalSpacer_2);


        verticalLayout->addWidget(frame_settings_1);

        label_2 = new QLabel(frameData);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setMinimumSize(QSize(0, 20));
        label_2->setMaximumSize(QSize(16777215, 30));
        QFont font7;
        font7.setFamily(QString::fromUtf8("Courier 10 Pitch"));
        font7.setPointSize(10);
        font7.setBold(false);
        font7.setItalic(true);
        font7.setUnderline(true);
        font7.setWeight(50);
        font7.setKerning(true);
        label_2->setFont(font7);

        verticalLayout->addWidget(label_2);

        frame_settings_2 = new QFrame(frameData);
        frame_settings_2->setObjectName(QString::fromUtf8("frame_settings_2"));
        frame_settings_2->setMinimumSize(QSize(0, 20));
        frame_settings_2->setMaximumSize(QSize(16777215, 30));
        frame_settings_2->setFrameShape(QFrame::NoFrame);
        frame_settings_2->setFrameShadow(QFrame::Raised);
        frame_settings_2->setLineWidth(0);
        horizontalLayout_4 = new QHBoxLayout(frame_settings_2);
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(-1, 0, -1, 0);
        radTopoFixedMp = new QRadioButton(frame_settings_2);
        radTopoFixedMp->setObjectName(QString::fromUtf8("radTopoFixedMp"));
        radTopoFixedMp->setFont(font6);

        horizontalLayout_4->addWidget(radTopoFixedMp);

        radTopoFixedJc = new QRadioButton(frame_settings_2);
        radTopoFixedJc->setObjectName(QString::fromUtf8("radTopoFixedJc"));
        radTopoFixedJc->setFont(font6);

        horizontalLayout_4->addWidget(radTopoFixedJc);

        radTopoFixedGtr = new QRadioButton(frame_settings_2);
        radTopoFixedGtr->setObjectName(QString::fromUtf8("radTopoFixedGtr"));
        radTopoFixedGtr->setFont(font6);
        radTopoFixedGtr->setChecked(true);

        horizontalLayout_4->addWidget(radTopoFixedGtr);

        radTopoML = new QRadioButton(frame_settings_2);
        radTopoML->setObjectName(QString::fromUtf8("radTopoML"));
        radTopoML->setFont(font6);
        radTopoML->setChecked(false);

        horizontalLayout_4->addWidget(radTopoML);

        radTopoU = new QRadioButton(frame_settings_2);
        radTopoU->setObjectName(QString::fromUtf8("radTopoU"));
        radTopoU->setFont(font6);

        horizontalLayout_4->addWidget(radTopoU);

        horizontalSpacer = new QSpacerItem(162, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer);


        verticalLayout->addWidget(frame_settings_2);


        verticalLayout_8->addWidget(frameData);

        line = new QFrame(scrollAreaWidgetContents);
        line->setObjectName(QString::fromUtf8("line"));
        line->setMinimumSize(QSize(0, 10));
        line->setFrameShadow(QFrame::Sunken);
        line->setLineWidth(1);
        line->setFrameShape(QFrame::HLine);

        verticalLayout_8->addWidget(line);

        label_3 = new QLabel(scrollAreaWidgetContents);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setMinimumSize(QSize(0, 20));
        label_3->setMaximumSize(QSize(16777215, 20));
        QFont font8;
        font8.setFamily(QString::fromUtf8("Courier 10 Pitch"));
        font8.setPointSize(11);
        font8.setBold(true);
        font8.setItalic(true);
        font8.setUnderline(false);
        font8.setWeight(75);
        label_3->setFont(font8);
        label_3->setStyleSheet(QString::fromUtf8("border-bottom: 1px dotted"));

        verticalLayout_8->addWidget(label_3);

        frameModels = new QFrame(scrollAreaWidgetContents);
        frameModels->setObjectName(QString::fromUtf8("frameModels"));
        frameModels->setStyleSheet(QString::fromUtf8("background: #fee;"));
        verticalLayout_11 = new QVBoxLayout(frameModels);
        verticalLayout_11->setSpacing(2);
        verticalLayout_11->setContentsMargins(11, 11, 11, 11);
        verticalLayout_11->setObjectName(QString::fromUtf8("verticalLayout_11"));
        verticalLayout_11->setContentsMargins(2, 2, 2, 2);
        grpSubstSchemes = new QFrame(frameModels);
        grpSubstSchemes->setObjectName(QString::fromUtf8("grpSubstSchemes"));
        grpSubstSchemes->setEnabled(true);
        grpSubstSchemes->setMinimumSize(QSize(0, 20));
        grpSubstSchemes->setMaximumSize(QSize(16777215, 20));
        grpSubstSchemes->setFrameShape(QFrame::NoFrame);
        grpSubstSchemes->setFrameShadow(QFrame::Plain);
        grpSubstSchemes->setLineWidth(0);
        horizontalLayout_5 = new QHBoxLayout(grpSubstSchemes);
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        horizontalLayout_5->setContentsMargins(-1, 2, -1, 2);
        radSchemes3 = new QRadioButton(grpSubstSchemes);
        radSchemes3->setObjectName(QString::fromUtf8("radSchemes3"));
        radSchemes3->setFont(font6);

        horizontalLayout_5->addWidget(radSchemes3);

        radSchemes5 = new QRadioButton(grpSubstSchemes);
        radSchemes5->setObjectName(QString::fromUtf8("radSchemes5"));
        radSchemes5->setFont(font6);

        horizontalLayout_5->addWidget(radSchemes5);

        radSchemes7 = new QRadioButton(grpSubstSchemes);
        radSchemes7->setObjectName(QString::fromUtf8("radSchemes7"));
        radSchemes7->setFont(font6);

        horizontalLayout_5->addWidget(radSchemes7);

        radSchemes11 = new QRadioButton(grpSubstSchemes);
        radSchemes11->setObjectName(QString::fromUtf8("radSchemes11"));
        radSchemes11->setFont(font6);
        radSchemes11->setChecked(true);

        horizontalLayout_5->addWidget(radSchemes11);

        radSchemes203 = new QRadioButton(grpSubstSchemes);
        radSchemes203->setObjectName(QString::fromUtf8("radSchemes203"));
        radSchemes203->setFont(font6);
        radSchemes203->setChecked(false);

        horizontalLayout_5->addWidget(radSchemes203);

        radSchemesUser = new QRadioButton(grpSubstSchemes);
        radSchemesUser->setObjectName(QString::fromUtf8("radSchemesUser"));
        radSchemesUser->setFont(font6);
        radSchemesUser->setChecked(false);

        horizontalLayout_5->addWidget(radSchemesUser);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_5);


        verticalLayout_11->addWidget(grpSubstSchemes);

        label_4 = new QLabel(frameModels);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setMinimumSize(QSize(0, 20));
        label_4->setMaximumSize(QSize(16777215, 20));
        QFont font9;
        font9.setFamily(QString::fromUtf8("Courier 10 Pitch"));
        font9.setPointSize(10);
        font9.setBold(false);
        font9.setItalic(true);
        font9.setUnderline(true);
        font9.setWeight(50);
        label_4->setFont(font9);

        verticalLayout_11->addWidget(label_4);

        frame_3 = new QFrame(frameModels);
        frame_3->setObjectName(QString::fromUtf8("frame_3"));
        frame_3->setMinimumSize(QSize(0, 50));
        frame_3->setMaximumSize(QSize(16777215, 50));
        frame_3->setFrameShape(QFrame::NoFrame);
        frame_3->setFrameShadow(QFrame::Plain);
        frame_3->setLineWidth(0);
        gridLayout_6 = new QGridLayout(frame_3);
        gridLayout_6->setSpacing(6);
        gridLayout_6->setContentsMargins(11, 11, 11, 11);
        gridLayout_6->setObjectName(QString::fromUtf8("gridLayout_6"));
        gridLayout_6->setContentsMargins(-1, 2, -1, 2);
        cbNoRateVarModels = new QCheckBox(frame_3);
        cbNoRateVarModels->setObjectName(QString::fromUtf8("cbNoRateVarModels"));
        cbNoRateVarModels->setMinimumSize(QSize(0, 22));
        cbNoRateVarModels->setMaximumSize(QSize(160, 16777215));
        cbNoRateVarModels->setFont(font6);
        cbNoRateVarModels->setChecked(true);

        gridLayout_6->addWidget(cbNoRateVarModels, 0, 0, 1, 1);

        cbIGModels = new QCheckBox(frame_3);
        cbIGModels->setObjectName(QString::fromUtf8("cbIGModels"));
        cbIGModels->setMinimumSize(QSize(0, 22));
        cbIGModels->setFont(font6);
        cbIGModels->setChecked(true);

        gridLayout_6->addWidget(cbIGModels, 0, 3, 1, 1);

        cbIModels = new QCheckBox(frame_3);
        cbIModels->setObjectName(QString::fromUtf8("cbIModels"));
        cbIModels->setMinimumSize(QSize(0, 22));
        cbIModels->setMaximumSize(QSize(180, 16777215));
        cbIModels->setFont(font6);
        cbIModels->setChecked(true);

        gridLayout_6->addWidget(cbIModels, 0, 2, 1, 1);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_6->addItem(horizontalSpacer_6, 0, 6, 1, 1);

        lblNCat = new QLabel(frame_3);
        lblNCat->setObjectName(QString::fromUtf8("lblNCat"));
        lblNCat->setMinimumSize(QSize(100, 20));
        lblNCat->setMaximumSize(QSize(100, 20));
        lblNCat->setFont(font6);

        gridLayout_6->addWidget(lblNCat, 1, 2, 1, 1);

        cbGModels = new QCheckBox(frame_3);
        cbGModels->setObjectName(QString::fromUtf8("cbGModels"));
        cbGModels->setMinimumSize(QSize(0, 22));
        cbGModels->setMaximumSize(QSize(220, 16777215));
        cbGModels->setFont(font6);
        cbGModels->setChecked(true);

        gridLayout_6->addWidget(cbGModels, 0, 1, 1, 1);

        sliderNCat = new QSlider(frame_3);
        sliderNCat->setObjectName(QString::fromUtf8("sliderNCat"));
        sliderNCat->setMinimumSize(QSize(100, 20));
        sliderNCat->setMaximumSize(QSize(100, 20));
        sliderNCat->setFont(font6);
        sliderNCat->setAutoFillBackground(false);
        sliderNCat->setMinimum(2);
        sliderNCat->setMaximum(20);
        sliderNCat->setValue(4);
        sliderNCat->setOrientation(Qt::Horizontal);

        gridLayout_6->addWidget(sliderNCat, 1, 1, 1, 1);

        cbIModels->raise();
        cbNoRateVarModels->raise();
        cbGModels->raise();
        cbIGModels->raise();
        sliderNCat->raise();
        lblNCat->raise();

        verticalLayout_11->addWidget(frame_3);

        label_5 = new QLabel(frameModels);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setMinimumSize(QSize(0, 20));
        label_5->setMaximumSize(QSize(16777215, 20));
        label_5->setFont(font9);

        verticalLayout_11->addWidget(label_5);

        frame_4 = new QFrame(frameModels);
        frame_4->setObjectName(QString::fromUtf8("frame_4"));
        frame_4->setMinimumSize(QSize(0, 25));
        frame_4->setMaximumSize(QSize(16777215, 25));
        frame_4->setFrameShape(QFrame::NoFrame);
        frame_4->setFrameShadow(QFrame::Plain);
        frame_4->setLineWidth(0);
        horizontalLayout_6 = new QHBoxLayout(frame_4);
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        horizontalLayout_6->setContentsMargins(-1, 2, -1, 2);
        cbEqualFreq = new QCheckBox(frame_4);
        cbEqualFreq->setObjectName(QString::fromUtf8("cbEqualFreq"));
        cbEqualFreq->setMinimumSize(QSize(160, 22));
        cbEqualFreq->setMaximumSize(QSize(160, 16777215));
        cbEqualFreq->setFont(font6);
        cbEqualFreq->setChecked(true);

        horizontalLayout_6->addWidget(cbEqualFreq);

        cbMlFreq = new QCheckBox(frame_4);
        cbMlFreq->setObjectName(QString::fromUtf8("cbMlFreq"));
        cbMlFreq->setMinimumSize(QSize(160, 22));
        cbMlFreq->setMaximumSize(QSize(180, 16777215));
        cbMlFreq->setFont(font6);
        cbMlFreq->setChecked(true);

        horizontalLayout_6->addWidget(cbMlFreq);

        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_7);


        verticalLayout_11->addWidget(frame_4);

        frame = new QFrame(frameModels);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setMinimumSize(QSize(0, 30));
        frame->setMaximumSize(QSize(16777215, 30));
        horizontalLayout_3 = new QHBoxLayout(frame);
        horizontalLayout_3->setSpacing(0);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(-1, 2, -1, 2);
        lblTextNModels = new QLabel(frame);
        lblTextNModels->setObjectName(QString::fromUtf8("lblTextNModels"));
        lblTextNModels->setMinimumSize(QSize(150, 22));
        lblTextNModels->setMaximumSize(QSize(150, 22));
        lblTextNModels->setFont(font6);

        horizontalLayout_3->addWidget(lblTextNModels);

        lblNumModels = new QLabel(frame);
        lblNumModels->setObjectName(QString::fromUtf8("lblNumModels"));
        lblNumModels->setMinimumSize(QSize(100, 22));
        lblNumModels->setMaximumSize(QSize(16777215, 22));
        lblNumModels->setFont(font6);

        horizontalLayout_3->addWidget(lblNumModels);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_3);


        verticalLayout_11->addWidget(frame);

        cbShowMatrices = new QCheckBox(frameModels);
        cbShowMatrices->setObjectName(QString::fromUtf8("cbShowMatrices"));
        cbShowMatrices->setMinimumSize(QSize(0, 20));
        cbShowMatrices->setMaximumSize(QSize(16777215, 20));
        cbShowMatrices->setFont(font6);

        verticalLayout_11->addWidget(cbShowMatrices);

        modelsListView = new QListWidget(frameModels);
        modelsListView->setObjectName(QString::fromUtf8("modelsListView"));
        QFont font10;
        font10.setFamily(QString::fromUtf8("Courier"));
        font10.setPointSize(10);
        font10.setBold(true);
        font10.setWeight(75);
        modelsListView->setFont(font10);
        modelsListView->setFrameShape(QFrame::NoFrame);
        modelsListView->setFrameShadow(QFrame::Plain);
        modelsListView->setLineWidth(0);
        modelsListView->setSelectionMode(QAbstractItemView::NoSelection);

        verticalLayout_11->addWidget(modelsListView, 0, Qt::AlignHCenter|Qt::AlignVCenter);


        verticalLayout_8->addWidget(frameModels);

        line_2 = new QFrame(scrollAreaWidgetContents);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setFrameShadow(QFrame::Sunken);
        line_2->setFrameShape(QFrame::HLine);

        verticalLayout_8->addWidget(line_2);

        cbAdvanced = new QCheckBox(scrollAreaWidgetContents);
        cbAdvanced->setObjectName(QString::fromUtf8("cbAdvanced"));
        cbAdvanced->setMinimumSize(QSize(205, 22));
        cbAdvanced->setMaximumSize(QSize(16777215, 22));
        QFont font11;
        font11.setFamily(QString::fromUtf8("Courier"));
        font11.setPointSize(10);
        font11.setBold(true);
        font11.setItalic(true);
        font11.setWeight(75);
        cbAdvanced->setFont(font11);

        verticalLayout_8->addWidget(cbAdvanced);

        grpAdvanced = new QFrame(scrollAreaWidgetContents);
        grpAdvanced->setObjectName(QString::fromUtf8("grpAdvanced"));
        grpAdvanced->setMinimumSize(QSize(0, 70));
        grpAdvanced->setMaximumSize(QSize(16777215, 135));
        grpAdvanced->setFont(font1);
        grpAdvanced->setStyleSheet(QString::fromUtf8("background-color: #99d;"));
        gridLayout_2 = new QGridLayout(grpAdvanced);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setVerticalSpacing(0);
        gridLayout_2->setContentsMargins(-1, 0, -1, 0);
        txtOptEpsilon = new QLineEdit(grpAdvanced);
        txtOptEpsilon->setObjectName(QString::fromUtf8("txtOptEpsilon"));
        txtOptEpsilon->setMinimumSize(QSize(0, 20));
        txtOptEpsilon->setMaximumSize(QSize(150, 25));
        QFont font12;
        font12.setFamily(QString::fromUtf8("Courier"));
        font12.setPointSize(10);
        font12.setBold(true);
        font12.setItalic(false);
        font12.setWeight(75);
        txtOptEpsilon->setFont(font12);
        txtOptEpsilon->setText(QString::fromUtf8("0.01"));
        txtOptEpsilon->setMaxLength(10);
        txtOptEpsilon->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(txtOptEpsilon, 2, 1, 1, 1);

        txtParEpsilon = new QLineEdit(grpAdvanced);
        txtParEpsilon->setObjectName(QString::fromUtf8("txtParEpsilon"));
        txtParEpsilon->setMinimumSize(QSize(0, 20));
        txtParEpsilon->setMaximumSize(QSize(150, 25));
        txtParEpsilon->setFont(font12);
        txtParEpsilon->setText(QString::fromUtf8("0.001"));
        txtParEpsilon->setMaxLength(10);
        txtParEpsilon->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(txtParEpsilon, 1, 1, 1, 1);

        lblParEpsText = new QLabel(grpAdvanced);
        lblParEpsText->setObjectName(QString::fromUtf8("lblParEpsText"));
        lblParEpsText->setMinimumSize(QSize(0, 20));
        lblParEpsText->setMaximumSize(QSize(16777215, 25));
        lblParEpsText->setFont(font12);

        gridLayout_2->addWidget(lblParEpsText, 1, 0, 1, 1);

        lblOptEpsText = new QLabel(grpAdvanced);
        lblOptEpsText->setObjectName(QString::fromUtf8("lblOptEpsText"));
        lblOptEpsText->setMinimumSize(QSize(0, 20));
        lblOptEpsText->setMaximumSize(QSize(16777215, 25));
        lblOptEpsText->setFont(font12);

        gridLayout_2->addWidget(lblOptEpsText, 2, 0, 1, 1);

        lblNThreads = new QLabel(grpAdvanced);
        lblNThreads->setObjectName(QString::fromUtf8("lblNThreads"));
        lblNThreads->setMinimumSize(QSize(100, 20));
        lblNThreads->setMaximumSize(QSize(16777215, 25));
        lblNThreads->setFont(font10);

        gridLayout_2->addWidget(lblNThreads, 0, 2, 1, 1);

        sliderNThreads = new QSlider(grpAdvanced);
        sliderNThreads->setObjectName(QString::fromUtf8("sliderNThreads"));
        sliderNThreads->setMinimumSize(QSize(150, 20));
        sliderNThreads->setMaximumSize(QSize(150, 25));
        sliderNThreads->setFont(font1);
        sliderNThreads->setAutoFillBackground(false);
        sliderNThreads->setMinimum(1);
        sliderNThreads->setMaximum(10);
        sliderNThreads->setValue(2);
        sliderNThreads->setOrientation(Qt::Horizontal);

        gridLayout_2->addWidget(sliderNThreads, 0, 1, 1, 1);

        lblNThreadText = new QLabel(grpAdvanced);
        lblNThreadText->setObjectName(QString::fromUtf8("lblNThreadText"));
        lblNThreadText->setMinimumSize(QSize(0, 20));
        lblNThreadText->setMaximumSize(QSize(16777215, 25));
        lblNThreadText->setFont(font12);

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
        icon1.addFile(QString::fromUtf8(":/IMG/IMG/tool-icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        tabView->addTab(tab_configure, icon1, QString());
        tab_runsingle = new QWidget();
        tab_runsingle->setObjectName(QString::fromUtf8("tab_runsingle"));
        verticalLayout_3 = new QVBoxLayout(tab_runsingle);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        frameProgressHeader = new QFrame(tab_runsingle);
        frameProgressHeader->setObjectName(QString::fromUtf8("frameProgressHeader"));
        frameProgressHeader->setMinimumSize(QSize(0, 25));
        frameProgressHeader->setMaximumSize(QSize(16777213, 25));
        horizontalLayout_8 = new QHBoxLayout(frameProgressHeader);
        horizontalLayout_8->setSpacing(5);
        horizontalLayout_8->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        horizontalLayout_8->setContentsMargins(0, 0, 0, 0);
        cbShowSelection = new QCheckBox(frameProgressHeader);
        cbShowSelection->setObjectName(QString::fromUtf8("cbShowSelection"));
        cbShowSelection->setMinimumSize(QSize(0, 20));
        cbShowSelection->setMaximumSize(QSize(16777215, 20));
        QFont font13;
        font13.setFamily(QString::fromUtf8("Arial"));
        font13.setItalic(false);
        cbShowSelection->setFont(font13);
        cbShowSelection->setChecked(true);

        horizontalLayout_8->addWidget(cbShowSelection);

        cbShowHetParams = new QCheckBox(frameProgressHeader);
        cbShowHetParams->setObjectName(QString::fromUtf8("cbShowHetParams"));
        cbShowHetParams->setMinimumSize(QSize(0, 20));
        cbShowHetParams->setMaximumSize(QSize(16777215, 20));
        cbShowHetParams->setFont(font13);

        horizontalLayout_8->addWidget(cbShowHetParams);

        cbShowFreqs = new QCheckBox(frameProgressHeader);
        cbShowFreqs->setObjectName(QString::fromUtf8("cbShowFreqs"));
        cbShowFreqs->setMinimumSize(QSize(0, 20));
        cbShowFreqs->setMaximumSize(QSize(16777215, 20));
        cbShowFreqs->setFont(font13);

        horizontalLayout_8->addWidget(cbShowFreqs);

        cbShowRates = new QCheckBox(frameProgressHeader);
        cbShowRates->setObjectName(QString::fromUtf8("cbShowRates"));
        cbShowRates->setMinimumSize(QSize(0, 20));
        cbShowRates->setMaximumSize(QSize(16777215, 20));
        cbShowRates->setFont(font13);

        horizontalLayout_8->addWidget(cbShowRates);

        horizontalSpacer_11 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_8->addItem(horizontalSpacer_11);

        cmbProgressPartitions = new QComboBox(frameProgressHeader);
        cmbProgressPartitions->setObjectName(QString::fromUtf8("cmbProgressPartitions"));
        cmbProgressPartitions->setMinimumSize(QSize(200, 0));
        cmbProgressPartitions->setMaximumSize(QSize(200, 16777215));

        horizontalLayout_8->addWidget(cmbProgressPartitions);


        verticalLayout_3->addWidget(frameProgressHeader);

        tblModels = new QTableView(tab_runsingle);
        tblModels->setObjectName(QString::fromUtf8("tblModels"));
        tblModels->setFont(font4);
        tblModels->setAlternatingRowColors(true);
        tblModels->setSortingEnabled(true);

        verticalLayout_3->addWidget(tblModels);

        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/IMG/IMG/run-icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        tabView->addTab(tab_runsingle, icon2, QString());
        tab_results = new QWidget();
        tab_results->setObjectName(QString::fromUtf8("tab_results"));
        verticalLayout_12 = new QVBoxLayout(tab_results);
        verticalLayout_12->setSpacing(6);
        verticalLayout_12->setContentsMargins(11, 11, 11, 11);
        verticalLayout_12->setObjectName(QString::fromUtf8("verticalLayout_12"));
        frameResultsHeader = new QFrame(tab_results);
        frameResultsHeader->setObjectName(QString::fromUtf8("frameResultsHeader"));
        frameResultsHeader->setFrameShape(QFrame::StyledPanel);
        frameResultsHeader->setFrameShadow(QFrame::Raised);
        horizontalLayout = new QHBoxLayout(frameResultsHeader);
        horizontalLayout->setSpacing(5);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer_10 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_10);

        cmbResultsPartition = new QComboBox(frameResultsHeader);
        cmbResultsPartition->setObjectName(QString::fromUtf8("cmbResultsPartition"));
        cmbResultsPartition->setMinimumSize(QSize(200, 0));
        cmbResultsPartition->setMaximumSize(QSize(200, 16777215));

        horizontalLayout->addWidget(cmbResultsPartition);


        verticalLayout_12->addWidget(frameResultsHeader);

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
        tblResultsBic->setFont(font4);
        tblResultsBic->setAlternatingRowColors(true);
        tblResultsBic->setSortingEnabled(true);

        verticalLayout_4->addWidget(tblResultsBic);

        frame_2 = new QFrame(tabResultsBic);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        frame_2->setMinimumSize(QSize(0, 20));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        gridLayout_5 = new QGridLayout(frame_2);
        gridLayout_5->setSpacing(6);
        gridLayout_5->setContentsMargins(11, 11, 11, 11);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        gridLayout_5->setVerticalSpacing(6);
        lblImportances = new QLabel(frame_2);
        lblImportances->setObjectName(QString::fromUtf8("lblImportances"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(lblImportances->sizePolicy().hasHeightForWidth());
        lblImportances->setSizePolicy(sizePolicy);
        lblImportances->setMinimumSize(QSize(0, 0));
        QFont font14;
        font14.setBold(true);
        font14.setUnderline(false);
        font14.setWeight(75);
        lblImportances->setFont(font14);
        lblImportances->setLineWidth(1);

        gridLayout_5->addWidget(lblImportances, 0, 0, 1, 2, Qt::AlignHCenter);

        lblImpGamma = new QLabel(frame_2);
        lblImpGamma->setObjectName(QString::fromUtf8("lblImpGamma"));
        lblImpGamma->setMinimumSize(QSize(100, 0));

        gridLayout_5->addWidget(lblImpGamma, 3, 0, 1, 1);

        spacerImp = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_5->addItem(spacerImp, 3, 2, 1, 1);

        lblImpFreqs = new QLabel(frame_2);
        lblImpFreqs->setObjectName(QString::fromUtf8("lblImpFreqs"));
        lblImpFreqs->setMinimumSize(QSize(100, 0));

        gridLayout_5->addWidget(lblImpFreqs, 4, 0, 1, 1);

        txtImpFreqs = new QLabel(frame_2);
        txtImpFreqs->setObjectName(QString::fromUtf8("txtImpFreqs"));
        txtImpFreqs->setMinimumSize(QSize(60, 0));

        gridLayout_5->addWidget(txtImpFreqs, 4, 1, 1, 1);

        txtImpGamma = new QLabel(frame_2);
        txtImpGamma->setObjectName(QString::fromUtf8("txtImpGamma"));
        txtImpGamma->setMinimumSize(QSize(60, 0));

        gridLayout_5->addWidget(txtImpGamma, 3, 1, 1, 1);

        lblImpInv = new QLabel(frame_2);
        lblImpInv->setObjectName(QString::fromUtf8("lblImpInv"));
        lblImpInv->setMinimumSize(QSize(100, 0));

        gridLayout_5->addWidget(lblImpInv, 2, 0, 1, 1);

        txtImpInv = new QLabel(frame_2);
        txtImpInv->setObjectName(QString::fromUtf8("txtImpInv"));
        txtImpInv->setMinimumSize(QSize(60, 0));

        gridLayout_5->addWidget(txtImpInv, 2, 1, 1, 1);

        lblImpInvGamma = new QLabel(frame_2);
        lblImpInvGamma->setObjectName(QString::fromUtf8("lblImpInvGamma"));

        gridLayout_5->addWidget(lblImpInvGamma, 1, 0, 1, 1);

        txtImpInvGamma = new QLabel(frame_2);
        txtImpInvGamma->setObjectName(QString::fromUtf8("txtImpInvGamma"));

        gridLayout_5->addWidget(txtImpInvGamma, 1, 1, 1, 1);


        verticalLayout_4->addWidget(frame_2);

        results_content->addTab(tabResultsBic, QString());
        tabResultsAic = new QWidget();
        tabResultsAic->setObjectName(QString::fromUtf8("tabResultsAic"));
        verticalLayout_5 = new QVBoxLayout(tabResultsAic);
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setContentsMargins(11, 11, 11, 11);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        tblResultsAic = new QTableView(tabResultsAic);
        tblResultsAic->setObjectName(QString::fromUtf8("tblResultsAic"));
        tblResultsAic->setFont(font4);
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
        tblResultsAicc->setFont(font4);
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
        tblResultsDt->setFont(font4);
        tblResultsDt->setAlternatingRowColors(true);
        tblResultsDt->setSortingEnabled(true);

        verticalLayout_7->addWidget(tblResultsDt);

        results_content->addTab(tabResultsDt, QString());

        verticalLayout_12->addWidget(results_content);

        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/IMG/IMG/results-icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        tabView->addTab(tab_results, icon3, QString());
        tab_console = new QWidget();
        tab_console->setObjectName(QString::fromUtf8("tab_console"));
        horizontalLayout_2 = new QHBoxLayout(tab_console);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        debugConsole = new QTextBrowser(tab_console);
        debugConsole->setObjectName(QString::fromUtf8("debugConsole"));
        debugConsole->setEnabled(true);
        debugConsole->setFont(font4);
        debugConsole->setAutoFillBackground(false);
        debugConsole->setStyleSheet(QString::fromUtf8("background: #fff;"));

        horizontalLayout_2->addWidget(debugConsole);

        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/IMG/IMG/console-icon.png"), QSize(), QIcon::Normal, QIcon::Off);
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
        txt_messages->setMinimumSize(QSize(200, 100));
        txt_messages->setMaximumSize(QSize(10000, 10000));
        txt_messages->setFont(font1);
        txt_messages->setStyleSheet(QString::fromUtf8("background: #fff;"));

        horizontalLayout_10->addWidget(txt_messages);

        splitter->addWidget(widget_footer);

        verticalLayout_2->addWidget(splitter);

        jModelTest->setCentralWidget(layout_main);
        menuBar = new QMenuBar(jModelTest);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 670, 25));
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

        tabView->setCurrentIndex(2);
        results_content->setCurrentIndex(0);


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
        lblGear->setText(QString());
        lblAlignment->setText(QString());
        lblTree->setText(QString());
        lblLoadAlignText->setText(QApplication::translate("jModelTest", "MSA:", 0, QApplication::UnicodeUTF8));
        lblParts->setText(QString());
        lblLoadTreeText->setText(QApplication::translate("jModelTest", "Tree:", 0, QApplication::UnicodeUTF8));
        lblLoadPartsText->setText(QApplication::translate("jModelTest", "Partitions:", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        btnLoadAlignment->setToolTip(QApplication::translate("jModelTest", "<html><head/><body><p>Load multiple sequence alignment in <span style=\" font-weight:600;\">FASTA</span> format</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        btnLoadAlignment->setText(QApplication::translate("jModelTest", "Load", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        btnLoadTree->setToolTip(QApplication::translate("jModelTest", "<html><head/><body><p>Load tree file in <span style=\" font-weight:600;\">NEWICK</span> format</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        btnLoadTree->setText(QApplication::translate("jModelTest", "Load", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        btnLoadParts->setToolTip(QApplication::translate("jModelTest", "<html><head/><body><p>Load tree file in <span style=\" font-weight:600;\">NEWICK</span> format</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        btnLoadParts->setText(QApplication::translate("jModelTest", "Load", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("jModelTest", "Data type:", 0, QApplication::UnicodeUTF8));
        radDatatypeDna->setText(QApplication::translate("jModelTest", "DNA", 0, QApplication::UnicodeUTF8));
        radDatatypeProt->setText(QApplication::translate("jModelTest", "Protein", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("jModelTest", "Templates:", 0, QApplication::UnicodeUTF8));
        radSetModelTest->setText(QApplication::translate("jModelTest", "ModelTest", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        radSetMrbayes->setToolTip(QApplication::translate("jModelTest", "<html><head/><body><p>Select default options for MrBAYES</p></body></html>", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        radSetMrbayes->setText(QApplication::translate("jModelTest", "MrBAYES", 0, QApplication::UnicodeUTF8));
        radSetRaxml->setText(QApplication::translate("jModelTest", "RAxML", 0, QApplication::UnicodeUTF8));
        radSetPhyml->setText(QApplication::translate("jModelTest", "PhyML", 0, QApplication::UnicodeUTF8));
        radSetPAUP->setText(QApplication::translate("jModelTest", "PAUP*", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("jModelTest", "Starting topology", 0, QApplication::UnicodeUTF8));
        radTopoFixedMp->setText(QApplication::translate("jModelTest", "Fixed MP", 0, QApplication::UnicodeUTF8));
        radTopoFixedJc->setText(QApplication::translate("jModelTest", "Fixed ML (JC)", 0, QApplication::UnicodeUTF8));
        radTopoFixedGtr->setText(QApplication::translate("jModelTest", "Fixed ML (GTR)", 0, QApplication::UnicodeUTF8));
        radTopoML->setText(QApplication::translate("jModelTest", "ML", 0, QApplication::UnicodeUTF8));
        radTopoU->setText(QApplication::translate("jModelTest", "Custom", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("jModelTest", "Models", 0, QApplication::UnicodeUTF8));
        radSchemes3->setText(QApplication::translate("jModelTest", "3", 0, QApplication::UnicodeUTF8));
        radSchemes5->setText(QApplication::translate("jModelTest", "5", 0, QApplication::UnicodeUTF8));
        radSchemes7->setText(QApplication::translate("jModelTest", "7", 0, QApplication::UnicodeUTF8));
        radSchemes11->setText(QApplication::translate("jModelTest", "11", 0, QApplication::UnicodeUTF8));
        radSchemes203->setText(QApplication::translate("jModelTest", "203", 0, QApplication::UnicodeUTF8));
        radSchemesUser->setText(QApplication::translate("jModelTest", "User defined", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("jModelTest", "Rate variation", 0, QApplication::UnicodeUTF8));
        cbNoRateVarModels->setText(QApplication::translate("jModelTest", "uniform", 0, QApplication::UnicodeUTF8));
        cbIGModels->setText(QApplication::translate("jModelTest", "+I+G", 0, QApplication::UnicodeUTF8));
        cbIModels->setText(QApplication::translate("jModelTest", "pinv (+I)", 0, QApplication::UnicodeUTF8));
        lblNCat->setText(QApplication::translate("jModelTest", "4 categories", 0, QApplication::UnicodeUTF8));
        cbGModels->setText(QApplication::translate("jModelTest", "gamma (+G)", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("jModelTest", "Frequencies", 0, QApplication::UnicodeUTF8));
        cbEqualFreq->setText(QApplication::translate("jModelTest", "Equal frequencies", 0, QApplication::UnicodeUTF8));
        cbMlFreq->setText(QApplication::translate("jModelTest", "ML frequencies", 0, QApplication::UnicodeUTF8));
        lblTextNModels->setText(QApplication::translate("jModelTest", "Number of models:", 0, QApplication::UnicodeUTF8));
        lblNumModels->setText(QApplication::translate("jModelTest", "0", 0, QApplication::UnicodeUTF8));
        cbShowMatrices->setText(QApplication::translate("jModelTest", "Show model matrices", 0, QApplication::UnicodeUTF8));
        cbAdvanced->setText(QApplication::translate("jModelTest", "Show advanced options", 0, QApplication::UnicodeUTF8));
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
        cbShowSelection->setText(QApplication::translate("jModelTest", "Selection", 0, QApplication::UnicodeUTF8));
        cbShowHetParams->setText(QApplication::translate("jModelTest", "Het. params", 0, QApplication::UnicodeUTF8));
        cbShowFreqs->setText(QApplication::translate("jModelTest", "Frequencies", 0, QApplication::UnicodeUTF8));
        cbShowRates->setText(QApplication::translate("jModelTest", "Rates", 0, QApplication::UnicodeUTF8));
        tabView->setTabText(tabView->indexOf(tab_runsingle), QApplication::translate("jModelTest", "Progress", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        results_content->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        lblImportances->setText(QApplication::translate("jModelTest", "Importances", 0, QApplication::UnicodeUTF8));
        lblImpGamma->setText(QApplication::translate("jModelTest", "Gamma:", 0, QApplication::UnicodeUTF8));
        lblImpFreqs->setText(QApplication::translate("jModelTest", "Frequencies:", 0, QApplication::UnicodeUTF8));
        txtImpFreqs->setText(QApplication::translate("jModelTest", "N/A", 0, QApplication::UnicodeUTF8));
        txtImpGamma->setText(QApplication::translate("jModelTest", "N/A", 0, QApplication::UnicodeUTF8));
        lblImpInv->setText(QApplication::translate("jModelTest", "Prop.Inv:", 0, QApplication::UnicodeUTF8));
        txtImpInv->setText(QApplication::translate("jModelTest", "N/A", 0, QApplication::UnicodeUTF8));
        lblImpInvGamma->setText(QApplication::translate("jModelTest", "Inv+Gamma", 0, QApplication::UnicodeUTF8));
        txtImpInvGamma->setText(QApplication::translate("jModelTest", "N/A", 0, QApplication::UnicodeUTF8));
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
