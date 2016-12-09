#!/bin/sh
# Copyright (C) 2009-2014 Kalray SA.

BINDIR=
DIRNAME=$(dirname $0)

if [ x$DIRNAME != x. ];
then
BINDIR=$DIRNAME/
fi

case "$*" in
    *-mos=nodeos*) exec ${BINDIR}k1-nodeos-gfortran "$@" ;;
    *-mos=rtems*) exec ${BINDIR}k1-rtems-gfortran -mprocessing=single -qrtems "$@" ;;
    *-mos=linux*) exec ${BINDIR}k1-linux-gfortran "$@" ;;
    *) exec ${BINDIR}k1-elf-gfortran "$@" ;;
esac

