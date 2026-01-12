#ifndef LOOP_ANALYZER_H
#define LOOP_ANALYZER_H

#include "ComplexityTypes.h"
#include <string>
#include <vector>
#include <utility>

struct FunctionBoundary {
    std::string name;
    size_t start;
    size_t end;

    FunctionBoundary(const std::string& n, size_t s, size_t e)
        : name(n), start(s), end(e) {}
};

class LoopAnalyzer {
public:

    LoopAnalyzer();

    ~LoopAnalyzer();

    std::vector<LoopInfo> analyzeLoops(const std::string& code);

    ComplexityType estimateLoopComplexity();

    const std::vector<LoopInfo>& getDetectedLoops() const;

    const std::vector<FunctionBoundary>& getFunctionBoundaries() const;

    void clear();

    bool hasEarlyTermination(const LoopInfo& loop, const std::string& code);

    bool hasDependentBounds(const LoopInfo& loop, const std::string& code);

    ComplexityType analyzeIncrementPattern(const std::string& increment);

    double estimateIterationFraction(const LoopInfo& loop, const std::string& code);

private:

    void detectFunctionBoundaries(const std::string& code);

    void analyzeForLoops(const std::string& code);

    void analyzeWhileLoops(const std::string& code);

    void analyzeDoWhileLoops(const std::string& code);

    void extractLoopDetails(const std::string& forStatement, LoopInfo& loop);

    int calculateNestingLevel(const std::string& code, size_t position);

    void fixNestingLevelsWithFunctions(const std::string& code);

    int calculateNestingWithinFunction(const std::string& code, int lineNumber);

    const FunctionBoundary* findContainingFunction(size_t position) const;

    int detectBreakStatements(const std::string& loopBody);

    int detectReturnStatements(const std::string& loopBody);

    void analyzeBoundDependencies(std::vector<LoopInfo>& loops, const std::string& code);

    std::string getLoopBody(const std::string& code, size_t loopStart);

    std::vector<LoopInfo> detectedLoops;
    std::vector<FunctionBoundary> functionBounds;
};

#endif
