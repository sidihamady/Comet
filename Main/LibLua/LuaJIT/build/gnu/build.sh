#!/bin/bash

CWD=$(pwd)

SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do
    SCRIPTDIR="$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )"
    SOURCE="$(readlink "$SOURCE")"
    [[ $SOURCE != /* ]] && SOURCE="$SCRIPTDIR/$SOURCE"
done
SCRIPTDIR="$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )"
if [ "$SCRIPTDIR" = "" ] ; then
    SCRIPTDIR="."
fi
SCRIPTDIR=${SCRIPTDIR%/}

cd ${SCRIPTDIR}/../..
make clean
LD_RUN_PATH='$ORIGIN' make all

DEVC_BINDIR=${SCRIPTDIR}/../../../../../output/Linux64/bin/
BITS=$(getconf LONG_BIT)
if [ $BITS = 32 ]; then
    DEVC_BINDIR=${SCRIPTDIR}/../../../../../output/Linux32/bin/
fi
cp -f src/libluacore.so $DEVC_BINDIR

cd ${CWD}
