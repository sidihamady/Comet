#!/bin/bash
# Comet Build System
# (C) 2010-2022 Pr. Sidi HAMADY
# http://www.hamady.org
# sidi@hamady.org

BITS=$(getconf LONG_BIT)

# 32bit version for test purpose only
if [ ! $BITS = 64 ]
then
    printf "\nscript for 64bit version only\n\n"
    exit 1
fi

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


MAINDIR=${SCRIPTDIR}/../../../../../Main
readonly MAINDIR

if [ -d $MAINDIR/LibLua/LuaMod/lpeg/src ]; then
    cd $MAINDIR/LibLua/LuaMod/lpeg/src
    ./build.sh
fi
if [ -d $MAINDIR/LibLua/LuaMod/lmapm/src ]; then
    cd $MAINDIR/LibLua/LuaMod/lmapm/src
    ./build.sh
fi

cd $CWD
