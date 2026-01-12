# C++ Complexity Analyzer v6.0

A comprehensive tool for analyzing time complexity of C/C++ code through both static analysis and runtime profiling.

**NEW: Desktop GUI Application Available!** See [GUI Quick Start](gui/QUICKSTART.md)

## Features

### Static Analysis
- **Loop Analysis**: Detects nested loops, dependent bounds, logarithmic increments, early termination
- **Recursion Analysis**: Identifies direct/indirect recursion, tail recursion, recurrence relations
- **Pattern Detection**: Recognizes common algorithms (binary search, merge sort, quick sort, dynamic programming)
- **Confidence Scores**: Provides reliability metrics for all analyses

### Runtime Profiling
- **Automatic Test Generation**: Creates test harnesses for your functions
- **Cross-Platform Compilation**: Works with g++, clang++, and MSVC
- **Safe Execution**: Runs code with timeouts and memory limits
- **Empirical Analysis**: Measures actual performance and determines complexity from growth rates

### Comparison & Recommendations
- **Result Comparison**: Compares static predictions with runtime measurements
- **Accuracy Metrics**: Calculates how well static analysis predicted actual behavior
- **Optimization Suggestions**: Provides actionable recommendations

## Installation

### Prerequisites
- C++11 compatible compiler (g++, clang++, or MSVC)
- Make (optional, for building with Makefile)

### Building

```bash
# Using Make (Linux/macOS/MinGW)
make all

# Manual compilation
g++ -std=c++11 -O2 -I./include -o bin/complexity_analyzer main.cpp build/*.o
```

## Usage

### GUI Application (Recommended)

**Windows:**
```cmd
python gui\complexity_analyzer_gui.py
```
Or double-click `gui/run_gui.bat`

**Linux/macOS:**
```bash
python3 gui/complexity_analyzer_gui.py
```

See [GUI Quick Start Guide](gui/QUICKSTART.md) for details.

### Command-Line Interface

#### Basic Static Analysis
```bash
./complexity_analyzer code.cpp
```

#### Runtime Profiling
```bash
./complexity_analyzer --runtime --function bubbleSort code.cpp
```

### Combined Analysis
```bash
./complexity_analyzer --both --function linearSearch code.cpp
```

### Custom Options
```bash
./complexity_analyzer --both --function mergeSort --sizes 10,100,1000,10000 --timeout 60 code.cpp
```

## Command-Line Options

| Option | Description |
|--------|-------------|
| `--static` | Run static analysis only (default) |
| `--runtime` | Run runtime profiling only |
| `--both` | Run both analyses and compare |
| `--function <name>` | Specify function to analyze (required for runtime) |
| `--timeout <seconds>` | Set execution timeout (default: 30) |
| `--sizes <n1,n2,...>` | Custom input sizes for profiling |
| `--help` | Show help message |

## Examples

### Example 1: Analyze Bubble Sort
```cpp
// bubble_sort.cpp
void bubbleSort(int arr[], int n) {
    for(int i = 0; i < n-1; i++) {
        for(int j = 0; j < n-i-1; j++) {
            if(arr[j] > arr[j+1]) {
                int temp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = temp;
            }
        }
    }
}
```

```bash
$ ./complexity_analyzer --static bubble_sort.cpp
Complexity: O(n²)
Confidence: 89%
Explanation: Found 2 nested loops with max nesting level 2
```

### Example 2: Profile Binary Search
```cpp
// binary_search.cpp
int binarySearch(int arr[], int left, int right, int target) {
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (arr[mid] == target) return mid;
        if (arr[mid] < target) left = mid + 1;
        else right = mid - 1;
    }
    return -1;
}
```

```bash
$ ./complexity_analyzer --both --function binarySearch binary_search.cpp
Static: O(log n) (85% confidence)
Runtime: O(log n) (95% confidence)
Match: YES
Accuracy: 100%
```

## Architecture

### Core Components

1. **Parser**: Tokenizes and preprocesses C/C++ code
2. **LoopAnalyzer**: Analyzes loop structures and nesting
3. **RecursionAnalyzer**: Detects and analyzes recursive patterns
4. **PatternDetector**: Identifies known algorithm patterns
5. **ComplexityEstimator**: Combines all analyses with confidence scores

### Runtime Profiling Components

6. **TestGenerator**: Generates test harnesses automatically
7. **CompilerWrapper**: Handles cross-platform compilation
8. **ProcessManager**: Executes code safely with resource limits
9. **EmpiricalAnalyzer**: Analyzes runtime measurements
10. **RuntimeProfiler**: Orchestrates the profiling workflow

### Integration Components

11. **ResultComparator**: Compares static vs runtime results
12. **CLI**: Command-line interface for all features

## Supported Complexity Classes

- O(1) - Constant
- O(log n) - Logarithmic
- O(n) - Linear
- O(n log n) - Linearithmic
- O(n²) - Quadratic
- O(n³) - Cubic
- O(2^n) - Exponential
- O(n!) - Factorial

## Limitations

- Static analysis may not detect all runtime optimizations
- Runtime profiling requires compilable, executable code
- Small input sizes may not reveal true complexity
- Platform-specific optimizations may affect results

## Contributing

This project was developed as part of a complexity analysis research project. Contributions are welcome!

## License

MIT License - See LICENSE file for details

## Version History

### v6.0 (Current)
- Enhanced static analysis with pattern detection
- Complete runtime profiling system
- Result comparison and recommendations
- Cross-platform support

### v5.0
- Refactored architecture with proper separation
- Improved loop and recursion analysis
- Bug fixes and performance improvements

### v4.0
- Initial release with basic static analysis

## Authors

- Aarush Bakshi - Project Lead & Architecture
- Ameya Borkar - Core Analysis
- Arnav Gupta - Build & DevOps
- Aditya Chimurkar - Parser & Preprocessing
- Ayush - QA & Testing

## Acknowledgments

Built with modern C++ best practices and comprehensive testing.
