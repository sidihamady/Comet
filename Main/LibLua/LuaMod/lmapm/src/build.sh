#!/bin/bash

DEVC_BINDIR=../../../../../output/Linux64/bin/
BITS=$(getconf LONG_BIT)
if [ $BITS = 32 ]
then
    DEVC_BINDIR=../../../../../output/Linux32/bin/
fi

make clean
LD_RUN_PATH='$ORIGIN' make all
cp -f lmapm.so $DEVC_BINDIR
make clean
