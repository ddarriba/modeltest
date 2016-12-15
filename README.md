![ModelTest](https://github.com/ddlsandbox/assets/blob/master/modeltest/img/header.png?raw=true)

ModelTest is a tool for selecting the best-fit model and partitioning scheme. ModelTest integrates jModelTest, ProtTest and PartitionTest in one single tool, with graphical and command console interfaces.

## Documentation

If you want to read about using ModelTest, a PDF manual is attached to each [release](https://github.com/ddarriba/pll-modules/releases). You can also browse the [wiki](https://github.com/ddarriba/pll-modules/wiki) for online documentation.

The API reference for developing packages is also documented on Atom.io.

## Prerequisites

Core model parameter optimization and likelihood evaluation depend on the high-level modules for the Phylogenetic Likelihood Library. The latest compatible version is linked here as a submodule, so if you are cloning the repository, make sure you initialize the submodules:

```bash
$ git clone --recursive https://github.com/ddarriba/modeltest
```
This step is not necessary if you downloaded the released tarball.

## Install

1. Graphical User Interface

  To install ModelTest GUI type the following commands:
  
  ```bash
  $ ./build_qmake-sh
  $ make -f Makefile.qmake
  $ make install -f Makefile.qmake
  ```

2. Command Console Interface

  To install ModelTest type the following commands:
  
  ```bash
  $ ./configure [--prefix=INSTALL_DIR]
  $ make
  $ make install
  ```

Set a custom install directory if you don't have root access or you prefer a
different location.

This will compile pthreads version
Run 'modeltest' without arguments for loading the GUI
Run 'modeltest --help' for help about the console interface.

To compile a Windows executable, install MinGW and run:

```bash
$ ac_cv_func_malloc_0_nonnull=yes ac_cv_func_realloc_0_nonnull=yes mingw64-configure
$ make
```

In case the configure script does not exist, it must be generated using autotools:

```bash
$ autoreconf -i
```
