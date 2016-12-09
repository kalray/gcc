#!/bin/sh
# Copyright (C) 2009-2014 Kalray SA.

BINDIR=
DIRNAME=$(dirname $0)

if [ x$DIRNAME != x. ];
then
BINDIR=$DIRNAME/
fi

case "$*" in
    *-mos=nodeos*) exec ${BINDIR}k1-nodeos-gcc "$@" ;;
    *-mos=rtems*) exec ${BINDIR}k1-rtems-gcc -mprocessing=single -qrtems "$@" ;;
    *-mos=linux*) exec ${BINDIR}k1-linux-gcc "$@" ;;
    *) exec ${BINDIR}k1-elf-gcc "$@" ;;
esac

