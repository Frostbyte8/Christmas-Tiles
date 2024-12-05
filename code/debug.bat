@echo off

cd /D "%~dp0"

if NOT exist "..\build\main.exe" (
    echo Could not find main.exe to debug.
    goto EOF
)

start devenv ..\build\main.exe