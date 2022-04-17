#!/bin/bash
# Comet Build System
# (C) 2010-2022 Pr. Sidi HAMADY
# http://www.hamady.org
# sidi@hamady.org

colorErr='\033[1;31m'
colorOK='\033[38;5;2m'
colorInfo='\033[34m'
colorStd='\033[0m'

doBuild() {
    TARG=
    if [ "$#" -eq 1 ]; then
        TARG=$1
    fi
    ./build.sh $TARG
    RETSTATUS=$?
    if [ $RETSTATUS -ne 0 ]; then
        echo
        echo -e "${colorErr}build failed${colorStd}"
        echo
        exit 1
    fi
}

CDIR=`cd "$(dirname "$0")" && pwd`

cd $CDIR/../../src
doBuild

cd $CDIR