#include "../include/PatternDetector.h"
#include "../include/CodeUtils.h"
#include <regex>
#include <algorithm>

PatternDetector::PatternDetector() {}

PatternDetector::~PatternDetector() {}

bool PatternDetector::isBinarySearch(const std::string& code) {
    int indicators = 0;

    if (hasHalvingPattern(code)) {
        indicators++;
    }

    if (code.find("mid") != std::string::npos) {
        indicators++;
    }

    std::regex midCompare("(arr|array|data)\\[mid\\]|\\bmid\\b.*[<>=]");
    if (std::regex_search(code, midCompare)) {
        indicators++;
    }

    if ((code.find("left") != std::string::npos && code.find("right") != std::string::npos) ||
        (code.find("low") != std::string::npos && code.find("high") != std::string::npos)) {
        indicators++;
    }

    return indicators >= 3;
}

bool PatternDetector::isMergeSort(const std::string& code) {
    int indicators = 0;

    if (hasMergePattern(code)) {
        indicators++;
    }

    if (hasRecursiveSplit(code)) {
        indicators++;
    }

    if (code.find("merge") != std::string::npos || code.find("Merge") != std::string::npos) {
        indicators++;
    }

    if (hasHalvingPattern(code)) {
        indicators++;
    }

    return indicators >= 3;
}

bool PatternDetector::isQuickSort(const std::string& code) {
    int indicators = 0;

    if (hasPartitionPattern(code)) {
        indicators++;
    }

    if (code.find("pivot") != std::string::npos) {
        indicators++;
    }

    if (code.find("quick") != std::string::npos ||
        code.find("Quick") != std::string::npos ||
        code.find("partition") != std::string::npos) {
        indicators++;
    }

    if (hasRecursiveSplit(code) && hasPartitionPattern(code)) {
        indicators++;
    }

    return indicators >= 3;
}

bool PatternDetector::isDivideAndConquer(const std::string& code) {
    int indicators = 0;

    if (hasRecursiveSplit(code)) {
        indicators++;
    }

    if (hasHalvingPattern(code) || hasPartitionPattern(code)) {
        indicators++;
    }

    if (hasMergePattern(code)) {
        indicators++;
    }

    return indicators >= 2;
}

bool PatternDetector::isDynamicProgramming(const std::string& code) {
    int indicators = 0;

    if (hasMemoization(code)) {
        indicators++;
    }

    if (code.find("dp[") != std::string::npos ||
        code.find("memo[") != std::string::npos ||
        code.find("cache[") != std::string::npos) {
        indicators++;
    }

    std::regex dpPattern("for.*for.*\\[.*\\]\\[.*\\]");
    if (std::regex_search(code, dpPattern)) {
        indicators++;
    }

    if (code.find("dp") != std::string::npos ||
        code.find("DP") != std::string::npos ||
        code.find("dynamic") != std::string::npos) {
        indicators++;
    }

    return indicators >= 2;
}

std::vector<AlgorithmPattern> PatternDetector::detectPatterns(const std::string& code) {
    std::vector<AlgorithmPattern> patterns;

    if (isMergeSort(code)) {
        AlgorithmPattern pattern;
        pattern.patternName = "Merge Sort";
        pattern.expectedComplexity = ComplexityType::LINEARITHMIC;
        pattern.confidence = 80;
        pattern.explanation = "Detected divide-and-conquer with merge operation";
        patterns.push_back(pattern);
    }

    if (isQuickSort(code)) {
        AlgorithmPattern pattern;
        pattern.patternName = "Quick Sort";
        pattern.expectedComplexity = ComplexityType::LINEARITHMIC;
        pattern.confidence = 80;
        pattern.explanation = "Detected partition-based divide-and-conquer pattern";
        patterns.push_back(pattern);
    }

    if (patterns.empty() && isBinarySearch(code)) {
        AlgorithmPattern pattern;
        pattern.patternName = "Binary Search";
        pattern.expectedComplexity = ComplexityType::LOGARITHMIC;
        pattern.confidence = 85;
        pattern.explanation = "Detected halving search space pattern with mid-point calculation";
        patterns.push_back(pattern);
    }

    if (isDivideAndConquer(code) && patterns.empty()) {
        AlgorithmPattern pattern;
        pattern.patternName = "Divide and Conquer";
        pattern.expectedComplexity = ComplexityType::LINEARITHMIC;
        pattern.confidence = 70;
        pattern.explanation = "Detected recursive splitting and combining pattern";
        patterns.push_back(pattern);
    }

    if (isDynamicProgramming(code)) {
        AlgorithmPattern pattern;
        pattern.patternName = "Dynamic Programming";
        pattern.expectedComplexity = ComplexityType::QUADRATIC;
        pattern.confidence = 75;
        pattern.explanation = "Detected memoization or tabulation pattern";
        patterns.push_back(pattern);
    }

    if (hasTwoPointerPattern(code)) {
        AlgorithmPattern pattern;
        pattern.patternName = "Two Pointer";
        pattern.expectedComplexity = ComplexityType::LINEAR;
        pattern.confidence = 70;
        pattern.explanation = "Detected two-pointer traversal pattern";
        patterns.push_back(pattern);
    }

    return patterns;
}

bool PatternDetector::hasHalvingPattern(const std::string& code) {

    std::regex halvingPattern(
        "mid\\s*=\\s*\\(?\\s*(\\w+)\\s*\\+\\s*(\\w+)\\s*\\)?\\s*/\\s*2|"
        "(\\w+)\\s*/\\s*2|"
        "(\\w+)\\s*>>=?\\s*1|"
        "\\bmid\\b"
    );

    return std::regex_search(code, halvingPattern);
}

bool PatternDetector::hasMemoization(const std::string& code) {

    std::regex memoPattern(
        "if\\s*\\(.*\\[.*\\].*!=.*\\)|"
        "if\\s*\\(.*\\[.*\\].*==.*\\)|"
        "(dp|memo|cache)\\[.*\\]\\s*="
    );

    return std::regex_search(code, memoPattern);
}

bool PatternDetector::hasRecursiveSplit(const std::string& code) {

    std::regex splitPattern(
        "(\\w+)\\s*\\(.*,\\s*mid\\s*[,\\)]|"
        "(\\w+)\\s*\\(.*,\\s*(\\w+)\\s*/\\s*2|"
        "(\\w+)\\s*\\(.*left.*\\).*\\+.*(\\w+)\\s*\\(.*right"
    );

    return std::regex_search(code, splitPattern);
}

bool PatternDetector::hasTwoPointerPattern(const std::string& code) {

    int indicators = 0;

    if ((code.find("left") != std::string::npos && code.find("right") != std::string::npos) ||
        (code.find("i") != std::string::npos && code.find("j") != std::string::npos)) {
        indicators++;
    }

    if ((code.find("left++") != std::string::npos || code.find("i++") != std::string::npos) &&
        (code.find("right--") != std::string::npos || code.find("j--") != std::string::npos ||
         code.find("right++") != std::string::npos || code.find("j++") != std::string::npos)) {
        indicators++;
    }

    std::regex whileTwo("while\\s*\\([^)]*<[^)]*&&[^)]*<[^)]*\\)");
    if (std::regex_search(code, whileTwo)) {
        indicators++;
    }

    return indicators >= 2;
}

bool PatternDetector::hasPartitionPattern(const std::string& code) {

    int indicators = 0;

    if (code.find("pivot") != std::string::npos) {
        indicators++;
    }

    std::regex swapPattern("swap\\s*\\(|"
                           "temp\\s*=.*\\[.*\\].*\\[.*\\]\\s*=.*\\[.*\\]\\s*=.*temp");
    if (std::regex_search(code, swapPattern)) {
        indicators++;
    }

    if (code.find("partition") != std::string::npos) {
        indicators++;
    }

    return indicators >= 2;
}

bool PatternDetector::hasMergePattern(const std::string& code) {

    int indicators = 0;

    if (code.find("merge") != std::string::npos || code.find("Merge") != std::string::npos) {
        indicators++;
    }

    std::regex twoIndices("while\\s*\\(.*i.*<.*&&.*j.*<|"
                          "while\\s*\\(.*left.*<.*&&.*right.*<");
    if (std::regex_search(code, twoIndices)) {
        indicators++;
    }

    std::regex copyPattern("(result|temp|arr)\\[.*\\]\\s*=\\s*(left|right|arr)\\[");
    if (std::regex_search(code, copyPattern)) {
        indicators++;
    }

    return indicators >= 2;
}

int PatternDetector::calculateConfidence(int indicators, int maxIndicators) {
    if (maxIndicators == 0) return 0;
    return (indicators * 100) / maxIndicators;
}
