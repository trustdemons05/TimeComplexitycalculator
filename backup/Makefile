# C++ Complexity Analyzer v4.0 Makefile

CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2
TARGET = complexity_analyzer_v4

# Source files
SOURCES = main.cpp
HEADERS = ComplexityTypes.h Parser.cpp LoopAnalyzer.cpp RecursionAnalyzer.cpp ComplexityEstimator.cpp

# Default target
all: $(TARGET)

# Build the main executable
$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.cpp
	@echo "v4.0 Build complete! Full-featured complexity analyzer."

# Clean build artifacts
clean:
	del /Q $(TARGET).exe 2>nul || echo "Nothing to clean"

# Test with sample files
test: $(TARGET)
	@echo "=== Testing v4.0 with sample code ==="
	@echo "int main() { for(int i=0; i<n; i++) { for(int j=0; j<n; j++) { printf(\"test\"); } } return 0; }" > test_sample.c
	./$(TARGET) test_sample.c
	@rm -f test_sample.c

.PHONY: all clean test