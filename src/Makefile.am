bin_PROGRAMS = modeltest-cmd

modeltest_headers = \
	static_analyzer.h \
	genesis/date_time.h \
	genesis/std.h \
	genesis/string.h \
	genesis/logging.h \
	service/modeltestservice.h \
	service/checkpoint.h \
	thread/observer.h \
	thread/threadpool.h \
	model/abstract_parameter.h \
	model/parameter_branches.h \
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

modeltest_cmd_SOURCES = \
  $(modeltest_headers) \
	genesis/date_time.cpp \
	genesis/string.cpp \
	genesis/logging.cpp \
	static_analyzer.cpp \
	service/modeltestservice.cpp \
	service/checkpoint.cpp \
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

modeltest_cmd_CPPFLAGS = -D_NO_GUI_ -g -O3 -std=c++11 -Wall -DPTHREADS
modeltest_cmd_CPPFLAGS += -DHAVE_PTHREADS
modeltest_cmd_LDFLAGS =

if BUILD_STATIC
  modeltest_cmd_LDADD = -l:libpll_algorithm.a -l:libpll_binary.a -l:libpll_optimize.a -l:libpll_msa.a -l:libpll_tree.a -l:libpll_util.a -l:libpll.a
else
  modeltest_cmd_LDADD = -lpll -lpll_binary -lpll_algorithm -lpll_optimize -lpll_msa -lpll_tree -lpll_util
endif

if PLL_PREFIX
  modeltest_cmd_CPPFLAGS += -DPLL_PREFIX
endif
