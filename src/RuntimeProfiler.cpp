#include "../include/RuntimeProfiler.h"
#include "../include/ComplexityUtils.h"
#include "../include/Exceptions.h"
#include "../include/PathUtils.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <map>

double calculateMedian(std::vector<double> values);
double calculateMean(const std::vector<double>& values);
double calculateStdDev(const std::vector<double>& values, double mean);
std::vector<double> removeOutliers(const std::vector<double>& values);

RuntimeProfiler::RuntimeProfiler() {
    timeoutSeconds = 30;
    memoryLimitBytes = 1024 * 1024 * 100;
    compilerFlags = "";
    runsPerSize = 10;
}

RuntimeProfiler::~RuntimeProfiler() {}

ProfilingResult RuntimeProfiler::profileFunction(
    const std::string& sourceFile,
    const std::string& functionName,
    const std::vector<int>& inputSizes
) {
    ProfilingResult result;
    result.functionName = functionName;
    result.success = false;

    try {

        if (sourceFile.empty()) {
            throw InvalidInputException("Source file path cannot be empty", "sourceFile");
        }

        if (functionName.empty()) {
            throw InvalidInputException("Function name cannot be empty", "functionName");
        }

        if (inputSizes.empty()) {
            throw InvalidInputException("Input sizes cannot be empty", "inputSizes");
        }

        for (size_t i = 0; i < inputSizes.size(); i++) {
            if (inputSizes[i] <= 0) {
                throw InvalidInputException("Input sizes must be positive", "inputSizes");
            }
            if (i > 0 && inputSizes[i] <= inputSizes[i-1]) {
                throw InvalidInputException("Input sizes must be sorted in ascending order", "inputSizes");
            }
        }

        if (timeoutSeconds <= 0) {
            throw InvalidInputException("Timeout must be positive", "timeout");
        }

        if (memoryLimitBytes < 1024*1024) {
            throw InvalidInputException("Memory limit must be at least 1MB", "memoryLimit");
        }

        std::ifstream file(sourceFile);
        if (!file.is_open()) {
            throw FileException("Could not open source file", sourceFile);
        }

        std::string sourceCode((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
        file.close();

        if (sourceCode.empty()) {
            throw FileException("Source file is empty", sourceFile);
        }

        std::string testCode = testGen.generateTestCode(sourceCode, functionName, inputSizes);

        if (testCode.find("Error:") != std::string::npos) {
            result.errorMessage = "Test generation failed: " + testCode;
            return result;
    }

    std::string testFile = "temp_profile_test.cpp";
    std::ofstream outFile(testFile);
    outFile << testCode;
    outFile.close();

        std::string executable = PathUtils::join("bin", "temp_profile_test" + PathUtils::getExecutableExtension());
        CompilationResult compResult = compiler.compile(testFile, executable, compilerFlags);

    if (!compResult.success) {
        result.errorMessage = "Compilation failed: " + compResult.errorMessage;
        return result;
    }

        std::map<int, std::vector<double>> timingsBySize;

        for (int run = 0; run < runsPerSize; run++) {

            std::string execPath = PathUtils::isAbsolute(executable) ? executable : PathUtils::join(".", executable);
            ExecutionResult execResult = procMgr.execute(
                execPath,
                timeoutSeconds,
                memoryLimitBytes
            );

            if (!execResult.success) {
                if (execResult.timeout) {
                    result.errorMessage = "Execution timed out on run " + std::to_string(run + 1);
                } else if (execResult.crashed) {
                    result.errorMessage = "Program crashed during execution on run " + std::to_string(run + 1);
                } else {
                    result.errorMessage = "Execution failed on run " + std::to_string(run + 1) + ": " + execResult.errorOutput;
                }
                return result;
            }

            std::vector<DataPoint> runMeasurements = procMgr.parseOutput(execResult.output);

            if (runMeasurements.empty()) {
                result.errorMessage = "No timing data found in output on run " + std::to_string(run + 1);
                return result;
            }

            for (const auto& point : runMeasurements) {
                timingsBySize[point.inputSize].push_back(point.executionTime_us);
            }
        }

        for (auto& pair : timingsBySize) {
            int inputSize = pair.first;
            std::vector<double>& times = pair.second;

            if (times.size() > 3) {
                times.erase(times.begin(), times.begin() + 3);
            }

            times = removeOutliers(times);

            double medianTime = calculateMedian(times);

            DataPoint point;
            point.inputSize = inputSize;
            point.executionTime_us = medianTime;
            point.memoryUsage_bytes = 0;
            point.timeout = false;
            point.crashed = false;

            result.measurements.push_back(point);
        }

    std::sort(result.measurements.begin(), result.measurements.end(),
              [](const DataPoint& a, const DataPoint& b) { return a.inputSize < b.inputSize; });

    if (result.measurements.empty()) {
        result.errorMessage = "No valid measurements after statistical processing";
        return result;
    }

        ComplexityResult analysisResult = analyzer.analyzeComplexity(result.measurements);

        result.detectedComplexity = analysisResult.type;
        result.confidence = analysisResult.confidence;
        result.explanation = analysisResult.explanation;
        result.success = true;

        lastResult = result;

        return result;
    }
    catch (const InvalidInputException& e) {
        result.errorMessage = "Input validation error: " + e.getFullMessage();
        return result;
    }
    catch (const FileException& e) {
        result.errorMessage = "File error: " + e.getFullMessage();
        return result;
    }
    catch (const CompilationException& e) {
        result.errorMessage = "Compilation error: " + e.getFullMessage();
        return result;
    }
    catch (const TimeoutException& e) {
        result.errorMessage = "Timeout: " + std::string(e.what());
        return result;
    }
    catch (const ExecutionException& e) {
        result.errorMessage = "Execution error: " + std::string(e.what());
        return result;
    }
    catch (const std::exception& e) {
        result.errorMessage = "Unexpected error: " + std::string(e.what());
        return result;
    }
}

void RuntimeProfiler::setTimeout(int seconds) {
    timeoutSeconds = seconds;
}

void RuntimeProfiler::setMemoryLimit(size_t bytes) {
    memoryLimitBytes = bytes;
}

void RuntimeProfiler::setCompilerFlags(const std::string& flags) {
    compilerFlags = flags;
}

const ProfilingResult& RuntimeProfiler::getLastResult() const {
    return lastResult;
}

void RuntimeProfiler::setRunsPerSize(int runs) {
    runsPerSize = (runs > 0) ? runs : 10;
}

double calculateMedian(std::vector<double> values) {
    if (values.empty()) return 0.0;

    std::sort(values.begin(), values.end());
    size_t n = values.size();

    if (n % 2 == 0) {
        return (values[n/2 - 1] + values[n/2]) / 2.0;
    } else {
        return values[n/2];
    }
}

double calculateMean(const std::vector<double>& values) {
    if (values.empty()) return 0.0;

    double sum = 0.0;
    for (double v : values) {
        sum += v;
    }
    return sum / values.size();
}

double calculateStdDev(const std::vector<double>& values, double mean) {
    if (values.size() < 2) return 0.0;

    double sumSquaredDiff = 0.0;
    for (double v : values) {
        double diff = v - mean;
        sumSquaredDiff += diff * diff;
    }
    return std::sqrt(sumSquaredDiff / (values.size() - 1));
}

std::vector<double> removeOutliers(const std::vector<double>& values) {
    if (values.size() < 5) return values;

    double mean = calculateMean(values);
    double stdDev = calculateStdDev(values, mean);

    std::vector<double> filtered;
    for (double v : values) {
        if (std::abs(v - mean) <= 2.0 * stdDev) {
            filtered.push_back(v);
        }
    }

    if (filtered.size() < 5) {
        return values;
    }

    return filtered;
}
