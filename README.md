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

## Discussion group

Please use the ModelTest discussion group for any question: http://groups.google.com/group/modeltest-ng

## Download

From the [releases section](https://github.com/ddarriba/modeltest/releases) you can
download the latest stable distribution and pre-compiled binaries.

If there is no binary for your system or it is not working, you can either download the sources
distribution or clone this repository, and follow the Install instructions below:

```bash
$ git clone https://github.com/ddarriba/modeltest
```

## Install

1. **Install the dependencies.** On Debian-based systems, you can simply run

```
sudo apt-get install flex bison
```
For other systems, please make sure you have following packages/libraries installed:
[`GNU Bison`](http://www.gnu.org/software/bison/) [`Flex`](http://flex.sourceforge.net/)

2. **Build ModelTest-NG.**

Multithread version:

```
mkdir build && cd build
cmake ..
make
```

MPI version:

```
mkdir build && cd build
cmake -DENABLE_MPI=ON ..
make
```

GUI version:

```
mkdir build && cd build
cmake -DENABLE_GUI=ON ..
make
```

Portable version (static linkage, compatible with old non-AVX CPUs):

```
mkdir build && cd build
cmake -DSTATIC_BUILD=ON -DENABLE_MODELTEST_SIMD=OFF -DENABLE_PLLMOD_SIMD=OFF ..
make
```

ModelTest-NG binaries will be placed in `modeltest-ng/bin` directory.

## Build a Docker image of ModelTest-NG

If you want to build a [Docker](https://www.docker.com/) image, use a command like this:

```sh
docker build -t modeltest-ng .
```

Then call docker run to create a container using the created image.

```sh
docker run -it modeltest-ng bash
```
## Disclaimer

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. The jModelTest distribution includes Phyml executables.

These programs are protected by their own license and conditions, and using jModelTest implies agreeing with those conditions as well.
