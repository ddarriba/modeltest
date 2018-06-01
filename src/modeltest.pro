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
    genesis/date_time.cpp \
    genesis/string.cpp \
    genesis/logging.cpp \
    model/abstract_parameter.cpp \
    model/parameter_branches.cpp \
    model/parameter_gamma.cpp \
    model/parameter_ratecats.cpp \
    model/parameter_frequencies.cpp \
    model/parameter_pinv.cpp \
    model/parameter_substrates.cpp \
    model/model.cpp \
    model/dnamodel.cpp \
    model/protmodel.cpp \
    optimize/model_optimizer_pll.cpp \
    optimize/partition_optimizer.cpp \
    modeltest.cpp \
    msapll.cpp \
    treepll.cpp \
    model_selection.cpp \
    partition.cpp \
    partitioning_scheme.cpp \
    static_analyzer.cpp \
    gui/xthreadopt.cpp \
    gui/progressdialog.cpp \
    gui/datainfodialog.cpp \
    gui/xutils.cpp \
    gui/resultsdialog.cpp \
    service/modeltestservice.cpp \
    gui/modelsdialog.cpp \
    gui/resultsexportdialog.cpp \
    thread/observer.cpp \
    thread/parallel_context.cpp \
    meta.cpp \
    gui2/xmodeltestfancy.cpp \
    gui2/xmodeltestfancy_actions.cpp

HEADERS  += \
    model_defs.h \
    utils.h \
    genesis/date_time.h \
    genesis/string.h \
    genesis/logging.h \
    model/abstract_parameter.h \
    model/parameter_branches.h \
    model/parameter_gamma.h \
    model/parameter_ratecats.h \
    model/parameter_frequencies.h \
    model/parameter_pinv.h \
    model/parameter_substrates.h \
    model/model.h \
    model/dnamodel.h \
    model/protmodel.h \
    optimize/model_optimizer.h \
    optimize/model_optimizer_pll.h \
    optimize/partition_optimizer.h \
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
    gui/mydebugstream.h \
    gui/xthreadopt.h \
    gui/progressdialog.h \
    thread/observer.h \
    thread/parallel_context.h \
    gui/datainfodialog.h \
    gui/xutils.h \
    gui/resultsdialog.h \
    service/modeltestservice.h \
    gui/modelsdialog.h \
    loggable.h \
    gui/resultsexportdialog.h \
    meta.h \
    gui2/xmodeltestfancy.h

FORMS    += \
    gui/progressdialog.ui \
    gui/datainfodialog.ui \
    gui/resultsdialog.ui \
    gui/modelsdialog.ui \
    gui/resultsexportdialog.ui \
    gui2/xmodeltestfancy.ui \
    gui2/aboutdialog.ui

RESOURCES = mtgraphics.qrc

MOC_DIR = build
UI_DIR = build
OBJECTS_DIR = build
RCC_DIR = build

CONFIG += c++11 -g debug pll_prefix
QMAKE_CXXFLAGS += -std=c++11 -g -DHAVE_CONFIG_H
#QMAKE_LFLAGS = -Xlinker -Bstatic $$QMAKE_LFLAGS

build_static {
  CONFIG += static pll_static
} else {
  CONFIG += pll_dyn
}

pll_local {
  message(link to pll local)
  INCPATH += $$PWD/../build/include
  LIBS += -Lbuild/lib
} else {
  message(link to pll global)
}

pll_dyn {
  message(pll dynamic link)
  unix|win32: LIBS += -lpll_algorithm -lpll_binary -lpll_optimize -lpll_msa -lpll_tree -lpll_util -lpll
}

pll_static {
  message(pll static link)
  unix|win32: LIBS += -static -lpll_algorithm \
                      -lpll_binary -lpll_optimize -lpll_msa \
                      -lpll_tree -lpll_util -lpll -Wl,-Bdynamic
  unix: LIBS += -Wl,--dynamic-linker=/lib64/ld-linux-x86-64.so.2
}
