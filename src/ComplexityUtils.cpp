#include "../include/ComplexityUtils.h"

int ComplexityUtils::getComplexityRank(ComplexityType type) {
    switch (type) {
        case ComplexityType::CONSTANT:     return 0;
        case ComplexityType::LOGARITHMIC:  return 1;
        case ComplexityType::LINEAR:       return 2;
        case ComplexityType::LINEARITHMIC: return 3;
        case ComplexityType::QUADRATIC:    return 4;
        case ComplexityType::CUBIC:        return 5;
        case ComplexityType::EXPONENTIAL:  return 6;
        case ComplexityType::FACTORIAL:    return 7;
        case ComplexityType::UNKNOWN:
        default:                           return -1;
    }
}

std::string ComplexityUtils::complexityToString(ComplexityType type) {
    switch (type) {
        case ComplexityType::CONSTANT:     return "O(1)";
        case ComplexityType::LOGARITHMIC:  return "O(log n)";
        case ComplexityType::LINEAR:       return "O(n)";
        case ComplexityType::LINEARITHMIC: return "O(n log n)";
        case ComplexityType::QUADRATIC:    return "O(n²)";
        case ComplexityType::CUBIC:        return "O(n³)";
        case ComplexityType::EXPONENTIAL:  return "O(2^n)";
        case ComplexityType::FACTORIAL:    return "O(n!)";
        case ComplexityType::UNKNOWN:
        default:                           return "O(?)";
    }
}

std::string ComplexityUtils::complexityToName(ComplexityType type) {
    switch (type) {
        case ComplexityType::CONSTANT:     return "Constant";
        case ComplexityType::LOGARITHMIC:  return "Logarithmic";
        case ComplexityType::LINEAR:       return "Linear";
        case ComplexityType::LINEARITHMIC: return "Linearithmic";
        case ComplexityType::QUADRATIC:    return "Quadratic";
        case ComplexityType::CUBIC:        return "Cubic";
        case ComplexityType::EXPONENTIAL:  return "Exponential";
        case ComplexityType::FACTORIAL:    return "Factorial";
        case ComplexityType::UNKNOWN:
        default:                           return "Unknown";
    }
}

int ComplexityUtils::compareComplexity(ComplexityType a, ComplexityType b) {
    int rankA = getComplexityRank(a);
    int rankB = getComplexityRank(b);

    if (rankA < rankB) return -1;
    if (rankA > rankB) return 1;
    return 0;
}

ComplexityType ComplexityUtils::maxComplexity(ComplexityType a, ComplexityType b) {
    return (getComplexityRank(a) >= getComplexityRank(b)) ? a : b;
}

ComplexityType ComplexityUtils::combineComplexities(ComplexityType loopComplexity,
                                                    ComplexityType recursionComplexity) {
    if (loopComplexity == ComplexityType::CONSTANT) {
        return recursionComplexity;
    }

    if (recursionComplexity == ComplexityType::CONSTANT) {
        return loopComplexity;
    }

    if ((loopComplexity == ComplexityType::LINEAR &&
         recursionComplexity == ComplexityType::LOGARITHMIC) ||
        (loopComplexity == ComplexityType::LOGARITHMIC &&
         recursionComplexity == ComplexityType::LINEAR)) {
        return ComplexityType::LINEARITHMIC;
    }

    return maxComplexity(loopComplexity, recursionComplexity);
}

std::string ComplexityUtils::getComplexityColor(ComplexityType type) {
    switch (type) {
        case ComplexityType::CONSTANT:
        case ComplexityType::LOGARITHMIC:
            return "\033[32m";
        case ComplexityType::LINEAR:
        case ComplexityType::LINEARITHMIC:
            return "\033[33m";
        case ComplexityType::QUADRATIC:
        case ComplexityType::CUBIC:
            return "\033[31m";
        case ComplexityType::EXPONENTIAL:
        case ComplexityType::FACTORIAL:
            return "\033[35m";
        default:
            return "\033[0m";
    }
}
