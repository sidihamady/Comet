@echo off

attrib +R %~n0%~x0

@del /f /q *.obj >nul 2>&1
@del /f /q *.manifest >nul 2>&1
@del /f /q minilua.exe >nul 2>&1
@del /f /q buildvm.exe >nul 2>&1
@del /f /q host\buildvm_arch.h >nul 2>&1
@del /f /q lj_bcdef.h >nul 2>&1
@del /f /q lj_ffdef.h >nul 2>&1
@del /f /q lj_libdef.h >nul 2>&1
@del /f /q lj_recdef.h >nul 2>&1
@del /f /q lj_folddef.h >nul 2>&1

@echo.

attrib -R %~n0%~x0
