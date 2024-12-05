@echo off

cd /D "%~dp0"

if NOT exist "..\..\misc\" (
	mkdir "..\..\misc\"
)

cl.exe /Ox lastmodified.c
move /y lastmodified.exe ..\..\misc\
del lastmodified.obj

