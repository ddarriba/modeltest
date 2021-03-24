bin_PROGRAMS = modeltest-ng

modeltest_ng_SOURCES = \
	genesis/date_time.cpp \
	genesis/string.cpp \
	genesis/logging.cpp \
	static_analyzer.cpp \
	service/modeltestservice.cpp \
	service/checkpoint.cpp \
	thread/observer.cpp \
	thread/parallel_context.cpp \
	model/abstract_parameter.cpp \
	model/parameter_branches.cpp \
	model/parameter_ratecats.cpp \
	model/parameter_gamma.cpp \
	model/parameter_pinv.cpp \
	model/parameter_substrates.cpp \
	model/parameter_frequencies.cpp \
	model/model.cpp \
	model/dnamodel.cpp \
	model/protmodel.cpp \
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

modeltest_ng_CPPFLAGS = -D_NO_GUI_ -g -O3 -std=c++11 -Wall -DPTHREADS
modeltest_ng_CPPFLAGS += -DHAVE_PTHREADS

if BUILD_STATIC
  modeltest_ng_LDFLAGS = -static -static-libstdc++ -Wl,--whole-archive -lpthread -Wl,--no-whole-archive
endif

modeltest_ng_LDADD = -lpll_binary -lpll_algorithm -lpll_optimize -lpll_msa -lpll_tree -lpll_util -lpll

