#ifndef COMPLEXITY_TYPES_H
#define COMPLEXITY_TYPES_H

#include <string>
#include <vector>

enum class ComplexityType {
    CONSTANT,
    LOGARITHMIC,
    LINEAR,
    LINEARITHMIC,
    QUADRATIC,
    CUBIC,
    EXPONENTIAL,
    FACTORIAL,
    UNKNOWN
};

struct LoopInfo {
    int nestingLevel;
    std::string variable;
    std::string increment;
    int startLine;
    int endLine;
    bool hasEarlyTermination;
    bool hasDependentBounds;
    double iterationFraction;
};

struct RecursionInfo {
    std::string functionName;
    int recursiveCalls;
    bool isTailRecursion;
    bool isDirectRecursion;
    std::vector<std::string> parameters;
    std::string recurrenceRelation;
};

struct ComplexityResult {
    ComplexityType type;
    std::string notation;
    std::string explanation;
    int confidence;
};

struct AlgorithmPattern {
    std::string patternName;
    ComplexityType expectedComplexity;
    int confidence;
    std::string explanation;
};

struct Parameter {
    std::string type;
    std::string name;
    bool isArray;
    bool isPointer;
};

struct FunctionSignature {
    std::string returnType;
    std::string functionName;
    std::vector<Parameter> parameters;
};

struct CompilationResult {
    bool success;
    std::string outputFile;
    std::string errorMessage;
    std::vector<std::string> warnings;
};

struct DataPoint {
    int inputSize;
    double executionTime_us;
    size_t memoryUsage_bytes;
    bool timeout;
    bool crashed;
};

struct ExecutionResult {
    bool success;
    std::string output;
    std::string errorOutput;
    int exitCode;
    bool timeout;
    bool memoryExceeded;
    bool crashed;
};

struct CurveFitResult {
    ComplexityType complexity;
    double rSquared;
    double coefficient;
};

struct ProfilingResult {
    std::string functionName;
    std::vector<DataPoint> measurements;
    ComplexityType detectedComplexity;
    int confidence;
    std::string explanation;
    bool success;
    std::string errorMessage;
};

struct ComparisonReport {
    ComplexityType staticComplexity;
    ComplexityType runtimeComplexity;
    bool match;
    double accuracy;
    std::string explanation;
    std::vector<std::string> recommendations;
};

#endif