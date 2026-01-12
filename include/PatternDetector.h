#ifndef PATTERN_DETECTOR_H
#define PATTERN_DETECTOR_H

#include "ComplexityTypes.h"
#include <string>
#include <vector>

class PatternDetector {
public:

    PatternDetector();

    ~PatternDetector();

    bool isBinarySearch(const std::string& code);

    bool isMergeSort(const std::string& code);

    bool isQuickSort(const std::string& code);

    bool isDivideAndConquer(const std::string& code);

    bool isDynamicProgramming(const std::string& code);

    std::vector<AlgorithmPattern> detectPatterns(const std::string& code);

private:

    bool hasHalvingPattern(const std::string& code);

    bool hasMemoization(const std::string& code);

    bool hasRecursiveSplit(const std::string& code);

    bool hasTwoPointerPattern(const std::string& code);

    bool hasPartitionPattern(const std::string& code);

    bool hasMergePattern(const std::string& code);

    int calculateConfidence(int indicators, int maxIndicators);
};

#endif
