@echo off
setlocal

set "COMPILER=g++"
set "APP_OUTPUT=RollingAnimals.exe"
set "APP_SOURCES=main.cpp FileStorage.cpp dice.cpp MagicDice.cpp AnimalDie.cpp"

where %COMPILER% >nul 2>nul
if errorlevel 1 (
    echo g++ was not found on your PATH.
    echo Install MinGW/MSYS2 or another C++ compiler, then add g++ to PATH and run build.bat again.
    exit /b 1
)

if exist "%APP_OUTPUT%" (
    del /f /q "%APP_OUTPUT%"
)

"%COMPILER%" -std=c++17 -Wall -Wextra -I. %APP_SOURCES% -o "%APP_OUTPUT%"
if errorlevel 1 (
    echo App build failed.
    exit /b 1
)

echo Build complete: %APP_OUTPUT%
