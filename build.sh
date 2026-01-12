#!/bin/bash
# C++ Complexity Analyzer - Linux/macOS Build Script
# Detects g++ or clang++, compiles all sources, and links executable

# Color codes
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo -e "${GREEN}=== C++ Complexity Analyzer Build System ===${NC}"
echo

# Detect compiler (prefer clang++ on macOS, g++ on Linux)
CXX=""
if [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS - prefer clang++
    if command -v clang++ &> /dev/null; then
        CXX="clang++"
    elif command -v g++ &> /dev/null; then
        CXX="g++"
    fi
else
    # Linux - prefer g++
    if command -v g++ &> /dev/null; then
        CXX="g++"
    elif command -v clang++ &> /dev/null; then
        CXX="clang++"
    fi
fi

if [ -z "$CXX" ]; then
    echo -e "${RED}ERROR: No C++ compiler found!${NC}"
    echo "Please install g++ or clang++"
    echo "  Ubuntu/Debian: sudo apt-get install g++"
    echo "  macOS: xcode-select --install"
    exit 1
fi

echo -e "${YELLOW}Detected compiler:${NC}"
$CXX --version | head -n 1
echo

# Create directories
echo -e "${YELLOW}Creating build directories...${NC}"
mkdir -p build
mkdir -p bin
echo -e "${GREEN}Directories created.${NC}"
echo

# Compile utility sources
echo -e "${YELLOW}Compiling utility sources...${NC}"
$CXX -std=c++11 -Wall -Wextra -O2 -I./include -c src/CodeUtils.cpp -o build/CodeUtils.o
if [ $? -ne 0 ]; then
    echo -e "${RED}ERROR: Failed to compile CodeUtils.cpp${NC}"
    exit 1
fi
echo -e "${GREEN}  ✓ CodeUtils.cpp${NC}"

$CXX -std=c++11 -Wall -Wextra -O2 -I./include -c src/ComplexityUtils.cpp -o build/ComplexityUtils.o
if [ $? -ne 0 ]; then
    echo -e "${RED}ERROR: Failed to compile ComplexityUtils.cpp${NC}"
    exit 1
fi
echo -e "${GREEN}  ✓ ComplexityUtils.cpp${NC}"

# Compile analyzer sources
echo -e "${YELLOW}Compiling analyzer sources...${NC}"
$CXX -std=c++11 -Wall -Wextra -O2 -I./include -c src/Parser.cpp -o build/Parser.o
if [ $? -ne 0 ]; then
    echo -e "${RED}ERROR: Failed to compile Parser.cpp${NC}"
    exit 1
fi
echo -e "${GREEN}  ✓ Parser.cpp${NC}"

$CXX -std=c++11 -Wall -Wextra -O2 -I./include -c src/LoopAnalyzer.cpp -o build/LoopAnalyzer.o
if [ $? -ne 0 ]; then
    echo -e "${RED}ERROR: Failed to compile LoopAnalyzer.cpp${NC}"
    exit 1
fi
echo -e "${GREEN}  ✓ LoopAnalyzer.cpp${NC}"

$CXX -std=c++11 -Wall -Wextra -O2 -I./include -c src/RecursionAnalyzer.cpp -o build/RecursionAnalyzer.o
if [ $? -ne 0 ]; then
    echo -e "${RED}ERROR: Failed to compile RecursionAnalyzer.cpp${NC}"
    exit 1
fi
echo -e "${GREEN}  ✓ RecursionAnalyzer.cpp${NC}"

$CXX -std=c++11 -Wall -Wextra -O2 -I./include -c src/PatternDetector.cpp -o build/PatternDetector.o
if [ $? -ne 0 ]; then
    echo -e "${RED}ERROR: Failed to compile PatternDetector.cpp${NC}"
    exit 1
fi
echo -e "${GREEN}  ✓ PatternDetector.cpp${NC}"

$CXX -std=c++11 -Wall -Wextra -O2 -I./include -c src/ComplexityEstimator.cpp -o build/ComplexityEstimator.o
if [ $? -ne 0 ]; then
    echo -e "${RED}ERROR: Failed to compile ComplexityEstimator.cpp${NC}"
    exit 1
fi
echo -e "${GREEN}  ✓ ComplexityEstimator.cpp${NC}"

$CXX -std=c++11 -Wall -Wextra -O2 -I./include -c src/ResultComparator.cpp -o build/ResultComparator.o
if [ $? -ne 0 ]; then
    echo -e "${RED}ERROR: Failed to compile ResultComparator.cpp${NC}"
    exit 1
fi
echo -e "${GREEN}  ✓ ResultComparator.cpp${NC}"

# Compile profiler sources
echo -e "${YELLOW}Compiling profiler sources...${NC}"
$CXX -std=c++11 -Wall -Wextra -O2 -I./include -c src/TestGenerator.cpp -o build/TestGenerator.o
if [ $? -ne 0 ]; then
    echo -e "${RED}ERROR: Failed to compile TestGenerator.cpp${NC}"
    exit 1
fi
echo -e "${GREEN}  ✓ TestGenerator.cpp${NC}"

$CXX -std=c++11 -Wall -Wextra -O2 -I./include -c src/CompilerWrapper.cpp -o build/CompilerWrapper.o
if [ $? -ne 0 ]; then
    echo -e "${RED}ERROR: Failed to compile CompilerWrapper.cpp${NC}"
    exit 1
fi
echo -e "${GREEN}  ✓ CompilerWrapper.cpp${NC}"

$CXX -std=c++11 -Wall -Wextra -O2 -I./include -c src/ProcessManager.cpp -o build/ProcessManager.o
if [ $? -ne 0 ]; then
    echo -e "${RED}ERROR: Failed to compile ProcessManager.cpp${NC}"
    exit 1
fi
echo -e "${GREEN}  ✓ ProcessManager.cpp${NC}"

$CXX -std=c++11 -Wall -Wextra -O2 -I./include -c src/EmpiricalAnalyzer.cpp -o build/EmpiricalAnalyzer.o
if [ $? -ne 0 ]; then
    echo -e "${RED}ERROR: Failed to compile EmpiricalAnalyzer.cpp${NC}"
    exit 1
fi
echo -e "${GREEN}  ✓ EmpiricalAnalyzer.cpp${NC}"

$CXX -std=c++11 -Wall -Wextra -O2 -I./include -c src/RuntimeProfiler.cpp -o build/RuntimeProfiler.o
if [ $? -ne 0 ]; then
    echo -e "${RED}ERROR: Failed to compile RuntimeProfiler.cpp${NC}"
    exit 1
fi
echo -e "${GREEN}  ✓ RuntimeProfiler.cpp${NC}"

# Link executable
echo
echo -e "${YELLOW}Linking executable...${NC}"
$CXX -std=c++11 -O2 -o bin/complexity_analyzer main.cpp \
    build/CodeUtils.o \
    build/ComplexityUtils.o \
    build/Parser.o \
    build/LoopAnalyzer.o \
    build/RecursionAnalyzer.o \
    build/PatternDetector.o \
    build/ComplexityEstimator.o \
    build/ResultComparator.o \
    build/TestGenerator.o \
    build/CompilerWrapper.o \
    build/ProcessManager.o \
    build/EmpiricalAnalyzer.o \
    build/RuntimeProfiler.o

if [ $? -ne 0 ]; then
    echo -e "${RED}ERROR: Failed to link executable${NC}"
    exit 1
fi

# Set executable permissions
chmod +x bin/complexity_analyzer

echo
echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}  BUILD SUCCESSFUL!${NC}"
echo -e "${GREEN}  Executable: bin/complexity_analyzer${NC}"
echo -e "${GREEN}========================================${NC}"
echo

exit 0
