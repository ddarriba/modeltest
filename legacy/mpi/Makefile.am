CXX = @MPICXX@

if BUILD_MPI

bin_PROGRAMS = modeltest-mpi

modeltestmpi_headers = \
	../src/static_analyzer.h \
	../src/genesis/date_time.h \
	../src/genesis/std.h \
	../src/genesis/string.h \
	../src/genesis/logging.h \
	../src/service/modeltestservice.h \
	../src/service/checkpoint.h \
	../src/thread/observer.h \
	../src/thread/threadpool.h \
	../src/thread/parallel_context.h \
	../src/model/abstract_parameter.h \
	../src/model/parameter_branches.h \
	../src/model/parameter_ratecats.h \
	../src/model/parameter_gamma.h \
	../src/model/parameter_pinv.h \
	../src/model/parameter_substrates.h \
	../src/model/parameter_frequencies.h \
	../src/model/model.h \
	../src/model/dnamodel.h \
	../src/model/protmodel.h \
	../src/optimize/model_optimizer_pll.h \
	../src/optimize/model_optimizer.h \
	../src/optimize/partition_optimizer.h \
	../src/loggable.h \
	../src/meta.h \
	../src/utils.h \
	../src/tree.h \
	../src/plldefs.h \
	../src/model_selection.h \
	../src/model_defs.h \
	../src/partition.h \
	../src/treepll.h \
	../src/partitioning_scheme.h \
	../src/msa.h \
	../src/msapll.h \
	../src/global_defs.h \
	../src/modeltest.h

modeltest_mpi_SOURCES = \
  $(modeltestmpi_headers) \
	../src/genesis/date_time.cpp \
	../src/genesis/string.cpp \
	../src/genesis/logging.cpp \
	../src/static_analyzer.cpp \
	../src/service/modeltestservice.cpp \
	../src/service/checkpoint.cpp \
	../src/thread/observer.cpp \
	../src/thread/parallel_context.cpp \
	../src/model/abstract_parameter.cpp \
	../src/model/parameter_branches.cpp \
	../src/model/parameter_ratecats.cpp \
	../src/model/parameter_gamma.cpp \
	../src/model/parameter_pinv.cpp \
	../src/model/parameter_substrates.cpp \
	../src/model/parameter_frequencies.cpp \
	../src/model/model.cpp \
	../src/model/dnamodel.cpp \
	../src/model/protmodel.cpp \
	../src/optimize/model_optimizer_pll.cpp \
	../src/optimize/partition_optimizer.cpp \
	../src/model_selection.cpp \
	../src/modeltest.cpp \
	../src/msapll.cpp \
	../src/partition.cpp \
	../src/partitioning_scheme.cpp \
	../src/treepll.cpp \
	../src/utils.cpp \
	../src/meta.cpp \
	../src/main.cpp

modeltest_mpi_CPPFLAGS = -I.. -I../src -D_NO_GUI_ -g -O3 -std=c++11 -Wall -DPTHREADS
modeltest_mpi_CPPFLAGS += -DHAVE_PTHREADS -DBUILD_MPI

modeltest_mpi_LDFLAGS = 
if BUILD_STATIC
  modeltest_mpi_LDADD = -lpll_binary -lpll_algorithm -lpll_optimize -lpll_msa -lpll_tree -lpll_util -lpll -lpthread
else
  modeltest_mpi_LDADD = -lpll_binary -lpll_algorithm -lpll_optimize -lpll_msa -lpll_tree -lpll_util -lpll -lpthread
endif

endif
