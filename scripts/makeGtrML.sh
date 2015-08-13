#!/bin/sh

RAXMLBINARY=raxmlHPC-SSE3
INPUTALIGN=$1
OUTPUTFILE=$2
RANDOMSEED=$3

EXECNAME=mtTemp
OUTRAXML=RAxML_bestTree.${EXECNAME}

if [ -z "${RANDOMSEED}" ]; then
  RANDOMSEED=12345
fi

rm RAxML*.${EXECNAME} 2> /dev/null
${RAXMLBINARY} -s ${INPUTALIGN} -p $RANDOMSEED -m GTRGAMMA -n ${EXECNAME} 2>&1 > /dev/null

if [ -f ${OUTRAXML} ]; then
    if [ -z "${OUTPUTFILE}" ]; then
        cat ${OUTRAXML}
    else
        mv ${OUTRAXML} ${OUTPUTFILE}
    fi
    rm RAxML*.${EXECNAME}
fi
