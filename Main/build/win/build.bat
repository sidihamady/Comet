@echo off

rem Comet Build System
rem (C) 2010-2022 Pr. Sidi HAMADY
rem http://www.hamady.org
rem sidi@hamady.org

rem examples of use:    build.bat 64 all
rem                     build.bat 64 release

pushd "%~dp0"
attrib +R %~n0%~x0

SETLOCAL enabledelayedexpansion

set stopFile=%~dp0.\_stop.txt

@del /f /q %stopFile% >nul 2>&1

for /F "usebackq tokens=1,2 delims==" %%i in (`wmic os get LocalDateTime /VALUE 2^>NUL`) do if '.%%i.'=='.LocalDateTime.' set timeStart=%%j
set timeStart=%timeStart:~0,4%-%timeStart:~4,2%-%timeStart:~6,2% %timeStart:~8,2%:%timeStart:~10,2%
echo Build started at %timeStart%

set BITS=64
set VCARCH=x64

set command1=%1
if "%command1%" == "" set command1=64

set command2=%2
if "%command2%" == "" set command2=all
if "%command1%" == "release" set command2=release
if "%command1%" == "release" set command1=64

if "%command1%" == "32" (
    set BITS=32
    set VCARCH=x86
)

set "buildOK="
if "%command2%" == "all" (set buildOK=1)
if "%command2%" == "main" (set buildOK=1)
if "%command2%" == "file" (set buildOK=1)
if "%command2%" == "wx" (set buildOK=1)
if "%command2%" == "lua" (set buildOK=1)
if "%command2%" == "luaext" (set buildOK=1)
if "%command2%" == "luamod" (set buildOK=1)
if "%command2%" == "comet" (set buildOK=1)
if "%command2%" == "release" (set buildOK=1)
if defined buildOK (
    echo.
) else (
    ENDLOCAL
    echo.
    echo Unknown command argument
    echo Example of valid command: build.bat 64 release
    attrib -R %~n0%~x0
    popd
    exit /b 1
)

set SCRIPTDIR=%~dp0
set DEVCDIR=%SCRIPTDIR%\..\..\..
set OUTDIR=%DEVCDIR%\output\Windows64
set OBJDIR=%DEVCDIR%\output\Windows64\obj
set BINDIR=%DEVCDIR%\output\Windows64\bin
set DISTDIR=%DEVCDIR%\Dist\Windows64\Comet
if "%BITS%" == "32" (
    set OUTDIR=%DEVCDIR%\output\Windows32
    set OBJDIR=%DEVCDIR%\output\Windows32\obj
    set BINDIR=%DEVCDIR%\output\Windows32\bin
    set DISTDIR=%DEVCDIR%\Dist\Windows32\Comet
)

call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" %VCARCH%

if "%command2%" == "all" (
    call clean.bat %BITS%

    del /f /q %BINDIR%\wx*.dll >nul 2>&1
    del /f /q %BINDIR%\file.dll >nul 2>&1
    del /f /q %BINDIR%\luacore.dll >nul 2>&1
    del /f /q %BINDIR%\luaext.dll >nul 2>&1
    del /f /q %BINDIR%\comet.exe >nul 2>&1
    del /f /q %BINDIR%\lpeg.dll >nul 2>&1
    del /f /q %BINDIR%\lmapm.dll >nul 2>&1

    del /f /q %DISTDIR%\bin\wx*.dll >nul 2>&1
    del /f /q %DISTDIR%\bin\file.dll >nul 2>&1
    del /f /q %DISTDIR%\bin\luacore.dll >nul 2>&1
    del /f /q %DISTDIR%\bin\luaext.dll >nul 2>&1
    del /f /q %DISTDIR%\bin\comet.exe >nul 2>&1
    del /f /q %DISTDIR%\bin\lpeg.dll >nul 2>&1
    del /f /q %DISTDIR%\bin\lmapm.dll >nul 2>&1
)

if exist %stopFile% (
    @del /f /q %stopFile%
    goto :stop
)

set file=false
if "%command2%" == "all" set file=true
if "%command2%" == "file" set file=true
if "%command2%" == "main" set file=true
if "%file%" == "true" (
    del /f /q %DEVCDIR%\Main\LibFile\build\win\*.suo >nul 2>&1
    MSBuild %DEVCDIR%\Main\LibFile\build\win\__LibFile%BITS%.sln /t:rebuild
    if errorlevel 1 (
        set /p continueBuild="error occurred during compilation. continue anyway (y/n)? "
        if "!continueBuild!" == "n" (
            goto :onerror
        )
    )
)

if exist %stopFile% (
    @del /f /q %stopFile%
    goto :stop
)

set wx=false
if "%command2%" == "all" set wx=true
if "%command2%" == "wx" set wx=true
if "%wx%" == "true" (
    del /f /q %DEVCDIR%\Main\wxWidgets\build\msw%BITS%\*.suo >nul 2>&1
    MSBuild %DEVCDIR%\Main\wxWidgets\build\win%BITS%\wx%BITS%.sln /t:rebuild
    if errorlevel 1 (
        set /p continueBuild="error occurred during compilation. continue anyway (y/n)? "
        if "!continueBuild!" == "n" (
            goto :onerror
        )
    )
    del /f /q %DEVCDIR%\Main\wxWidgets\contrib\build\stc\*.suo >nul 2>&1
    MSBuild %DEVCDIR%\Main\wxWidgets\contrib\build\stc\stc%BITS%.sln /t:rebuild
    if errorlevel 1 (
        set /p continueBuild="error occurred during compilation. continue anyway (y/n)? "
        if "!continueBuild!" == "n" (
            goto :onerror
        )
    )
    del /f /q %DEVCDIR%\Main\wxWidgets\sqlite\*.suo >nul 2>&1
    MSBuild %DEVCDIR%\Main\wxWidgets\sqlite\sqlite%BITS%.sln /t:rebuild
    if errorlevel 1 (
        set /p continueBuild="error occurred during compilation. continue anyway (y/n)? "
        if "!continueBuild!" == "n" (
            goto :onerror
        )
    )
    del /f /q %DEVCDIR%\Main\wxWidgets\wxsqlite\build\*.suo >nul 2>&1
    MSBuild %DEVCDIR%\Main\wxWidgets\wxsqlite\build\wxsqlite%BITS%.sln /t:rebuild
    if errorlevel 1 (
        set /p continueBuild="error occurred during compilation. continue anyway (y/n)? "
        if "!continueBuild!" == "n" (
            goto :onerror
        )
    )
)

if exist %stopFile% (
    @del /f /q %stopFile%
    goto :stop
)

set lua=false
if "%command2%" == "all" set lua=true
if "%command2%" == "lua" set lua=true
if "%command2%" == "main" set lua=true
if "%lua%" == "true" (
    pushd %DEVCDIR%\Main\LibLua\LuaJIT\src
    call msvcbuild.bat %BITS%
    if errorlevel 1 (
        set /p continueBuild="error occurred during compilation. continue anyway (y/n)? "
        if "!continueBuild!" == "n" (
            popd
            goto :onerror
        )
    )
    popd
)

if exist %stopFile% (
    @del /f /q %stopFile%
    goto :stop
)

set luaext=false
if "%command2%" == "all" set luaext=true
if "%command2%" == "lua" set luaext=true
if "%command2%" == "luaext" set luaext=true
if "%command2%" == "main" set luaext=true
if "%luaext%" == "true" (
    del /f /q %DEVCDIR%\Main\LibLua\LuaExt\build\win\*.suo >nul 2>&1
    MSBuild %DEVCDIR%\Main\LibLua\LuaExt\build\win\__LuaExt%BITS%.sln /t:rebuild
    if errorlevel 1 (
        set /p continueBuild="error occurred during compilation. continue anyway (y/n)? "
        if "!continueBuild!" == "n" (
            goto :onerror
        )
    )
)

if exist %stopFile% (
    @del /f /q %stopFile%
    goto :stop
)

set mod=false
if "%command2%" == "all" set mod=true
if "%command2%" == "luamod" set mod=true
if "%command2%" == "main" set mod=true
if "%mod%" == "true" (
    del /f /q %DEVCDIR%\Main\LibLua\LuaMod\build\win\*.suo >nul 2>&1
    MSBuild %DEVCDIR%\Main\LibLua\LuaMod\build\win\__LuaMod%BITS%.sln /t:rebuild
    if errorlevel 1 (
        set /p continueBuild="error occurred during compilation. continue anyway (y/n)? "
        if "!continueBuild!" == "n" (
            goto :onerror
        )
    )
)

if exist %stopFile% (
    @del /f /q %stopFile%
    goto :stop
)

set comet=false
if "%command2%" == "all" set comet=true
if "%command2%" == "comet" set comet=true
if "%command2%" == "main" set comet=true
if "%comet%" == "true" (
    del /f /q %DEVCDIR%\Main\Comet\build\win\*.suo >nul 2>&1
    MSBuild %DEVCDIR%\Main\Comet\build\win\__Comet%BITS%.sln /t:rebuild
    if errorlevel 1 (
        set /p continueBuild="error occurred during compilation. continue anyway (y/n)? "
        if "!continueBuild!" == "n" (
            goto :onerror
        )
    )
)

if exist %stopFile% (
    @del /f /q %stopFile%
    goto :stop
)

set release=false
if "%command2%" == "all" set release=true
if "%command2%" == "release" set release=true
if "%release%" == "true" (

    copy /y %OUTDIR%\bin\wxadv.dll %DISTDIR%\bin\
    copy /y %OUTDIR%\bin\wxaui.dll %DISTDIR%\bin\
    copy /y %OUTDIR%\bin\wxbase.dll %DISTDIR%\bin\
    copy /y %OUTDIR%\bin\wxcore.dll %DISTDIR%\bin\
    copy /y %OUTDIR%\bin\wxstc.dll %DISTDIR%\bin\
    copy /y %OUTDIR%\bin\file.dll %DISTDIR%\bin\
    copy /y %OUTDIR%\bin\luacore.dll %DISTDIR%\bin\
    copy /y %OUTDIR%\bin\luaext.dll %DISTDIR%\bin\
    copy /y %OUTDIR%\bin\comet.exe %DISTDIR%\bin\
    copy /y %OUTDIR%\bin\lpeg.dll %DISTDIR%\bin\
    copy /y %OUTDIR%\bin\lmapm.dll %DISTDIR%\bin\

    del /f /q %DISTDIR%\Comet\doc\*.* >nul 2>&1
    copy /y %DEVCDIR%\Manual\doc\comet.pdf %DISTDIR%\doc\
    del /f /q %DEVCDIR%\Comet\icons\*.* >nul 2>&1
    copy /y %DEVCDIR%\Manual\icons\* %DISTDIR%\icons\

    copy /y %DISTDIR%\doc\comet.pdf %DEVCDIR%\output\Windows%BITS%\doc\

    copy /y %DEVCDIR%\Main\Comet\etc\comet.xpm %DISTDIR%\icons\
    copy /y %DEVCDIR%\Main\Comet\etc\comet.ico %DISTDIR%\icons\

    pushd %DISTDIR%\..
    "C:\Program Files\WinRAR\WinRAR.exe" a -afzip -ep1 -r -x*.xpm -x*.png -x*.desktop -x*.cometm -x*.solm -x*.solr -x*.solm -x*.docx -x*.pptx -x*.jpg -x*.rtf -x*\_readme.txt "cometn_windows_%BITS%bit.zip" "%DISTDIR%"
    popd

    call clean.bat %BITS%

)

goto :end

:onerror

:stop

for /F "usebackq tokens=1,2 delims==" %%i in (`wmic os get LocalDateTime /VALUE 2^>NUL`) do if '.%%i.'=='.LocalDateTime.' set timeEnd=%%j
set timeEnd=%timeEnd:~0,4%-%timeEnd:~4,2%-%timeEnd:~6,2% %timeEnd:~8,2%:%timeEnd:~10,2%
echo.
echo Build stopped at %timeEnd% (started at %timeStart%)

ENDLOCAL
Pause

attrib -R %~n0%~x0
popd
exit /b 1

:end

for /F "usebackq tokens=1,2 delims==" %%i in (`wmic os get LocalDateTime /VALUE 2^>NUL`) do if '.%%i.'=='.LocalDateTime.' set timeEnd=%%j
set timeEnd=%timeEnd:~0,4%-%timeEnd:~4,2%-%timeEnd:~6,2% %timeEnd:~8,2%:%timeEnd:~10,2%

echo.
echo Build successfully ended at %timeEnd% (started at %timeStart%)

ENDLOCAL

attrib -R %~n0%~x0
popd
exit /b 0