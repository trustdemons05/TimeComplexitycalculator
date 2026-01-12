#ifndef RECURSION_ANALYZER_H
#define RECURSION_ANALYZER_H

#include "ComplexityTypes.h"
#include <string>
#include <vector>
#include <map>
#include <set>

class RecursionAnalyzer {
public:

    RecursionAnalyzer();

    ~RecursionAnalyzer();

    std::vector<RecursionInfo> analyzeRecursion(const std::string& code);

    ComplexityType estimateRecursionComplexity();

    bool isTailRecursive(const RecursionInfo& info);

    bool isDirectRecursion(const std::string& functionName);

    bool isIndirectRecursion(const std::string& functionName);

    const std::vector<RecursionInfo>& getDetectedRecursion() const;

    const std::map<std::string, std::vector<std::string>>& getCallGraph() const;

    void clear();

private:

    void buildCallGraph(const std::string& code);

    void extractFunctionNames(const std::string& code);

    void detectDirectRecursion(const std::string& code);

    void detectIndirectRecursion();

    void analyzeRecurrenceRelation(RecursionInfo& info, const std::string& code);

    int countRecursiveCalls(const std::string& functionBody, const std::string& functionName);

    bool checkTailRecursion(const std::string& functionBody, const std::string& functionName);

    bool hasHalvingPattern(const std::string& functionBody);

    bool hasDecrementPattern(const std::string& functionBody);

    ComplexityType analyzeRecursionPattern(const RecursionInfo& recursion);

    void detectCycles(
        const std::string& node,
        std::set<std::string>& visited,
        std::set<std::string>& recStack,
        std::vector<std::vector<std::string>>& cycles
    );

    std::string getFunctionBody(const std::string& code, const std::string& functionName);

    std::vector<RecursionInfo> detectedRecursion;
    std::map<std::string, std::vector<std::string>> callGraph;
    std::map<std::string, std::string> functionBodies;
    std::set<std::string> functionNames;
};

#endif
