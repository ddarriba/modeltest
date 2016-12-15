# modeltest

![ModelTest](https://github.com/ddlsandbox/assets/blob/master/modeltest/img/header.png?raw=true)

ModelTest is a tool for selecting the best-fit model and partitioning scheme. ModelTest integrates jModelTest, ProtTest and PartitionTest in one single tool, with graphical and command console interfaces.

## prerequisites

Phylogenetic Likelihood Library High Level Modules repository is available at http://github.com/ddarriba/pll-modules

## install

1. Graphical User Interface

To install ModelTest-Light GUI type the following commands:
$ qmake -o Makefile.qmake src/modeltest.pro
$ make -f Makefile.qmake
$ make install -f Makefile.qmake

2. Command Console Interface

To install ModelTest-Light type the following commands:
$ ./configure [--prefix=INSTALL_DIR]
$ make
$ make install

Set a custom install directory if you don't have root access or you prefer a
different location.

This will compile pthreads version
Run 'modeltest' without arguments for loading the GUI
Run 'modeltest --help' for help about the console interface.

To compile a Windows executable, install MinGW and run:
$ ac_cv_func_malloc_0_nonnull=yes ac_cv_func_realloc_0_nonnull=yes mingw64-configure
$ make

In case the configure script does not exist, it must be generated using autotools:
$ autoreconf -i
