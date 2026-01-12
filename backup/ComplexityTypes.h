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
};

struct RecursionInfo {
    std::string functionName;
    int recursiveCalls;
    bool isTailRecursion;
    std::vector<std::string> parameters;
};

struct ComplexityResult {
    ComplexityType type;
    std::string notation;
    std::string explanation;
    int confidence;
};

#endif