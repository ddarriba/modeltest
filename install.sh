#!/bin/sh

SRC_DIR=src

#find tools
QMAKE_BIN=`which qmake`
QMAKE_INPUT=${SRC_DIR}/modeltest.pro
QMAKE_OUTPUT=Makefile.qmake

AUTORECONF_BIN=`which autoreconf`
CONFIG_bin=./configure

if [ ! -z ${QMAKE_BIN} ]; then
  echo Building ${QMAKE_OUTPUT}
  qmake_v=`${QMAKE_BIN} -v | head -n 1 | cut -d' ' -f3`
  qt_v=`${QMAKE_BIN} -v | head -n 2 | cut -d' ' -f4`
  echo qmake binary: ${QMAKE_BIN}
  echo qmake version: ${qmake_v}
  echo qt version: ${qt_v}
  [ ! -f ${QMAKE_INPUT} ] && echo Missing project file ${QMAKE_INPUT} && exit
  ${QMAKE_BIN} -o ${QMAKE_OUTPUT} ${QMAKE_INPUT} && make -f ${QMAKE_OUTPUT}
else
  qmake not found... Skip
fi

if [ ! -f ${CONFIG_BIN} ]; then
  echo Configuration file not found
  if [ ! -z ${AUTORECONF_BIN} ]; then
    echo Building Makefile
    ${AUTORECONF_BIN} -i
  fi
fi

${CONFIG_BIN} && make

echo Done
