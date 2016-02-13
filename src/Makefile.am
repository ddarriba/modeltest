bin_PROGRAMS = modeltestx modeltest

modeltest_SOURCES = \
	moc/moc_progressdialog.cpp \
	moc/moc_resultsdialog.cpp \
	moc/moc_resultsexportdialog.cpp \
	moc/moc_modelsdialog.cpp \
	moc/moc_datainfodialog.cpp \
	moc/moc_xmodeltest.cpp \
	moc/moc_xthreadopt.cpp \
	moc/moc_mydebugstream.cpp \
	gui/qrc_mtgraphics.cpp \
	gui/xmodeltest.cpp \
	gui/xmodeltest_actions.cpp \
	gui/xmodeltest_fill.cpp \
	gui/xthreadopt.cpp \
	gui/progressdialog.cpp \
	gui/datainfodialog.cpp \
	gui/resultsdialog.cpp \
	gui/resultsexportdialog.cpp \
	gui/modelsdialog.cpp \
	gui/xutils.cpp \
	thread/observer.cpp \
	service/modeltestservice.cpp \
	model.cpp \
	model_optimizer_pll.cpp \
	model_selection.cpp \
	modeltest.cpp \
	msapll.cpp \
	partition.cpp \
	partitioning_scheme.cpp \
	treepll.cpp \
	utils.cpp \
	main.cpp

modeltest_CPPFLAGS = -g -O3 -std=c++11 -Wall -DPTHREADS
modeltest_CPPFLAGS += -DHAVE_PTHREADS
modeltest_CPPFLAGS += $(QT4_CFLAGS)
modeltest_LDFLAGS =
modeltest_LDADD = $(QT4_LIBS) -lpll -lpll_optimize

if HAVE_AVX
  modeltest_CPPFLAGS += -DHAVE_AVX
  modeltest_LDFLAGS += -DHAVE_AVX
endif

if HAVE_SSE
  modeltest_CPPFLAGS += -DHAVE_SSE
  modeltest_LDFLAGS += -DHAVE_SSE
endif

modeltestx_SOURCES = \
	service/modeltestservice.cpp \
	thread/observer.cpp \
	model.cpp \
	model_optimizer_pll.cpp \
	model_selection.cpp \
	modeltest.cpp \
	msapll.cpp \
	partition.cpp \
	partitioning_scheme.cpp \
	treepll.cpp \
	utils.cpp \
	main.cpp

modeltestx_CPPFLAGS = -D_NO_GUI_ -g -O3 -std=c++11 -Wall -DPTHREADS
modeltestx_CPPFLAGS += -DHAVE_PTHREADS
modeltestx_LDFLAGS =
modeltestx_LDADD = -lpll -lpll_optimize

if HAVE_AVX
  modeltestx_CPPFLAGS += -DHAVE_AVX
  modeltestx_LDFLAGS += -DHAVE_AVX
endif

if HAVE_SSE
  modeltestx_CPPFLAGS += -DHAVE_SSE
  modeltestx_LDFLAGS += -DHAVE_SSE
endif

pkgincludedir=${includedir}/modeltest
pkginclude_HEADERS= \
	gui/progressdialog.h \
	gui/datainfodialog.h \
	gui/mydebugstream.h \
	gui/qdebugstream.h \
	gui/resultsexportdialog.h \
	gui/xthreadopt.h \
	gui/modelsdialog.h  \
	gui/resultsdialog.h \
	gui/xmodeltest.h \
	gui/xutils.h \
	service/modeltestservice.h \
	thread/observer.h \
	thread/threadpool.h \
	model_optimizer_pll.h \
	loggable.h \
	utils.h \
	tree.h \
	modeltest_gui.h \
	plldefs.h \
	model_selection.h \
	model.h \
	model_defs.h \
	partition.h \
	model_optimizer.h \
	treepll.h \
	partitioning_scheme.h \
	msa.h \
	msapll.h \
	global_defs.h \
	modeltest.h \
	ui_compute_options.h \
	ui_progressdialog.h \
	ui_modelsdialog.h \
	ui_resultsexportdialog.h \
	ui_datainfodialog.h \
	ui_resultsdialog.h \
	ui_xmodeltest.h


imgdir=$(datadir)/modeltest/src/IMG
dist_img_DATA=IMG/console-icon.png  IMG/gear.png  IMG/mt-icon.png  IMG/results-icon.png  IMG/run-icon.png  IMG/tool-icon.png IMG/arrows.png
