#!/bin/sh
# Copyright (C) 2009-2014 Kalray SA.

BINDIR=
DIRNAME=$(dirname $0)

if [ x$DIRNAME != x. ];
then
BINDIR=$DIRNAME/
fi

case "$*" in
    *-mos=nodeos*) exec ${BINDIR}k1-nodeos-g++ "$@" ;;
    *-mos=rtems*) exec ${BINDIR}k1-rtems-g++ -mprocessing=single -qrtems "$@" ;;
    *-mos=linux*) exec ${BINDIR}k1-linux-g++ "$@" ;;
    *) exec ${BINDIR}k1-elf-g++ "$@" ;;
esac

