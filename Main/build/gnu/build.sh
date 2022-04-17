#!/bin/bash
# Comet Build System
# (C) 2010-2022 Pr. Sidi HAMADY
# http://www.hamady.org
# sidi@hamady.org

BITS=$(getconf LONG_BIT)

BITSP=bit
ARCHNAME=linux_$BITS$BITSP
DISTNAME=cometn_$ARCHNAME

whatarg=${1:-all}

response=y
read -r -p "Start the build process? [y/n] " response
if [ ! $response = y ]
then
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

if [ $whatarg = all ]
then

    printf "\n>>>>>>>>>>>> CLEANING... <<<<<<<<<<<<\n"

    ./clean.sh

    rm -f $OUTDIR/bin/libwx*.so
    rm -f $OUTDIR/bin/libwxjpeg.a
    rm -f $OUTDIR/bin/libwxregex.a
    rm -f $OUTDIR/bin/libwxtiff.a
    rm -f $OUTDIR/bin/libfile.so
    rm -f $OUTDIR/bin/libluacore.so
    rm -f $OUTDIR/bin/libluaext.so
    if [ $BITS = 64 ]; then
        # Lua modules for 64bit version only
        rm -f $OUTDIR/bin/lpeg.so
        rm -f $OUTDIR/bin/lmapm.so
    fi
    rm -f $OUTDIR/bin/comet

    rm -f $DISTDIR/Comet/bin/libwx*.so
    rm -f $DISTDIR/Comet/bin/libfile.so
    rm -f $DISTDIR/Comet/bin/libluacore.so
    rm -f $DISTDIR/Comet/bin/libluaext.so
    if [ $BITS = 64 ]; then
        # Lua modules for 64bit version only
        rm -f $DISTDIR/Comet/bin/lpeg.so
        rm -f $DISTDIR/Comet/bin/lmapm.so
    fi
    rm -f $DISTDIR/Comet/bin/comet

    printf "\n>>>>>>>>>> CLEANING DONE. <<<<<<<<<<<\n"

fi

if [ $whatarg = all -o $whatarg = file -o $whatarg = main ]
then
    cd $MAINDIR/LibFile/build/gnu
    ./build.sh
fi

if [ $whatarg = all -o $whatarg = wx ]
then

    find $MAINDIR/wxWidgets/build -type f -name "bk-deps" -exec chmod +x {} \;
    find $MAINDIR/wxWidgets/build -type f -name "bk-make-pch" -exec chmod +x {} \;
    find $MAINDIR/wxWidgets/build -type f -name "build.sh" -exec chmod +x {} \;

    if [ $BITS = 32 ]; then
        cd $MAINDIR/wxWidgets/build/linux32
        ./build.sh
        cd $MAINDIR/wxWidgets/build/linux32/contrib/src/stc
        ./build.sh
    else
        cd $MAINDIR/wxWidgets/build/linux64
        ./build.sh
        cd $MAINDIR/wxWidgets/build/linux64/contrib/src/stc
        ./build.sh
    fi
fi

if [ $whatarg = all -o $whatarg = lua -o $whatarg = luacore -o $whatarg = main ]
then
    cd $MAINDIR/LibLua/LuaJIT/build/gnu
    ./build.sh
fi

if [ $whatarg = all -o $whatarg = lua -o $whatarg = luaext -o $whatarg = main ]
then
    cd $MAINDIR/LibLua/LuaExt/build/gnu
    ./build.sh
fi

if [ $whatarg = all -o $whatarg = lua -o $whatarg = luamod -o $whatarg = main ]
then
    cd $MAINDIR/LibLua/LuaMod/build/gnu
    ./build.sh
fi

if [ $whatarg = all -o $whatarg = comet -o $whatarg = main ]
then
    cd $MAINDIR/Comet/build/gnu
    ./build.sh
fi

if [ $whatarg = all -o $whatarg = release ]
then

    printf "\n>>>>>> DISTRIBUTION BUILDING... <<<<<\n"

    cp -f $OUTDIR/bin/libwx*.so $DISTDIR/Comet/bin/
    cp -f $OUTDIR/bin/libfile.so $DISTDIR/Comet/bin/
    cp -f $OUTDIR/bin/libluacore.so $DISTDIR/Comet/bin/
    cp -f $OUTDIR/bin/libluaext.so $DISTDIR/Comet/bin/
    if [ $BITS = 64 ]; then
        # Lua modules for 64bit version only
        cp -f $OUTDIR/bin/lpeg.so $DISTDIR/Comet/bin/
        cp -f $OUTDIR/bin/lmapm.so $DISTDIR/Comet/bin/
    fi
    cp -f $OUTDIR/bin/comet $DISTDIR/Comet/bin/

    strip -s $DISTDIR/Comet/bin/libwxadv.so
    strip -s $DISTDIR/Comet/bin/libwxaui.so
    strip -s $DISTDIR/Comet/bin/libwxbase.so
    strip -s $DISTDIR/Comet/bin/libwxbasenet.so
    strip -s $DISTDIR/Comet/bin/libwxbasexml.so
    strip -s $DISTDIR/Comet/bin/libwxcore.so
    strip -s $DISTDIR/Comet/bin/libwxhtml.so
    strip -s $DISTDIR/Comet/bin/libwxqa.so
    strip -s $DISTDIR/Comet/bin/libwxstc.so
    strip -s $DISTDIR/Comet/bin/libfile.so
    strip -s $DISTDIR/Comet/bin/libluacore.so
    strip -s $DISTDIR/Comet/bin/libluaext.so
    if [ $BITS = 64 ]; then
        # Lua modules for 64bit version only
        strip -s $DISTDIR/Comet/bin/lpeg.so
        strip -s $DISTDIR/Comet/bin/lmapm.so
    fi
    strip -s $DISTDIR/Comet/bin/comet

    rm -f $DISTDIR/Comet/doc/*
    cp -f $DEVCDIR/Manual/doc/comet.pdf $DISTDIR/Comet/doc/
    rm -f $DISTDIR/Comet/icons/*
    cp -f $DEVCDIR/Manual/icons/* $DISTDIR/Comet/icons/

    cp -f $DISTDIR/Comet/doc/comet.pdf $OUTDIR/doc/

    find $DEVCDIR -type f -path 'bin/*' -name "comet" -exec chmod +x {} \;

    cd $DISTDIR/
    tar czf --exclude "Comet/support" --exclude-from=$DISTDIR/ignore.txt $DISTNAME.tgz Comet

    printf "\n>>>> DISTRIBUTION BUILDING DONE.  <<<\n"

    printf "\n>>>>>>>>>>>> CLEANING... <<<<<<<<<<<<\n"

    rm -f $OUTDIR/lib/libwxjpeg.a
    rm -f $OUTDIR/lib/libwxregex.a
    rm -f $OUTDIR/lib/libwxtiff.a

    cd $CWD
    ./clean.sh

    printf "\n>>>>>>>>>> CLEANING DONE. <<<<<<<<<<<\n"
fi

cd $CWD
