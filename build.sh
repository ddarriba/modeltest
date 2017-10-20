#!/bin/sh

# script for building modeltest distribution locally.
#
# Syntax:
#   $ ./install.sh [actions]
#
#   actions:
#     `build` - Build modeltest-ng and modeltest-gui
#     `clean` - Clean all object and target files
#     `dist`  - Pack libraries for distribution
#
# Notes:
#   - Actions are run in the given order
#   - If no action is given, `build` is set automatically
#   - Everything will be built in `$prefix` (build/ by default)
#   - PLL and modules are statically linked

# Static configuration
build_pll=yes            # build PLL
build_modules=yes        # build PLL modules library
build_gui=yes            # build modeltest-gui
dir_base=${PWD}          # base directory
prefix=${dir_base}/build # output directory for modeltest
qmake_bin=qmake          # qmake binary (for GUI)

unameOut="$(uname -s)"
case "${unameOut}" in
  Linux*)     machine=Linux;;
  Darwin*)    machine=Mac;;
  CYGWIN*)    machine=Cygwin;;
  MINGW*)     machine=MinGw;;
  *)          machine="UNKNOWN:${unameOut}"
esac

echo "Running install script for ${machine}"

mkdir -p ${prefix}/bin ${prefix}/include/libpll ${prefix}/lib

test -d ${prefix}/bin || { echo "Output directory could not be created"; exit; }

# check if `qmake` is available
if test "x`which ${qmake_bin}`" = "x" ; then
  build_gui=no
else
  ${qmake_bin} -v 2> /dev/null || build_gui=no
fi

# output directory for pll and modules
dir_build=${prefix}

# variables for building pll and modules
dir_modules=${PWD}/libs/pll-modules
dir_pll=${dir_modules}/libs/libpll

dir_pll_include=${dir_build}/include
dir_pll_lib=${dir_build}/lib

makefile_qt=Makefile.qmake

actions=$*
test -z "${actions}" && actions="build"

for action in ${actions}; do
  case "$action" in
  "build")

    test -f ${dir_pll_lib}/libpll.a && build_pll=no
    test -f ${dir_pll_lib}/libpll_algorithm.a && build_modules=no

    echo "...configuration:"
    echo "   prefix:       ${prefix}"
    echo "   build:        ${dir_build}"
    echo "   pll:          [${build_pll}] ${dir_pll}"
    echo "   modules:      [${build_modules}] ${dir_modules}"
    echo "      include:   ${dir_pll_include}"
    echo "      lib:       ${dir_pll_lib}"
    echo ""

    # variables for building modeltest

    file_log=${PWD}/build.log

    test -d ${dir_build} || { echo "creating ${dir_build}"; mkdir -p ${dir_build} || exit 1; }
    test -d ${prefix} || { echo "creating ${prefix}"; mkdir -p ${prefix} || exit 1; }

    echo "...writing log to ${file_log}"
    test -f ${file_log} && rm ${file_log}

    fn_build()
    {
      args=
      pref_dir=$1
      inc_dir=$2
      lib_dir=$3
      test "x${inc_dir}" = "x-" || args="${args} CPPFLAGS=-I${inc_dir}"
      test "x${lib_dir}" = "x-" || args="${args} LDFLAGS=-I${lib_dir}"
      test "x${pref_dir}" = "x-" || args="${args} --prefix=-I${pref_dir}"

      shift; shift; shift;
      extra_args=$*
      test -f ./configure || autoreconf -i || return 1
      ./configure CPPFLAGS=-I${dir_pll_include} LDFLAGS=-L${dir_pll_lib} --prefix ${dir_build} ${extra_args} || \
        return 1
      make \
        && make install || return 1
    }

    fn_qt()
    {
      args=
      in_file=$1
      out_file=$2
      pref_dir=$3
      test "x${pref_dir}" = "x-" || args="${args} --prefix=-I${pref_dir}"

      shift; shift; shift;
      extra_args=$*

      ${qmake_bin} CONFIG+="pll_local ${extra_args}" -o ${out_file} ${in_file}
      test -f ${out_file} || return 1
      make -f ${out_file} \
        && make install -f ${out_file} || return 1
    }

    returnval=0

    if test x${build_pll} = xyes; then
      echo "...build pll"
      libpll_tarball=libpll-*.tar.gz
      libpll_files=`ls ${dir_pll}/* 2> /dev/null`
      if test -f ${libpll_tarball} && test -z "${libpll_files}"; then
        echo "   ...extracting pll tarball"
        libpll_dir=`echo ${libpll_tarball} | rev | cut -d'.' -f3- | rev`
        tar zxf ${libpll_tarball}
        test -d ${libpll_dir} || { echo "PLL extraction fail"; exit; }
        mkdir -p ${dir_pll}
        mv ${libpll_dir}/* ${dir_pll}/
        rm -rf ${libpll_dir}
      fi
      test -d ${dir_pll} || { echo "PLL directory missing"; exit; }
      cd $dir_pll
      echo "   ...building pll"
      fn_build ${dir_build} ${dir_pll_include} ${dir_pll_lib} >> ${file_log} 2>&1 && echo "...pll OK!" || { echo "...pll FAIL!"; exit 1; }
      cd -
    else
      echo "...skip pll"
    fi

    if test "x${build_modules}" = "xyes" ; then
      echo "...build modules"
      modules_tarball=pll-modules-*.tar.gz
      modules_files=`ls ${dir_modules}/config* 2> /dev/null`
      if test -f ${modules_tarball} && test -z "${modules_files}"; then
        echo "   ...extracting modules tarball"
        modules_dir=`echo ${modules_tarball} | rev | cut -d'.' -f3- | rev`
        tar zxf ${modules_tarball}
        test -d ${modules_dir} || { echo "Modules extraction fail"; exit; }
        mkdir -p ${dir_modules}
        mv ${modules_dir}/* ${dir_modules}/
        rm -rf ${modules_dir}
      fi
      test -d ${dir_modules} || { echo "PLL modules directory missing"; exit; }
      cd $dir_modules
      echo "   ...building modules"
      fn_build ${dir_build} ${dir_pll_include} ${dir_pll_lib} >> ${file_log} 2>&1 && echo "...modules OK!" || { echo "...modules FAIL!"; exit 1; }
      cd -
    else
      echo "...skip modules"
    fi

    echo "...build modeltest"
    mt_extra_conf=
    mt_qt_extra_conf=
    if test "x${machine}" = "xLinux" ; then
      mt_extra_conf="--enable-static"
      mt_qt_extra_conf="build_static"
    fi
    fn_build ${prefix} ${dir_pll_include} ${dir_pll_lib} ${mt_extra_conf} >> ${file_log} 2>&1 && echo "...modeltest OK!" || { echo "...modeltest FAIL!"; exit 1; }

    if test "x${build_gui}" = "xyes" ; then
      echo "...build modeltest GUI"
      fn_qt src/modeltest.pro ${makefile_qt} ${prefix} ${mt_qt_extra_conf} >> ${file_log} 2>&1 && \
        { echo "...modeltest GUI OK!"
          if test "x${machine}" = "xMac" ; then
            mv modeltest-gui.app ${prefix}/bin                                                                                                                                                                                                                                                                                                                                        
          else
            mv modeltest-gui ${prefix}/bin;
          fi 
        } || \
        { echo "...modeltest GUI FAIL!"; exit 1; }
    else
      echo "...cannot find qmake. Skip modeltest GUI"
    fi
  ;;
  "clean")
    echo "...cleaning object files"
    if test "x${build_pll}" = "xyes" ; then
      cd $dir_pll
      make clean
    fi
    if test "x${build_modules}" = "xyes" ; then
      cd $dir_modules
      make clean
    fi
    cd ${dir_base}
    make clean
    if test "x${build_gui}" = "xyes" ; then
      make clean -f ${makefile_qt}
    fi
    echo "...cleaning output files"
    rm -f ${dir_pll_lib}/libpll*
  ;;
  "dist")
    echo "...build distribution"
    cd $dir_pll
    make dist
    mv *gz ${dir_base}
    cd $dir_modules
    make dist
    mv *gz ${dir_base}
    cd ${dir_base}
    make dist
  ;;
  "test")
   if test -f ${prefix}/bin/modeltest-ng; then
     ${prefix}/bin/modeltest-ng -i example-data/dna/tiny.fas -o tmptest -t ml > /dev/null \
       && echo "test OK!" || echo "test ERROR!"
     rm -f tmptest.*
   else
     echo "...modeltest binary does not exist"
     exit 1
  fi
  ;;
  esac
done
