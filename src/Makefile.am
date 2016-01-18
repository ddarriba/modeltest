bin_PROGRAMS = modeltestx modeltest

modeltest_SOURCES = \
	moc/moc_modeltest_gui.cpp \
	moc/moc_progressdialog.cpp \
	gui/qrc_mtgraphics.cpp \
	gui/xmodeltest.cpp \
	moc_xmodeltest.cpp \
	model.cpp \
	model_optimizer_pll.cpp \
	model_selection.cpp \
	modeltest.cpp \
	modeltest_gui.cpp \
	msapll.cpp \
	partition.cpp \
	partitioning_scheme.cpp \
	progressdialog.cpp \
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
	thread/threadpool.h \
	model_optimizer_pll.h \
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
	progressdialog.h \
	msapll.h \
	global_defs.h \
	modeltest.h \
	ui_compute_options.h \
	ui_modeltest_gui.h \
	ui_progressdialog.h

imgdir=$(datadir)/modeltest/IMG
dist_img_DATA=IMG/console-icon.png  IMG/gear.png  IMG/mt-icon.png  IMG/results-icon.png  IMG/run-icon.png  IMG/tool-icon.png
