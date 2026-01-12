#include "include/ComplexityEstimator.h"
#include "include/RuntimeProfiler.h"
#include "include/ResultComparator.h"
#include "include/ComplexityUtils.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

void printUsage() {
    std::cout << "C++ Complexity Analyzer v6.0\n\n";
    std::cout << "Usage: complexity_analyzer [options] <source_file>\n\n";
    std::cout << "Options:\n";
    std::cout << "  --static              Run static analysis only (default)\n";
    std::cout << "  --runtime             Run runtime profiling only\n";
    std::cout << "  --both                Run both static and runtime analysis\n";
    std::cout << "  --function <name>     Specify function to analyze\n";
    std::cout << "  --timeout <seconds>   Set runtime timeout (default: 30)\n";
    std::cout << "  --sizes <n1,n2,...>   Custom input sizes for profiling\n";
    std::cout << "  --optimization <0-3>  Compiler optimization level (default: 0)\n";
    std::cout << "                        0=none, 1=basic, 2=moderate, 3=aggressive\n";
    std::cout << "  --help                Show this help message\n\n";
    std::cout << "Examples:\n";
    std::cout << "  complexity_analyzer code.cpp\n";
    std::cout << "  complexity_analyzer --runtime --function bubbleSort code.cpp\n";
    std::cout << "  complexity_analyzer --both --sizes 10,100,1000 code.cpp\n";
    std::cout << "  complexity_analyzer --both --optimization 0 --function sort code.cpp\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage();
        return 1;
    }

    std::string mode = "static";
    std::string sourceFile;
    std::string functionName;
    int timeout = 30;
    int optimizationLevel = 0;

    std::vector<int> sizes = {1, 10, 50, 100, 500, 1000, 5000, 10000, 50000, 100000};

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "--help") {
            printUsage();
            return 0;
        } else if (arg == "--static") {
            mode = "static";
        } else if (arg == "--runtime") {
            mode = "runtime";
        } else if (arg == "--both") {
            mode = "both";
        } else if (arg == "--function" && i + 1 < argc) {
            functionName = argv[++i];
        } else if (arg == "--timeout" && i + 1 < argc) {
            timeout = std::stoi(argv[++i]);
        } else if (arg == "--optimization" && i + 1 < argc) {
            optimizationLevel = std::stoi(argv[++i]);
            if (optimizationLevel < 0 || optimizationLevel > 3) {
                std::cerr << "Error: Optimization level must be 0-3\n";
                return 1;
            }
        } else if (arg == "--sizes" && i + 1 < argc) {

            sizes.clear();
            std::string sizeStr = argv[++i];
            size_t pos = 0;
            while ((pos = sizeStr.find(',')) != std::string::npos) {
                sizes.push_back(std::stoi(sizeStr.substr(0, pos)));
                sizeStr.erase(0, pos + 1);
            }
            sizes.push_back(std::stoi(sizeStr));
        } else if (arg[0] != '-') {
            sourceFile = arg;
        }
    }

    if (sourceFile.empty()) {
        std::cerr << "Error: No source file specified\n";
        return 1;
    }

    std::ifstream file(sourceFile);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << sourceFile << "\n";
        return 1;
    }

    std::string code((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());
    file.close();

    std::cout << "=== C++ Complexity Analyzer v6.0 ===\n";
    std::cout << "Analyzing: " << sourceFile << "\n\n";

    ComplexityResult staticResult;
    ProfilingResult runtimeResult;

    if (mode == "static" || mode == "both") {
        std::cout << "--- Static Analysis ---\n";
        ComplexityEstimator estimator;
        staticResult = estimator.estimateWithConfidence(code);

        std::cout << "Complexity: " << staticResult.notation << "\n";
        std::cout << "Confidence: " << staticResult.confidence << "%\n";
        std::cout << "Explanation: " << staticResult.explanation << "\n\n";
    }

    if (mode == "runtime" || mode == "both") {
        if (functionName.empty()) {
            std::cerr << "Error: --function required for runtime profiling\n";
            return 1;
        }

        std::cout << "--- Runtime Profiling ---\n";
        std::cout << "Function: " << functionName << "\n";
        std::cout << "Optimization level: -O" << optimizationLevel << "\n";
        std::cout << "Test sizes: ";
        for (size_t i = 0; i < sizes.size(); i++) {
            std::cout << sizes[i];
            if (i < sizes.size() - 1) std::cout << ", ";
        }
        std::cout << "\n";

        if (optimizationLevel >= 2) {
            std::cout << "WARNING: High optimization levels (-O2/-O3) may optimize away test code\n";
            std::cout << "         or obscure complexity. Use -O0 for clearest results.\n";
        }
        std::cout << "\n";

        RuntimeProfiler profiler;
        profiler.setTimeout(timeout);

        std::string optFlags = "-std=c++11 -O" + std::to_string(optimizationLevel) + " -Wall";
        profiler.setCompilerFlags(optFlags);

        runtimeResult = profiler.profileFunction(sourceFile, functionName, sizes);

        if (runtimeResult.success) {
            std::cout << "Detected Complexity: " << ComplexityUtils::complexityToString(runtimeResult.detectedComplexity) << "\n";
            std::cout << "Confidence: " << runtimeResult.confidence << "%\n";
            std::cout << "Measurements:\n";
            for (const auto& point : runtimeResult.measurements) {
                std::cout << "  Size " << point.inputSize << ": " << point.executionTime_us << " us\n";
            }
            std::cout << "\n";
        } else {
            std::cerr << "Runtime profiling failed: " << runtimeResult.errorMessage << "\n";
        }
    }

    if (mode == "both" && runtimeResult.success) {
        std::cout << "--- Comparison ---\n";
        ResultComparator comparator;
        ComparisonReport report = comparator.compare(staticResult, runtimeResult);

        std::cout << "Match: " << (report.match ? "YES" : "NO") << "\n";
        std::cout << "Accuracy: " << report.accuracy << "%\n";
        std::cout << "Explanation: " << report.explanation << "\n";

        if (!report.recommendations.empty()) {
            std::cout << "\nRecommendations:\n";
            for (const auto& rec : report.recommendations) {
                std::cout << "  - " << rec << "\n";
            }
        }
    }

    return 0;
}
