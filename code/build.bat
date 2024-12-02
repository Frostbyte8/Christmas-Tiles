@echo off

if NOT exist "..\build" (
    mkdir ..\build
)

pushd ..\build

cl /nologo /Zi /FC ..\code\main.cpp /link /SUBSYSTEM:WINDOWS user32.lib
REM > ../misc/errorlist.txt

popd