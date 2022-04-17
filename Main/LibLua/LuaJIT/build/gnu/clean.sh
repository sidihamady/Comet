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

cd ${SCRIPTDIR}
make clean
cd ${CWD}
