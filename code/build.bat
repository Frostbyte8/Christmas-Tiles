@echo off

cd /D "%~dp0"

if NOT exist "..\build" (
    mkdir ..\build
)

pushd ..\build

:: Set Compiler Settings

set libsShared=user32.lib comctl32.lib Ole32.lib Gdi32.lib Advapi32.lib

set compFlagsDebug=/nologo /c /Zi /FC /EHsc

set libsDebug=%libsShared%
set linkFlagsDebug=/nologo /SUBSYSTEM:WINDOWS /DEFAULTLIB:%libsDebug%

:: Source Files

set sources=
for /f "tokens=*" %%A in (..\code\sources.txt) do call :appendSourcesStart %%A 
goto appendSourcesEnd

:appendSourcesStart
set sources=%sources%"..\code\%1" 
goto :eof

:appendSourcesEnd

"../misc/lastmodified.exe" %sources% > "../misc/current.txt"
"../misc/comm.exe" -13 "../misc/current.txt" "../misc/stale.txt" > ../misc/_filech_temp.txt

set sources=

for /f "tokens=*" %%A in (..\misc\_filech_temp.txt) do call :getChangedSourcesStart %%A 
goto :getChangedSourcesEnd

:getChangedSourcesStart
set sources=%sources%%1
goto :eof

:getChangedSourcesEnd

if exist "../misc/stale.txt" (
    del "../misc/stale.txt"
)

if exist "../misc/_filech_temp.txt" (
    del "../misc/_filech_temp.txt"
)

ren "../misc/current.txt" "../misc/stale.txt"

echo %sources%
if not defined sources ( 
    echo No relevant changes detected.
) else (
    cl %compFlagsDebug% %sources%
    link %linkFlagsDebug% *.obj
)

popd
