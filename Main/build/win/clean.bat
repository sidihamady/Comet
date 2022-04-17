@echo off

rem Comet Build System
rem (C) 2010-2022 Pr. Sidi HAMADY
rem http://www.hamady.org
rem sidi@hamady.org

pushd "%~dp0"
attrib +R %~n0%~x0

SETLOCAL enabledelayedexpansion

set BITS=64
set command1=%1
if "%command1%" == "" set command1=64
if "%command1%" == "32" (
    set BITS=32
)

set SCRIPTDIR="%~dp0"
set DEVCDIR=%SCRIPTDIR%\..\..\..
set OBJDIR=%DEVCDIR%\output\Windows64\obj
set CNFDIR=%DEVCDIR%\output\Windows64\config
set DISTDIR=%DEVCDIR%\Dist\Windows64\Comet
if "%BITS%" == "32" (
    set OBJDIR=%DEVCDIR%\output\Windows32\obj
    set CNFDIR=%DEVCDIR%\output\Windows32\config
    set DISTDIR=%DEVCDIR%\Dist\Windows32\Comet
)

@del /s /f /q %OBJDIR%\*.* >nul 2>&1
for /d %%p in (%OBJDIR%\*.*) do rmdir "%%p" /s /q >nul 2>&1

@del /s /f /q %CNFDIR%\*.* >nul 2>&1
for /d %%p in (%CNFDIR%\*.*) do rmdir "%%p" /s /q >nul 2>&1

@del /s /f /q %DISTDIR%\config\*.* >nul 2>&1
for /d %%p in (%DISTDIR%\config\*.*) do rmdir "%%p" /s /q >nul 2>&1

@del /s /f /q %DISTDIR%\icons\*.* >nul 2>&1
for /d %%p in (%DISTDIR%\icons\*.*) do rmdir "%%p" /s /q >nul 2>&1

pushd %DEVCDIR%\Main\LibLua\LuaJIT\src
call clean.bat
popd
