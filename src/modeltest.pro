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
    model.cpp \
    modeltest.cpp \
    model_optimizer_pll.cpp \
    msapll.cpp \
    treepll.cpp \
    model_selection.cpp \
    partition.cpp \
    partitioning_scheme.cpp \
    gui/xmodeltest.cpp \
    gui/xmodeltest_actions.cpp \
    gui/xthreadopt.cpp \
    gui/xmodeltest_fill.cpp \
    gui/progressdialog.cpp \
    gui/datainfodialog.cpp \
    gui/xutils.cpp \
    gui/resultsdialog.cpp \
    partitionselection.cpp \
    service/modeltestservice.cpp \
    gui/modelsdialog.cpp \
    gui/resultsexportdialog.cpp \
    thread/observer.cpp \
    meta.cpp

HEADERS  += \
    model_defs.h \
    utils.h \
    model.h \
    modeltest.h \
    model_optimizer.h \
    model_optimizer_pll.h \
    msa.h \
    msapll.h \
    plldefs.h \
    tree.h \
    treepll.h \
    model_selection.h \
    global_defs.h \
    partition.h \
    partitioning_scheme.h \
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
    partitionselection.h \
    service/modeltestservice.h \
    gui/modelsdialog.h \
    loggable.h \
    gui/resultsexportdialog.h \
    meta.h

FORMS    += \
    compute_options.ui \
    modeltest_gui.ui \
    gui/xmodeltest.ui \
    gui/progressdialog.ui \
    gui/datainfodialog.ui \
    gui/resultsdialog.ui \
    gui/modelsdialog.ui \
    gui/resultsexportdialog.ui

RESOURCES = mtgraphics.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../usr/local/lib/release/ -lpll
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../usr/local/lib/debug/ -lpll
else:unix: LIBS += -L$$PWD/../../../../../usr/local/lib/ -lpll

CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++11

INCLUDEPATH += $$PWD/../../../../../usr/local/lib
DEPENDPATH += $$PWD/../../../../../usr/local/lib

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../usr/local/lib/release/ -lpll_optimize
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../usr/local/lib/debug/ -lpll_optimize
else:unix: LIBS += -L$$PWD/../../../../../usr/local/lib/ -lpll_optimize

INCLUDEPATH += $$PWD/../../../../../usr/local/include
DEPENDPATH += $$PWD/../../../../../usr/local/include
