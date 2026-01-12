@echo off
REM Launcher script for C++ Complexity Analyzer GUI (Windows)

echo Starting C++ Complexity Analyzer GUI...
echo.

REM Check if Python is installed
python --version >nul 2>&1
if %errorlevel% neq 0 (
    echo Error: Python is not installed or not in PATH
    echo Please install Python 3.6 or higher from https://www.python.org/
    pause
    exit /b 1
)

REM Check if analyzer is built
if not exist "..\bin\complexity_analyzer.exe" (
    echo Warning: Analyzer executable not found!
    echo Please build the project first by running: build.bat
    echo.
    pause
)

REM Run the GUI
python complexity_analyzer_gui.py

if %errorlevel% neq 0 (
    echo.
    echo Error: Failed to start GUI
    pause
)
