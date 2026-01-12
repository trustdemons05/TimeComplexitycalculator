@echo off
REM C++ Complexity Analyzer - Windows Build Script
REM Detects g++, compiles all sources, and links executable

setlocal enabledelayedexpansion

echo [92m=== C++ Complexity Analyzer Build System ===[0m
echo.

REM Check if g++ is available
where g++ >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [91mERROR: g++ compiler not found![0m
    echo Please install MinGW or add g++ to your PATH
    echo Download from: https://sourceforge.net/projects/mingw-w64/
    exit /b 1
)

echo [93mDetected compiler:[0m
g++ --version | findstr /C:"g++"
echo.

REM Create directories
echo [93mCreating build directories...[0m
if not exist build mkdir build
if not exist bin mkdir bin
echo [92mDirectories created.[0m
echo.

REM Compile utility sources
echo [93mCompiling utility sources...[0m
g++ -std=c++11 -Wall -Wextra -O2 -I./include -c src/CodeUtils.cpp -o build/CodeUtils.o
if %ERRORLEVEL% NEQ 0 (
    echo [91mERROR: Failed to compile CodeUtils.cpp[0m
    exit /b 1
)
echo [92m  ✓ CodeUtils.cpp[0m

g++ -std=c++11 -Wall -Wextra -O2 -I./include -c src/ComplexityUtils.cpp -o build/ComplexityUtils.o
if %ERRORLEVEL% NEQ 0 (
    echo [91mERROR: Failed to compile ComplexityUtils.cpp[0m
    exit /b 1
)
echo [92m  ✓ ComplexityUtils.cpp[0m

REM Compile analyzer sources
echo [93mCompiling analyzer sources...[0m
g++ -std=c++11 -Wall -Wextra -O2 -I./include -c src/Parser.cpp -o build/Parser.o
if %ERRORLEVEL% NEQ 0 (
    echo [91mERROR: Failed to compile Parser.cpp[0m
    exit /b 1
)
echo [92m  ✓ Parser.cpp[0m

g++ -std=c++11 -Wall -Wextra -O2 -I./include -c src/LoopAnalyzer.cpp -o build/LoopAnalyzer.o
if %ERRORLEVEL% NEQ 0 (
    echo [91mERROR: Failed to compile LoopAnalyzer.cpp[0m
    exit /b 1
)
echo [92m  ✓ LoopAnalyzer.cpp[0m

g++ -std=c++11 -Wall -Wextra -O2 -I./include -c src/RecursionAnalyzer.cpp -o build/RecursionAnalyzer.o
if %ERRORLEVEL% NEQ 0 (
    echo [91mERROR: Failed to compile RecursionAnalyzer.cpp[0m
    exit /b 1
)
echo [92m  ✓ RecursionAnalyzer.cpp[0m

g++ -std=c++11 -Wall -Wextra -O2 -I./include -c src/PatternDetector.cpp -o build/PatternDetector.o
if %ERRORLEVEL% NEQ 0 (
    echo [91mERROR: Failed to compile PatternDetector.cpp[0m
    exit /b 1
)
echo [92m  ✓ PatternDetector.cpp[0m

g++ -std=c++11 -Wall -Wextra -O2 -I./include -c src/ComplexityEstimator.cpp -o build/ComplexityEstimator.o
if %ERRORLEVEL% NEQ 0 (
    echo [91mERROR: Failed to compile ComplexityEstimator.cpp[0m
    exit /b 1
)
echo [92m  ✓ ComplexityEstimator.cpp[0m

g++ -std=c++11 -Wall -Wextra -O2 -I./include -c src/ResultComparator.cpp -o build/ResultComparator.o
if %ERRORLEVEL% NEQ 0 (
    echo [91mERROR: Failed to compile ResultComparator.cpp[0m
    exit /b 1
)
echo [92m  ✓ ResultComparator.cpp[0m

REM Compile profiler sources
echo [93mCompiling profiler sources...[0m
g++ -std=c++11 -Wall -Wextra -O2 -I./include -c src/TestGenerator.cpp -o build/TestGenerator.o
if %ERRORLEVEL% NEQ 0 (
    echo [91mERROR: Failed to compile TestGenerator.cpp[0m
    exit /b 1
)
echo [92m  ✓ TestGenerator.cpp[0m

g++ -std=c++11 -Wall -Wextra -O2 -I./include -c src/CompilerWrapper.cpp -o build/CompilerWrapper.o
if %ERRORLEVEL% NEQ 0 (
    echo [91mERROR: Failed to compile CompilerWrapper.cpp[0m
    exit /b 1
)
echo [92m  ✓ CompilerWrapper.cpp[0m

g++ -std=c++11 -Wall -Wextra -O2 -I./include -c src/ProcessManager.cpp -o build/ProcessManager.o
if %ERRORLEVEL% NEQ 0 (
    echo [91mERROR: Failed to compile ProcessManager.cpp[0m
    exit /b 1
)
echo [92m  ✓ ProcessManager.cpp[0m

g++ -std=c++11 -Wall -Wextra -O2 -I./include -c src/EmpiricalAnalyzer.cpp -o build/EmpiricalAnalyzer.o
if %ERRORLEVEL% NEQ 0 (
    echo [91mERROR: Failed to compile EmpiricalAnalyzer.cpp[0m
    exit /b 1
)
echo [92m  ✓ EmpiricalAnalyzer.cpp[0m

g++ -std=c++11 -Wall -Wextra -O2 -I./include -c src/RuntimeProfiler.cpp -o build/RuntimeProfiler.o
if %ERRORLEVEL% NEQ 0 (
    echo [91mERROR: Failed to compile RuntimeProfiler.cpp[0m
    exit /b 1
)
echo [92m  ✓ RuntimeProfiler.cpp[0m

REM Link executable
echo.
echo [93mLinking executable...[0m
g++ -std=c++11 -O2 -o bin/complexity_analyzer.exe main.cpp build/CodeUtils.o build/ComplexityUtils.o build/Parser.o build/LoopAnalyzer.o build/RecursionAnalyzer.o build/PatternDetector.o build/ComplexityEstimator.o build/ResultComparator.o build/TestGenerator.o build/CompilerWrapper.o build/ProcessManager.o build/EmpiricalAnalyzer.o build/RuntimeProfiler.o
if %ERRORLEVEL% NEQ 0 (
    echo [91mERROR: Failed to link executable[0m
    exit /b 1
)

echo.
echo [92m========================================[0m
echo [92m  BUILD SUCCESSFUL![0m
echo [92m  Executable: bin\complexity_analyzer.exe[0m
echo [92m========================================[0m
echo.

exit /b 0
