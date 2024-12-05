@echo off

cd /D "%~dp0"

if NOT exist "..\..\misc\" (
	mkdir "..\..\misc\"
)

cl.exe /Ox cmpfile.cpp
move /y cmpfile.exe ..\..\misc\
del cmpfile.obj

