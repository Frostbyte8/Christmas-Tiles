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
set linkFlagsDebug=/nologo /SUBSYSTEM:WINDOWS /DEBUG /DEFAULTLIB:%libsDebug%

:: Source Files

set sources=
for /f "tokens=*" %%A in (..\code\sources.txt) do call :appendSourcesStart %%A 
goto appendSourcesEnd

:appendSourcesStart
set sources=%sources%"..\code\%1" 
goto :eof

:appendSourcesEnd

"../misc/lastmodified.exe" %sources% > "../misc/current.txt"
REM comm.exe -13 "../misc/current.txt" "../misc/stale.txt" > ../misc/_filech_temp.txt
"../misc/cmpfile.exe" "../misc/current.txt" "../misc/stale.txt" > ../misc/_filech_temp.txt

set sources=

for /f "tokens=*" %%A in (..\misc\_filech_temp.txt) do call :getChangedSourcesStart %%A 
goto :getChangedSourcesEnd

:getChangedSourcesStart
set sources=%sources% %1
goto :eof

:getChangedSourcesEnd

if exist "../misc/_filech_temp.txt" (
    del "../misc/_filech_temp.txt"
)

if not defined sources ( 
    echo No relevant changes detected.
) else (
REM Ugh MSDN doesn't mention in their docs that you need to first
REM turn on EnableDelayedExpansion, and then get the value, otherwise, it
REM will always be 0.
    setlocal EnableDelayedExpansion
    cl %compFlagsDebug% %sources%
    if !errorlevel! equ 0 goto compileSuccess
    endlocal
    popd
    echo Compile Failed!
    goto :EOF
:compileSuccess
    endlocal
    link %linkFlagsDebug% *.obj
)


if exist "../misc/stale.txt" (
    del "../misc/stale.txt"
)

ren "../misc/current.txt" "../misc/stale.txt"

popd
