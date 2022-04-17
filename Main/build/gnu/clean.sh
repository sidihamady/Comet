#!/bin/bash
# Comet Build System
# (C) 2010-2022 Pr. Sidi HAMADY
# http://www.hamady.org
# sidi@hamady.org

BITS=$(getconf LONG_BIT)

BITSP=bit
ARCHNAME=linux_$BITS$BITSP
DISTNAME=comet_$ARCHNAME

whatarg=${1:-all}

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

DEVCDIR=${SCRIPTDIR}/../../..
readonly DEVCDIR

MAINDIR=$DEVCDIR/Main
readonly MAINDIR

OUTDIR=$DEVCDIR/output/Linux64
DISTDIR=$DEVCDIR/Dist/Linux64
if [ $BITS = 32 ]; then
    OUTDIR=$DEVCDIR/output/Linux32
    DISTDIR=$DEVCDIR/Dist/Linux32
fi
readonly OUTDIR
readonly DISTDIR

printf "\nrm -r -f ${OUTDIR}/obj/*\n"
rm -r -f ${OUTDIR}/obj/*

printf "\nrm -r -f ${OUTDIR}/config/*\n"
rm -r -f ${OUTDIR}/config/*

printf "\nrm -r -f ${DISTDIR}/config/*\n"
rm -r -f ${DISTDIR}/config/*

printf "\nrm -r -f ${DISTDIR}/icons/*\n"
rm -r -f ${DISTDIR}/icons/*

printf "\n"

cd $CWD
