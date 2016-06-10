bin_PROGRAMS = modeltestx 

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
	meta.cpp \
	main.cpp

modeltestx_CPPFLAGS = -D_NO_GUI_ -g -O3 -std=c++11 -Wall -DPTHREADS
modeltestx_CPPFLAGS += -DHAVE_PTHREADS
modeltestx_LDFLAGS =
modeltestx_LDADD = -lpll -lpll_optimize -lpll_msa -lpll_tree

#if HAVE_AVX
#  modeltestx_CPPFLAGS += -DHAVE_AVX
#  modeltestx_LDFLAGS += -DHAVE_AVX
#endif

#if HAVE_SSE
#  modeltestx_CPPFLAGS += -DHAVE_SSE
#  modeltestx_LDFLAGS += -DHAVE_SSE
#endif

pkgincludedir=${includedir}/modeltest
pkginclude_HEADERS= \
	service/modeltestservice.h \
	thread/observer.h \
	thread/threadpool.h \
	model_optimizer_pll.h \
	loggable.h \
	meta.h \
	utils.h \
	tree.h \
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
	modeltest.h 
