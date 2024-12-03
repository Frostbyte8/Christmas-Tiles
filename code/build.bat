@echo off

cd /D "%~dp0"

if NOT exist "..\build" (
    mkdir ..\build
)

pushd ..\build

:: Set Compiler Settings

set libsShared=user32.lib comctl32.lib Ole32.lib Gdi32.lib Advapi32.lib

set compFlagsDebug=/nologo /Zi /FC /EHsc
set linkFlagsDebug=/SUBSYSTEM:WINDOWS
set libsDebug=%libsShared%

:: Source Files

set sources=
for /f "tokens=*" %%A in (..\code\sources.txt) do call :appendSourcesStart %%A 
goto appendSourcesEnd

:appendSourcesStart
set sources=%sources%..\code\%1 
goto :eof

:appendSourcesEnd

cl %compFlagsDebug% %sources% /link %linkFlagsDebug% %libsDebug%

popd
