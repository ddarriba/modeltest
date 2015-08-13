#-------------------------------------------------
#
# Project created by QtCreator 2015-05-31T18:30:19
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = jModelTest2
TEMPLATE = app

SOURCES += main.cpp\
    utils.cpp \
    model.cpp \
    modeltest.cpp \
    jmodeltest.cpp \
    main-console.cpp \
    progressdialog.cpp \
    model_optimizer_pll.cpp \
    msapll.cpp \
    treepll.cpp \
    model_selection.cpp

HEADERS  += \
    model_defs.h \
    utils.h \
    model.h \
    modeltest.h \
    jmodeltest.h \
    progressdialog.h \
    model_optimizer.h \
    model_optimizer_pll.h \
    msa.h \
    msapll.h \
    plldefs.h \
    tree.h \
    treepll.h \
    model_selection.h \
    global_defs.h

FORMS    += jmodeltest.ui \
    compute_options.ui \
    progressdialog.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../usr/local/lib/release/ -lpll
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../usr/local/lib/debug/ -lpll
else:unix: LIBS += -L$$PWD/../../../../../usr/local/lib/ -lpll

CONFIG += c++11
CXXFLAGS += -std=c++11

INCLUDEPATH += $$PWD/../../../../../usr/local/lib
DEPENDPATH += $$PWD/../../../../../usr/local/lib

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../usr/local/lib/release/ -lpll_optimize
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../usr/local/lib/debug/ -lpll_optimize
else:unix: LIBS += -L$$PWD/../../../../../usr/local/lib/ -lpll_optimize

INCLUDEPATH += $$PWD/../../../../../usr/local/include
DEPENDPATH += $$PWD/../../../../../usr/local/include
