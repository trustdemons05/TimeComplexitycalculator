#include "../include/RecursionAnalyzer.h"
#include "../include/CodeUtils.h"
#include "../include/ComplexityUtils.h"
#include <regex>
#include <algorithm>
#include <iostream>

static const std::regex funcRegex("(int|void|char|float|double|long|bool|auto|std::\\w+)\\s+(\\w+)\\s*\\([^)]*\\)\\s*\\{");
static const std::regex callRegex("(\\w+)\\s*\\(");

RecursionAnalyzer::RecursionAnalyzer() {
    detectedRecursion.reserve(20);
}

RecursionAnalyzer::~RecursionAnalyzer() {}

std::vector<RecursionInfo> RecursionAnalyzer::analyzeRecursion(const std::string& code) {
    clear();

    extractFunctionNames(code);

    buildCallGraph(code);

    detectDirectRecursion(code);

    detectIndirectRecursion();

    return detectedRecursion;
}

void RecursionAnalyzer::extractFunctionNames(const std::string& code) {
    std::sregex_iterator iter(code.begin(), code.end(), funcRegex);
    std::sregex_iterator end;

    static const std::set<std::string> keywords = {
        "if", "while", "for", "switch", "return", "printf", "scanf",
        "sizeof", "typedef", "struct", "union", "enum", "case", "break",
        "continue", "goto", "do", "else", "main"
    };

    for (; iter != end; ++iter) {
        std::string funcName = (*iter)[2].str();
        size_t pos = iter->position();

        if (keywords.find(funcName) == keywords.end() &&
            !CodeUtils::isInsideStringLiteral(code, pos) &&
            !CodeUtils::isInsideComment(code, pos)) {
            functionNames.insert(funcName);

            size_t bracePos = code.find('{', pos);
            if (bracePos != std::string::npos) {
                size_t endBrace = CodeUtils::findMatchingBrace(code, bracePos);
                if (endBrace != std::string::npos) {
                    functionBodies[funcName] = code.substr(bracePos + 1, endBrace - bracePos - 1);
                }
            }
        }
    }
}

void RecursionAnalyzer::buildCallGraph(const std::string& ) {

    for (const auto& funcName : functionNames) {
        std::vector<std::string> calledFunctions;

        if (functionBodies.find(funcName) != functionBodies.end()) {
            const std::string& body = functionBodies[funcName];

            std::sregex_iterator iter(body.begin(), body.end(), callRegex);
            std::sregex_iterator end;

            for (; iter != end; ++iter) {
                std::string calledFunc = (*iter)[1].str();

                if (functionNames.find(calledFunc) != functionNames.end()) {
                    calledFunctions.push_back(calledFunc);
                }
            }
        }

        callGraph[funcName] = calledFunctions;
    }
}

void RecursionAnalyzer::detectDirectRecursion(const std::string& code) {
    for (const auto& funcName : functionNames) {
        if (functionBodies.find(funcName) == functionBodies.end()) {
            continue;
        }

        const std::string& body = functionBodies[funcName];

        int recursiveCallCount = countRecursiveCalls(body, funcName);

        if (recursiveCallCount > 0) {
            RecursionInfo info;
            info.functionName = funcName;
            info.recursiveCalls = recursiveCallCount;
            info.isTailRecursion = checkTailRecursion(body, funcName);
            info.isDirectRecursion = true;

            analyzeRecurrenceRelation(info, code);

            detectedRecursion.push_back(info);
        }
    }
}

void RecursionAnalyzer::detectIndirectRecursion() {

    std::set<std::string> visited;
    std::set<std::string> recStack;
    std::vector<std::vector<std::string>> cycles;

    for (const auto& funcName : functionNames) {
        if (visited.find(funcName) == visited.end()) {
            detectCycles(funcName, visited, recStack, cycles);
        }
    }

    for (const auto& cycle : cycles) {
        if (cycle.size() > 1) {
            for (const auto& funcName : cycle) {

                bool alreadyDetected = false;
                for (const auto& rec : detectedRecursion) {
                    if (rec.functionName == funcName) {
                        alreadyDetected = true;
                        break;
                    }
                }

                if (!alreadyDetected) {
                    RecursionInfo info;
                    info.functionName = funcName;
                    info.recursiveCalls = 1;
                    info.isTailRecursion = false;
                    info.isDirectRecursion = false;
                    info.recurrenceRelation = "Indirect recursion (mutual recursion)";

                    detectedRecursion.push_back(info);
                }
            }
        }
    }
}

void RecursionAnalyzer::detectCycles(
    const std::string& node,
    std::set<std::string>& visited,
    std::set<std::string>& recStack,
    std::vector<std::vector<std::string>>& cycles
) {
    visited.insert(node);
    recStack.insert(node);

    if (callGraph.find(node) != callGraph.end()) {
        for (const auto& neighbor : callGraph[node]) {
            if (visited.find(neighbor) == visited.end()) {
                detectCycles(neighbor, visited, recStack, cycles);
            } else if (recStack.find(neighbor) != recStack.end()) {

                std::vector<std::string> cycle;
                cycle.push_back(neighbor);
                cycle.push_back(node);
                cycles.push_back(cycle);
            }
        }
    }

    recStack.erase(node);
}

int RecursionAnalyzer::countRecursiveCalls(const std::string& functionBody, const std::string& functionName) {
    int count = 0;
    std::string pattern = functionName + "\\s*\\(";
    std::regex callPattern(pattern);

    std::sregex_iterator iter(functionBody.begin(), functionBody.end(), callPattern);
    std::sregex_iterator end;

    for (; iter != end; ++iter) {
        count++;
    }

    return count;
}

bool RecursionAnalyzer::checkTailRecursion(const std::string& functionBody, const std::string& functionName) {

    std::string pattern = "return\\s+" + functionName + "\\s*\\([^)]*\\)\\s*;";
    std::regex tailPattern(pattern);

    std::smatch match;
    if (std::regex_search(functionBody, match, tailPattern)) {

        std::string returnStatement = match.str();

        if (returnStatement.find('+') != std::string::npos ||
            returnStatement.find('-') != std::string::npos ||
            returnStatement.find('*') != std::string::npos ||
            returnStatement.find('/') != std::string::npos) {
            return false;
        }
        return true;
    }

    return false;
}

bool RecursionAnalyzer::hasHalvingPattern(const std::string& functionBody) {

    std::regex halvingPattern("(\\w+)\\s*/\\s*2|"
                              "(\\w+)\\s*>>\\s*1|"
                              "mid\\s*=|"
                              "(\\w+)\\s*\\+\\s*\\(\\s*(\\w+)\\s*-\\s*(\\w+)\\s*\\)\\s*/\\s*2");

    return std::regex_search(functionBody, halvingPattern);
}

bool RecursionAnalyzer::hasDecrementPattern(const std::string& functionBody) {

    std::regex decrementPattern("(\\w+)\\s*-\\s*1|"
                                "(\\w+)\\s*--|"
                                "--\\s*(\\w+)");

    return std::regex_search(functionBody, decrementPattern);
}

void RecursionAnalyzer::analyzeRecurrenceRelation(RecursionInfo& info, const std::string& ) {
    const std::string& body = functionBodies[info.functionName];

    if (info.recursiveCalls == 1) {
        if (hasHalvingPattern(body)) {
            info.recurrenceRelation = "T(n) = T(n/2) + O(1)";
        } else if (hasDecrementPattern(body)) {
            info.recurrenceRelation = "T(n) = T(n-1) + O(1)";
        } else {
            info.recurrenceRelation = "T(n) = T(f(n)) + O(1)";
        }
    } else if (info.recursiveCalls == 2) {
        if (hasHalvingPattern(body)) {
            info.recurrenceRelation = "T(n) = 2T(n/2) + O(n)";
        } else {
            info.recurrenceRelation = "T(n) = 2T(n-1) + O(1)";
        }
    } else if (info.recursiveCalls > 2) {
        info.recurrenceRelation = "T(n) = " + std::to_string(info.recursiveCalls) + "T(n-1) + O(1)";
    }
}

ComplexityType RecursionAnalyzer::analyzeRecursionPattern(const RecursionInfo& recursion) {

    if (recursion.isTailRecursion) {
        return ComplexityType::LINEAR;
    }

    if (recursion.recurrenceRelation.find("T(n/2)") != std::string::npos) {
        if (recursion.recursiveCalls == 1) {
            return ComplexityType::LOGARITHMIC;
        } else if (recursion.recursiveCalls == 2) {
            return ComplexityType::LINEARITHMIC;
        }
    }

    if (recursion.recurrenceRelation.find("T(n-1)") != std::string::npos) {
        if (recursion.recursiveCalls == 1) {
            return ComplexityType::LINEAR;
        } else if (recursion.recursiveCalls == 2) {
            return ComplexityType::EXPONENTIAL;
        } else if (recursion.recursiveCalls > 2) {
            return ComplexityType::EXPONENTIAL;
        }
    }

    if (recursion.recursiveCalls == 1) {
        return ComplexityType::LINEAR;
    } else if (recursion.recursiveCalls >= 2) {
        return ComplexityType::EXPONENTIAL;
    }

    return ComplexityType::LINEAR;
}

ComplexityType RecursionAnalyzer::estimateRecursionComplexity() {
    if (detectedRecursion.empty()) {
        return ComplexityType::CONSTANT;
    }

    ComplexityType maxComplexity = ComplexityType::CONSTANT;

    for (const auto& recursion : detectedRecursion) {
        ComplexityType currentComplexity = analyzeRecursionPattern(recursion);
        maxComplexity = ComplexityUtils::maxComplexity(maxComplexity, currentComplexity);
    }

    return maxComplexity;
}

bool RecursionAnalyzer::isTailRecursive(const RecursionInfo& info) {
    return info.isTailRecursion;
}

bool RecursionAnalyzer::isDirectRecursion(const std::string& functionName) {
    for (const auto& rec : detectedRecursion) {
        if (rec.functionName == functionName && rec.isDirectRecursion) {
            return true;
        }
    }
    return false;
}

bool RecursionAnalyzer::isIndirectRecursion(const std::string& functionName) {
    for (const auto& rec : detectedRecursion) {
        if (rec.functionName == functionName && !rec.isDirectRecursion) {
            return true;
        }
    }
    return false;
}

const std::vector<RecursionInfo>& RecursionAnalyzer::getDetectedRecursion() const {
    return detectedRecursion;
}

const std::map<std::string, std::vector<std::string>>& RecursionAnalyzer::getCallGraph() const {
    return callGraph;
}

std::string RecursionAnalyzer::getFunctionBody(const std::string& , const std::string& functionName) {
    if (functionBodies.find(functionName) != functionBodies.end()) {
        return functionBodies[functionName];
    }
    return "";
}

void RecursionAnalyzer::clear() {
    detectedRecursion.clear();
    callGraph.clear();
    functionBodies.clear();
    functionNames.clear();
}
