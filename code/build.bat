@echo off

set arg1=%1

cd /D "%~dp0"

if NOT exist "..\build" (
    mkdir ..\build
)
pushd ..\build

if "%arg1%" == "clean" (
    del *.obj
    del *.pdb
    del *.ilk
    del main.exe
) else (
    nmake /f ..\code\makefile main
)


popd
