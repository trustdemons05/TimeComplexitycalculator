# C++ Complexity Analyzer v5.0 Makefile - Production Ready
# Proper header/implementation separation with optimizations

CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O3 -I./include
DEBUGFLAGS = -g -DDEBUG
LDFLAGS =

# Directories
SRC_DIR = src
INC_DIR = include
TEST_DIR = test
BUILD_DIR = build
BIN_DIR = bin

# Source files
UTIL_SOURCES = $(SRC_DIR)/CodeUtils.cpp $(SRC_DIR)/ComplexityUtils.cpp
ANALYZER_SOURCES = $(SRC_DIR)/Parser.cpp $(SRC_DIR)/LoopAnalyzer.cpp $(SRC_DIR)/RecursionAnalyzer.cpp $(SRC_DIR)/PatternDetector.cpp $(SRC_DIR)/ComplexityEstimator.cpp $(SRC_DIR)/ResultComparator.cpp
PROFILER_SOURCES = $(SRC_DIR)/TestGenerator.cpp $(SRC_DIR)/CompilerWrapper.cpp $(SRC_DIR)/ProcessManager.cpp $(SRC_DIR)/EmpiricalAnalyzer.cpp $(SRC_DIR)/RuntimeProfiler.cpp
ALL_SOURCES = $(UTIL_SOURCES) $(ANALYZER_SOURCES) $(PROFILER_SOURCES)

# Object files
UTIL_OBJECTS = $(BUILD_DIR)/CodeUtils.o $(BUILD_DIR)/ComplexityUtils.o
ANALYZER_OBJECTS = $(BUILD_DIR)/Parser.o $(BUILD_DIR)/LoopAnalyzer.o $(BUILD_DIR)/RecursionAnalyzer.o $(BUILD_DIR)/PatternDetector.o $(BUILD_DIR)/ComplexityEstimator.o $(BUILD_DIR)/ResultComparator.o
PROFILER_OBJECTS = $(BUILD_DIR)/TestGenerator.o $(BUILD_DIR)/CompilerWrapper.o $(BUILD_DIR)/ProcessManager.o $(BUILD_DIR)/EmpiricalAnalyzer.o $(BUILD_DIR)/RuntimeProfiler.o
ALL_OBJECTS = $(UTIL_OBJECTS) $(ANALYZER_OBJECTS) $(PROFILER_OBJECTS)

# Targets
TARGET = $(BIN_DIR)/complexity_analyzer_v5
TEST_TARGET = $(BIN_DIR)/test_compile
BENCHMARK_TARGET = $(BIN_DIR)/performance_benchmark

# Colors for output
GREEN = \033[0;32m
YELLOW = \033[0;33m
RED = \033[0;31m
NC = \033[0m # No Color

# Default target
all: directories $(TARGET)
	@echo "$(GREEN)✓ Build complete! v5.0 Production Ready$(NC)"

# Create necessary directories
directories:
	@mkdir -p $(BUILD_DIR) $(BIN_DIR)

# Build main executable
$(TARGET): $(ALL_OBJECTS) main.cpp
	@echo "$(YELLOW)Linking $(TARGET)...$(NC)"
	$(CXX) $(CXXFLAGS) -o $@ main.cpp $(ALL_OBJECTS) $(LDFLAGS)
	@echo "$(GREEN)✓ Executable created: $(TARGET)$(NC)"

# Build test executable
test: directories $(ALL_OBJECTS) test_compile.cpp
	@echo "$(YELLOW)Building test executable...$(NC)"
	$(CXX) $(CXXFLAGS) -o $(TEST_TARGET) test_compile.cpp $(ALL_OBJECTS) $(LDFLAGS)
	@echo "$(GREEN)✓ Test executable created: $(TEST_TARGET)$(NC)"
	@echo "$(YELLOW)Running tests...$(NC)"
	@$(TEST_TARGET)

# Build benchmark executable
benchmark: directories $(BUILD_DIR)/performance_benchmark.o
	@echo "$(YELLOW)Building benchmark executable...$(NC)"
	$(CXX) $(CXXFLAGS) -o $(BENCHMARK_TARGET) $(BUILD_DIR)/performance_benchmark.o $(LDFLAGS)
	@echo "$(GREEN)✓ Benchmark executable created: $(BENCHMARK_TARGET)$(NC)"

# Compile utility objects
$(BUILD_DIR)/CodeUtils.o: $(SRC_DIR)/CodeUtils.cpp $(INC_DIR)/CodeUtils.h
	@echo "$(YELLOW)Compiling CodeUtils...$(NC)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/ComplexityUtils.o: $(SRC_DIR)/ComplexityUtils.cpp $(INC_DIR)/ComplexityUtils.h $(INC_DIR)/ComplexityTypes.h
	@echo "$(YELLOW)Compiling ComplexityUtils...$(NC)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile analyzer objects
$(BUILD_DIR)/Parser.o: $(SRC_DIR)/Parser.cpp $(INC_DIR)/Parser.h $(INC_DIR)/CodeUtils.h
	@echo "$(YELLOW)Compiling Parser...$(NC)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/LoopAnalyzer.o: $(SRC_DIR)/LoopAnalyzer.cpp $(INC_DIR)/LoopAnalyzer.h $(INC_DIR)/CodeUtils.h $(INC_DIR)/ComplexityUtils.h
	@echo "$(YELLOW)Compiling LoopAnalyzer...$(NC)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/RecursionAnalyzer.o: $(SRC_DIR)/RecursionAnalyzer.cpp $(INC_DIR)/RecursionAnalyzer.h $(INC_DIR)/CodeUtils.h $(INC_DIR)/ComplexityUtils.h
	@echo "$(YELLOW)Compiling RecursionAnalyzer...$(NC)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/PatternDetector.o: $(SRC_DIR)/PatternDetector.cpp $(INC_DIR)/PatternDetector.h $(INC_DIR)/CodeUtils.h
	@echo "$(YELLOW)Compiling PatternDetector...$(NC)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/ComplexityEstimator.o: $(SRC_DIR)/ComplexityEstimator.cpp $(INC_DIR)/ComplexityEstimator.h
	@echo "$(YELLOW)Compiling ComplexityEstimator...$(NC)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/ResultComparator.o: $(SRC_DIR)/ResultComparator.cpp $(INC_DIR)/ResultComparator.h
	@echo "$(YELLOW)Compiling ResultComparator...$(NC)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile profiler objects
$(BUILD_DIR)/TestGenerator.o: $(SRC_DIR)/TestGenerator.cpp $(INC_DIR)/TestGenerator.h $(INC_DIR)/CodeUtils.h
	@echo "$(YELLOW)Compiling TestGenerator...$(NC)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/CompilerWrapper.o: $(SRC_DIR)/CompilerWrapper.cpp $(INC_DIR)/CompilerWrapper.h
	@echo "$(YELLOW)Compiling CompilerWrapper...$(NC)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/ProcessManager.o: $(SRC_DIR)/ProcessManager.cpp $(INC_DIR)/ProcessManager.h
	@echo "$(YELLOW)Compiling ProcessManager...$(NC)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/EmpiricalAnalyzer.o: $(SRC_DIR)/EmpiricalAnalyzer.cpp $(INC_DIR)/EmpiricalAnalyzer.h $(INC_DIR)/ComplexityUtils.h
	@echo "$(YELLOW)Compiling EmpiricalAnalyzer...$(NC)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/RuntimeProfiler.o: $(SRC_DIR)/RuntimeProfiler.cpp $(INC_DIR)/RuntimeProfiler.h
	@echo "$(YELLOW)Compiling RuntimeProfiler...$(NC)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile benchmark
$(BUILD_DIR)/performance_benchmark.o: $(TEST_DIR)/performance_benchmark.cpp
	@echo "$(YELLOW)Compiling performance benchmark...$(NC)"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Debug build
debug: CXXFLAGS += $(DEBUGFLAGS)
debug: clean all
	@echo "$(GREEN)✓ Debug build complete$(NC)"

# Clean build artifacts
clean:
	@echo "$(YELLOW)Cleaning build artifacts...$(NC)"
	@rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "$(GREEN)✓ Clean complete$(NC)"

# Clean and rebuild
rebuild: clean all

# Run static analysis (if cppcheck is installed)
analyze:
	@echo "$(YELLOW)Running static analysis...$(NC)"
	@cppcheck --enable=all --suppress=missingIncludeSystem $(SRC_DIR)/*.cpp 2>&1 | grep -v "^Checking" || true
	@echo "$(GREEN)✓ Analysis complete$(NC)"

# Format code (if clang-format is installed)
format:
	@echo "$(YELLOW)Formatting code...$(NC)"
	@clang-format -i $(SRC_DIR)/*.cpp $(INC_DIR)/*.h 2>/dev/null || echo "$(RED)clang-format not found$(NC)"

# Show build info
info:
	@echo "$(GREEN)=== Build Information ===$(NC)"
	@echo "Compiler: $(CXX)"
	@echo "Flags: $(CXXFLAGS)"
	@echo "Sources: $(ALL_SOURCES)"
	@echo "Objects: $(ALL_OBJECTS)"
	@echo "Target: $(TARGET)"

# Help
help:
	@echo "$(GREEN)=== C++ Complexity Analyzer v5.0 - Makefile Help ===$(NC)"
	@echo ""
	@echo "Available targets:"
	@echo "  $(YELLOW)all$(NC)       - Build the main executable (default)"
	@echo "  $(YELLOW)test$(NC)      - Build and run component tests"
	@echo "  $(YELLOW)benchmark$(NC) - Build performance benchmark tool"
	@echo "  $(YELLOW)debug$(NC)     - Build with debug symbols"
	@echo "  $(YELLOW)clean$(NC)     - Remove all build artifacts"
	@echo "  $(YELLOW)rebuild$(NC)   - Clean and rebuild"
	@echo "  $(YELLOW)analyze$(NC)   - Run static code analysis"
	@echo "  $(YELLOW)format$(NC)    - Format code with clang-format"
	@echo "  $(YELLOW)info$(NC)      - Show build information"
	@echo "  $(YELLOW)help$(NC)      - Show this help message"
	@echo ""
	@echo "Examples:"
	@echo "  make test      # Build and run tests"
	@echo "  make debug     # Build with debugging enabled"
	@echo "  make clean all # Clean rebuild"

.PHONY: all test benchmark debug clean rebuild analyze format info help directories
