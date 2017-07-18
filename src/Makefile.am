bin_PROGRAMS = modeltest-cmd

modeltest_cmd_SOURCES = \
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

if BUILD_STATIC
  modeltest_cmd_LDFLAGS = -static -static-libstdc++
endif

modeltest_cmd_LDADD = -lpll_binary -lpll_algorithm -lpll_optimize -lpll_msa -lpll_tree -lpll_util -lpll

if PLL_PREFIX
  modeltest_cmd_CPPFLAGS += -DPLL_PREFIX
endif
