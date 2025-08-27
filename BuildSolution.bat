@echo off
REM Go to the folder where this script is located
cd /d "%~dp0"

REM Run CMake configure step
echo Running CMake...
cmake -S ./ -B ./builds

REM Open an interactive command prompt in this directory
cmd