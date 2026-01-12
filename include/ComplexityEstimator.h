#ifndef COMPLEXITY_ESTIMATOR_H
#define COMPLEXITY_ESTIMATOR_H

#include "ComplexityTypes.h"
#include "Parser.h"
#include "LoopAnalyzer.h"
#include "RecursionAnalyzer.h"
#include "PatternDetector.h"
#include <string>
#include <vector>

struct ComplexityBreakdown {
    ComplexityType loopComplexity;
    ComplexityType recursionComplexity;
    ComplexityType patternComplexity;
    ComplexityType combinedComplexity;
    std::vector<std::string> explanations;
    int confidence;
};

class ComplexityEstimator {
public:

    ComplexityEstimator();

    ~ComplexityEstimator();

    ComplexityResult estimateWithConfidence(const std::string& code);

    ComplexityBreakdown getDetailedBreakdown();

private:

    ComplexityType combineComplexities(
        ComplexityType loopComplexity,
        ComplexityType recursionComplexity
    );

    int calculateConfidence(
        const std::vector<LoopInfo>& loops,
        const std::vector<RecursionInfo>& recursions,
        const std::vector<AlgorithmPattern>& patterns
    );

    std::string generateExplanation(const ComplexityBreakdown& breakdown);

    Parser parser;
    LoopAnalyzer loopAnalyzer;
    RecursionAnalyzer recursionAnalyzer;
    PatternDetector patternDetector;

    ComplexityBreakdown lastBreakdown;
};

#endif
