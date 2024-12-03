@echo off

cd /D "%~dp0"

if NOT exist "..\build" (
    mkdir ..\build
)

pushd ..\build

REM NOTE: /EHsc is just for debug builds

cl /nologo /Zi /FC /EHsc ..\code\main.cpp /link /SUBSYSTEM:WINDOWS user32.lib comctl32.lib Ole32.lib Gdi32.lib Advapi32.lib

popd
