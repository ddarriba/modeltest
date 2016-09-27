bin_PROGRAMS = modeltestx 

modeltestx_SOURCES = \
	genesis/date_time.cpp \
	genesis/string.cpp \
	genesis/logging.cpp \
	static_analyzer.cpp \
	service/modeltestservice.cpp \
	thread/observer.cpp \
	model/abstract_parameter.cpp \
	model/parameter_branches.cpp \
	model/parameter_ratecats.cpp \
	model/parameter_gamma.cpp \
	model/parameter_pinv.cpp \
	model/parameter_substrates.cpp \
	model/parameter_frequencies.cpp \
	model/model.cpp \
	optimize/model_optimizer_pll.cpp \
	optimize/partition_optimizer.cpp \
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
modeltestx_LDADD = -lpll -lpll_binary -lpll_algorithm -lpll_optimize -lpll_msa -lpll_tree -lpll_util

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
	static_analyzer.h \
	genesis/date_time.h \
	genesis/string.h \
	genesis/logging.h \
	service/modeltestservice.h \
	thread/observer.h \
	thread/threadpool.h \
	model/abstract_parameter.h \
	model/parameter_ratecats.h \
	model/parameter_gamma.h \
	model/parameter_pinv.h \
	model/parameter_substrates.h \
	model/parameter_frequencies.h \
	model/model.h \
	optimize/model_optimizer_pll.h \
	optimize/model_optimizer.h \
	optimize/partition_optimizer.h \
	loggable.h \
	meta.h \
	utils.h \
	tree.h \
	plldefs.h \
	model_selection.h \
	model_defs.h \
	partition.h \
	treepll.h \
	partitioning_scheme.h \
	msa.h \
	msapll.h \
	global_defs.h \
	modeltest.h
