![ModelTest-NG](https://github.com/ddlsandbox/assets/blob/master/modeltest/img/header.png?raw=true)

ModelTest-NG is a tool for selecting the best-fit model of evolution
for DNA and protein alignments.
ModelTest-NG supersedes jModelTest and ProtTest in one single tool,
with graphical and command console interfaces.

## Documentation

If you want to read about using ModelTest-NG, a PDF manual is attached to
each [release](https://github.com/ddarriba/modeltest/releases).
You can also browse the [wiki](https://github.com/ddarriba/modeltest/wiki) for
online documentation, and the [FAQ section](https://github.com/ddarriba/modeltest/wiki/FAQ)
for common errors.

## Citation

If you use ModelTest-NG, please cite the following:

* Darriba, D., Posada, D., Kozlov, A. M., Stamatakis, A., Morel, B., & Flouri, T. (2020). ModelTest-NG: a new and scalable tool for the selection of DNA and protein evolutionary models. Molecular Biology and Evolution, 37(1), 291-294. doi.org/10.1093/molbev/msz189
* Flouri T., Izquierdo-Carrasco F., Darriba D., Aberer AJ, Nguyen LT, Minh BQ, von Haeseler A., Stamatakis A. (2014) The Phylogenetic Likelihood Library. Systematic Biology, 64(2): 356-362. doi:10.1093/sysbio/syu084

## Prerequisites

Core model parameter optimization and likelihood evaluation depend on the
high-level modules for the Phylogenetic Likelihood Library.
The latest compatible version is linked here as a submodule,
so if you are cloning the repository, make sure you initialize the submodules:

```bash
$ git clone --recursive https://github.com/ddarriba/modeltest
```
This step is not necessary if you downloaded the released tarball.

## Install

0. Automatic Build

  There are 3 ways of building ModelTest-NG:
    * Using cmake
    * Using autotools
    * Using docker

  Choose the one is more comfortable for you. If you experience
  any problem, please try another.

  a) Build ModelTest-NG using `cmake`:

    PTHREADS version:

    ```
    cd modeltest-ng
    mkdir build && cd build
    cmake ..
    make
    ```

    MPI version:

    ```
    cd modeltest-ng
    mkdir build && cd build
    cmake -DUSE_MPI=ON ..
    make
    ```

    GUI version:

    ```
    cd modeltest-ng
    mkdir build && cd build
    cmake -DUSE_GUI=ON ..
    make
    ```

    ModelTest-NG binaries will be placed in `modeltest-ng/bin` directory.

  b) Build ModelTest-NG using `autotools`

    If you have downloaded a complete distribution, check the following files exist:
      - libpll-x.y.z.tar.gz
      - pll-modules-x.y.z.tar.gz
      - build.sh

    Run the installer script:

    ```bash
    $ build.sh
    ```

    This should extract and compile the required libraries and link them statically
    in the ModelTest-NG binaries for console (modeltest-ng, modeltest-mpi) and GUI (modeltest-gui) if
    a valid `qmake` is available.

    Note that, qmake might be available in some linux distributions,
    but NOT a functional QT framework.
    Try running `qmake`, and if you observe and error,
    install `qt5-default` from apt repositories:

    ```bash
    $ sudo apt-get install qt5-default
    ```

    The resulting binaries and libraries will be placed in `build/bin` and `build/lib` directories

  c) Using docker

    Build docker image using a command like this:

    ```sh
    docker build -t modeltest-ng .
    ```

    Then call docker run to create a container using the created image.

    ```sh
    docker run -it modeltest-ng bash
    ```
