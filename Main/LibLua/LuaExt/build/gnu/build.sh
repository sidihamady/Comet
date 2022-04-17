#!/bin/bash
# Comet Build System
# (C) 2010-2022 Pr. Sidi HAMADY
# http://www.hamady.org
# sidi@hamady.org

BITS=$(getconf LONG_BIT)

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

if [ -d $MAINDIR/LibLua/LuaExt/src ]; then
    cd $MAINDIR/LibLua/LuaExt/src
    ./build.sh
fi

cd $CWD
