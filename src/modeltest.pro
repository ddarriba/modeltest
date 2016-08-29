#-------------------------------------------------
#
# Project created by QtCreator 2015-05-31T18:30:19
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = modeltest-gui
TEMPLATE = app

SOURCES += main.cpp\
    utils.cpp \
    model/abstract_parameter.cpp \
    model/parameter_branches.cpp \
    model/parameter_gamma.cpp \
    model/parameter_ratecats.cpp \
    model/parameter_frequencies.cpp \
    model/parameter_pinv.cpp \
    model/parameter_substrates.cpp \
    model/model.cpp \
    optimize/model_optimizer_pll.cpp \
    modeltest.cpp \
    msapll.cpp \
    treepll.cpp \
    model_selection.cpp \
    partition.cpp \
    partitioning_scheme.cpp \
    static_analyzer.cpp \
    gui/xmodeltest.cpp \
    gui/xmodeltest_actions.cpp \
    gui/xthreadopt.cpp \
    gui/progressdialog.cpp \
    gui/datainfodialog.cpp \
    gui/xutils.cpp \
    gui/resultsdialog.cpp \
    partition_selection.cpp \
    service/modeltestservice.cpp \
    gui/modelsdialog.cpp \
    gui/resultsexportdialog.cpp \
    thread/observer.cpp \
    meta.cpp

HEADERS  += \
    model_defs.h \
    utils.h \
    model/abstract_parameter.h \
    model/parameter_branches.h \
    model/parameter_gamma.h \
    model/parameter_ratecats.h \
    model/parameter_frequencies.h \
    model/parameter_pinv.h \
    model/parameter_substrates.h \
    model/model.h \
    optimize/model_optimizer.h \
    optimize/model_optimizer_pll.h \
    modeltest.h \
    msa.h \
    msapll.h \
    plldefs.h \
    tree.h \
    treepll.h \
    model_selection.h \
    global_defs.h \
    partition.h \
    partitioning_scheme.h \
    static_analyzer.h \
    thread/threadpool.h \
    gui/qdebugstream.h \
    gui/xmodeltest.h \
    gui/mydebugstream.h \
    gui/xthreadopt.h \
    gui/progressdialog.h \
    thread/observer.h \
    gui/datainfodialog.h \
    gui/xutils.h \
    gui/resultsdialog.h \
    partition_selection.h \
    service/modeltestservice.h \
    gui/modelsdialog.h \
    loggable.h \
    gui/resultsexportdialog.h \
    meta.h

FORMS    += \
    gui/xmodeltest.ui \
    gui/progressdialog.ui \
    gui/datainfodialog.ui \
    gui/resultsdialog.ui \
    gui/modelsdialog.ui \
    gui/resultsexportdialog.ui

RESOURCES = mtgraphics.qrc

MOC_DIR = build
UI_DIR = build
OBJECTS_DIR = build
RCC_DIR = build

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../usr/local/lib/release/ -lpll
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../usr/local/lib/debug/ -lpll
#else:unix: LIBS += -L$$PWD/../../../../../usr/local/lib/ -lpll

CONFIG += c++11 -g
QMAKE_CXXFLAGS += -std=c++11 -g

#INCLUDEPATH += $$PWD/../../../../../usr/local/lib
#DEPENDPATH += $$PWD/../../../../../usr/local/lib

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../usr/local/lib/release/ -lpll_optimize
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../usr/local/lib/debug/ -lpll_optimize
#else:unix: LIBS += -L$$PWD/../../../../../usr/local/lib/ -lpll_optimize

#INCLUDEPATH += $$PWD/../../../../../usr/local/include
#DEPENDPATH += $$PWD/../../../../../usr/local/include

unix|win32: LIBS += -lpll -lpll_algorithm -lpll_optimize -lpll_msa -lpll_tree
