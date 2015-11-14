bin_PROGRAMS = modeltest 

modeltest_SOURCES = \
	model.cpp \
	model_optimizer_pll.cpp \
	model_selection.cpp \
	modeltest.cpp \
	modeltest_gui.cpp \
	moc/moc_modeltest_gui.cpp \
	msapll.cpp \
	partition.cpp \
	partitioning_scheme.cpp \
	progressdialog.cpp \
	moc/moc_progressdialog.cpp \
	treepll.cpp \
	utils.cpp \
	main.cpp



modeltest_CPPFLAGS = -g -O3 -std=c++11 -Wall -DPTHREADS
# modeltest_LDFLAGS = -g -O3 -std=c++11 -Wall -DPTHREADS
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
	modeltest.h
