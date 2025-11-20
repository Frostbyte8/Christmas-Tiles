@echo off
set githash=fatal
FOR /F "usebackq tokens=* delims=" %%I IN (`git.exe rev-parse --short^=16 HEAD`) DO (
	SET githash=%%I
)

echo #pragma once > ".\src\gitinfo.h"

SET fatalStr=fatal
ECHO %githash% | FINDSTR /C:"%fatalStr%" >NUL

IF %ERRORLEVEL%==0 (
	echo static const wchar_t GIT_HASH[] = L"\0"; >> ".\src\gitinfo.h"
) ELSE (
	echo static const wchar_t GIT_HASH[] = L"%githash%"; >> ".\src\gitinfo.h"
)
