#include "../include/LoopAnalyzer.h"
#include "../include/CodeUtils.h"
#include "../include/ComplexityUtils.h"
#include <regex>
#include <algorithm>

static const std::regex forRegex("for\\s*\\(");
static const std::regex whileRegex("while\\s*\\(");
static const std::regex doWhileRegex("do\\s*\\{");
static const std::regex funcRegex("(int|void|char|float|double|long)\\s+(\\w+)\\s*\\([^)]*\\)\\s*\\{");

LoopAnalyzer::LoopAnalyzer() {
    detectedLoops.reserve(50);
    functionBounds.reserve(20);
}

LoopAnalyzer::~LoopAnalyzer() {}

std::vector<LoopInfo> LoopAnalyzer::analyzeLoops(const std::string& code) {
    clear();

    detectFunctionBoundaries(code);
    analyzeForLoops(code);
    analyzeWhileLoops(code);
    analyzeDoWhileLoops(code);
    fixNestingLevelsWithFunctions(code);
    analyzeBoundDependencies(detectedLoops, code);

    for (auto& loop : detectedLoops) {
        loop.hasEarlyTermination = hasEarlyTermination(loop, code);
        if (loop.hasEarlyTermination) {
            loop.iterationFraction = estimateIterationFraction(loop, code);
        }
    }

    return detectedLoops;
}

void LoopAnalyzer::clear() {
    detectedLoops.clear();
    functionBounds.clear();
}

void LoopAnalyzer::detectFunctionBoundaries(const std::string& code) {
    std::sregex_iterator iter(code.begin(), code.end(), funcRegex);
    std::sregex_iterator end;

    for (; iter != end; ++iter) {
        std::string funcName = (*iter)[2].str();
        size_t funcStart = iter->position();

        size_t bracePos = code.find('{', funcStart);
        if (bracePos != std::string::npos) {
            size_t funcEnd = CodeUtils::findMatchingBrace(code, bracePos);
            if (funcEnd != std::string::npos) {
                functionBounds.emplace_back(funcName, bracePos, funcEnd);
            }
        }
    }
}

void LoopAnalyzer::analyzeForLoops(const std::string& code) {
    std::sregex_iterator iter(code.begin(), code.end(), forRegex);
    std::sregex_iterator end;

    for (; iter != end; ++iter) {
        size_t forPos = iter->position();

        if (CodeUtils::isInsideStringLiteral(code, forPos) ||
            CodeUtils::isInsideComment(code, forPos)) {
            continue;
        }

        LoopInfo loop;

        size_t parenStart = forPos + 3;
        while (parenStart < code.length() && code[parenStart] != '(') parenStart++;

        if (parenStart < code.length()) {
            size_t parenEnd = CodeUtils::findMatchingParen(code, parenStart);
            if (parenEnd != std::string::npos) {
                std::string forStatement = code.substr(forPos, parenEnd - forPos + 1);
                extractLoopDetails(forStatement, loop);
            }
        }

        loop.nestingLevel = 1;
        loop.startLine = CodeUtils::positionToLine(code, forPos);
        loop.endLine = 0;
        loop.hasEarlyTermination = false;
        loop.hasDependentBounds = false;
        loop.iterationFraction = 1.0;

        detectedLoops.push_back(loop);
    }
}

void LoopAnalyzer::extractLoopDetails(const std::string& forStatement, LoopInfo& loop) {
    std::regex varRegex("(\\w+)\\s*=");
    std::smatch varMatch;
    if (std::regex_search(forStatement, varMatch, varRegex)) {
        loop.variable = varMatch[1].str();
    } else {
        loop.variable = "i";
    }

    if (forStatement.find("++") != std::string::npos) {
        loop.increment = "++";
    } else if (forStatement.find("--") != std::string::npos) {
        loop.increment = "--";
    } else if (forStatement.find("+=") != std::string::npos) {
        std::regex incrRegex("\\+=\\s*(\\d+)");
        std::smatch incrMatch;
        if (std::regex_search(forStatement, incrMatch, incrRegex)) {
            loop.increment = "+= " + incrMatch[1].str();
        } else {
            loop.increment = "+=";
        }
    } else if (forStatement.find("-=") != std::string::npos) {
        std::regex decrRegex("\\-=\\s*(\\d+)");
        std::smatch decrMatch;
        if (std::regex_search(forStatement, decrMatch, decrRegex)) {
            loop.increment = "-= " + decrMatch[1].str();
        } else {
            loop.increment = "-=";
        }
    } else if (forStatement.find("*=") != std::string::npos) {
        std::regex multRegex("\\*=\\s*(\\d+)");
        std::smatch multMatch;
        if (std::regex_search(forStatement, multMatch, multRegex)) {
            loop.increment = "*= " + multMatch[1].str();
        } else {
            loop.increment = "*=";
        }
    } else if (forStatement.find("/=") != std::string::npos) {
        std::regex divRegex("/=\\s*(\\d+)");
        std::smatch divMatch;
        if (std::regex_search(forStatement, divMatch, divRegex)) {
            loop.increment = "/= " + divMatch[1].str();
        } else {
            loop.increment = "/=";
        }
    } else {
        loop.increment = "unknown";
    }
}

void LoopAnalyzer::analyzeWhileLoops(const std::string& code) {
    std::sregex_iterator iter(code.begin(), code.end(), whileRegex);
    std::sregex_iterator end;

    for (; iter != end; ++iter) {
        size_t whilePos = iter->position();

        if (CodeUtils::isInsideStringLiteral(code, whilePos) ||
            CodeUtils::isInsideComment(code, whilePos)) {
            continue;
        }

        LoopInfo loop;
        loop.variable = "unknown";
        loop.increment = "unknown";
        loop.nestingLevel = 1;
        loop.startLine = CodeUtils::positionToLine(code, whilePos);
        loop.endLine = 0;
        loop.hasEarlyTermination = false;
        loop.hasDependentBounds = false;
        loop.iterationFraction = 1.0;

        detectedLoops.push_back(loop);
    }
}

void LoopAnalyzer::analyzeDoWhileLoops(const std::string& code) {
    std::sregex_iterator iter(code.begin(), code.end(), doWhileRegex);
    std::sregex_iterator end;

    for (; iter != end; ++iter) {
        size_t doPos = iter->position();

        if (CodeUtils::isInsideStringLiteral(code, doPos) ||
            CodeUtils::isInsideComment(code, doPos)) {
            continue;
        }

        LoopInfo loop;
        loop.variable = "unknown";
        loop.increment = "unknown";
        loop.nestingLevel = 1;
        loop.startLine = CodeUtils::positionToLine(code, doPos);
        loop.endLine = 0;
        loop.hasEarlyTermination = false;
        loop.hasDependentBounds = false;
        loop.iterationFraction = 1.0;

        detectedLoops.push_back(loop);
    }
}

int LoopAnalyzer::calculateNestingLevel(const std::string& code, size_t position) {
    int nestingLevel = 1;
    int braceDepth = 0;

    for (size_t i = position; i > 0; --i) {
        char c = code[i];

        if (c == '}') {
            braceDepth++;
        } else if (c == '{') {
            braceDepth--;

            if (braceDepth >= 0) {
                size_t searchStart = (i > 100) ? i - 100 : 0;
                std::string context = code.substr(searchStart, i - searchStart);

                if (context.find("for") != std::string::npos ||
                    context.find("while") != std::string::npos ||
                    context.find("do") != std::string::npos) {
                    nestingLevel++;
                }
            }
        }
    }

    return nestingLevel;
}

void LoopAnalyzer::fixNestingLevelsWithFunctions(const std::string& code) {
    std::sort(detectedLoops.begin(), detectedLoops.end(),
              [](const LoopInfo& a, const LoopInfo& b) {
                  return a.startLine < b.startLine;
              });

    for (auto& loop : detectedLoops) {
        loop.nestingLevel = calculateNestingWithinFunction(code, loop.startLine);
    }
}

int LoopAnalyzer::calculateNestingWithinFunction(const std::string& code, int lineNumber) {
    size_t loopPosition = CodeUtils::lineToPosition(code, lineNumber);
    if (loopPosition == std::string::npos) {
        return 1;
    }

    const FunctionBoundary* func = findContainingFunction(loopPosition);

    size_t functionStart = 0;
    size_t functionEnd = code.length();

    if (func != nullptr) {
        functionStart = func->start;
        functionEnd = func->end;
    }

    int nestingLevel = 1;

    for (size_t i = functionStart; i < loopPosition && i < functionEnd; ++i) {
        if (i + 3 < code.length() && code.substr(i, 3) == "for") {
            size_t bracePos = code.find('{', i);
            if (bracePos != std::string::npos && bracePos < loopPosition && bracePos < functionEnd) {
                size_t endBrace = CodeUtils::findMatchingBrace(code, bracePos);
                if (endBrace != std::string::npos && endBrace > loopPosition) {
                    nestingLevel++;
                }
            }
        } else if (i + 5 < code.length() && code.substr(i, 5) == "while") {
            size_t bracePos = code.find('{', i);
            if (bracePos != std::string::npos && bracePos < loopPosition && bracePos < functionEnd) {
                size_t endBrace = CodeUtils::findMatchingBrace(code, bracePos);
                if (endBrace != std::string::npos && endBrace > loopPosition) {
                    nestingLevel++;
                }
            }
        }
    }

    return std::max(1, nestingLevel);
}

const FunctionBoundary* LoopAnalyzer::findContainingFunction(size_t position) const {
    for (const auto& func : functionBounds) {
        if (position >= func.start && position <= func.end) {
            return &func;
        }
    }
    return nullptr;
}

ComplexityType LoopAnalyzer::estimateLoopComplexity() {
    if (detectedLoops.empty()) {
        return ComplexityType::CONSTANT;
    }

    int maxNesting = 0;
    bool hasLogIncrement = false;
    bool hasLinearIncrement = false;
    bool hasDependentBounds = false;

    for (const auto& loop : detectedLoops) {

        int effectiveNesting = loop.nestingLevel;
        if (loop.hasEarlyTermination && loop.iterationFraction < 0.75) {

        }

        maxNesting = std::max(maxNesting, effectiveNesting);

        if (loop.hasDependentBounds) {
            hasDependentBounds = true;
        }

        ComplexityType incrementComplexity = analyzeIncrementPattern(loop.increment);
        if (incrementComplexity == ComplexityType::LOGARITHMIC) {
            hasLogIncrement = true;
        } else {
            hasLinearIncrement = true;
        }
    }

    if (maxNesting >= 2 && hasDependentBounds) {

    }

    if (maxNesting >= 2 && hasLogIncrement && hasLinearIncrement) {
        return ComplexityType::LINEARITHMIC;
    }

    if (hasLogIncrement && maxNesting == 1) {
        return ComplexityType::LOGARITHMIC;
    }

    switch (maxNesting) {
        case 1: return ComplexityType::LINEAR;
        case 2: return ComplexityType::QUADRATIC;
        case 3: return ComplexityType::CUBIC;
        default: return ComplexityType::EXPONENTIAL;
    }
}

const std::vector<LoopInfo>& LoopAnalyzer::getDetectedLoops() const {
    return detectedLoops;
}

const std::vector<FunctionBoundary>& LoopAnalyzer::getFunctionBoundaries() const {
    return functionBounds;
}

bool LoopAnalyzer::hasEarlyTermination(const LoopInfo& loop, const std::string& code) {
    std::string loopBody = getLoopBody(code, CodeUtils::lineToPosition(code, loop.startLine));

    if (loopBody.empty()) {
        return false;
    }

    int breakCount = detectBreakStatements(loopBody);
    int returnCount = detectReturnStatements(loopBody);

    return (breakCount > 0 || returnCount > 0);
}

bool LoopAnalyzer::hasDependentBounds(const LoopInfo& loop, const std::string& ) {

    return loop.hasDependentBounds;
}

ComplexityType LoopAnalyzer::analyzeIncrementPattern(const std::string& increment) {

    if (increment.find("*=") != std::string::npos ||
        increment.find("/=") != std::string::npos) {
        return ComplexityType::LOGARITHMIC;
    }

    if (increment.find("++") != std::string::npos ||
        increment.find("--") != std::string::npos ||
        increment.find("+=") != std::string::npos ||
        increment.find("-=") != std::string::npos) {
        return ComplexityType::LINEAR;
    }

    return ComplexityType::LINEAR;
}

double LoopAnalyzer::estimateIterationFraction(const LoopInfo& loop, const std::string& code) {
    std::string loopBody = getLoopBody(code, CodeUtils::lineToPosition(code, loop.startLine));

    if (loopBody.empty()) {
        return 1.0;
    }

    int breakCount = detectBreakStatements(loopBody);
    int returnCount = detectReturnStatements(loopBody);

    if (breakCount > 0 || returnCount > 0) {
        return 0.5;
    }

    return 1.0;
}

int LoopAnalyzer::detectBreakStatements(const std::string& loopBody) {
    int count = 0;
    std::regex breakPattern("\\bbreak\\s*;");
    std::sregex_iterator iter(loopBody.begin(), loopBody.end(), breakPattern);
    std::sregex_iterator end;

    for (; iter != end; ++iter) {
        count++;
    }

    return count;
}

int LoopAnalyzer::detectReturnStatements(const std::string& loopBody) {
    int count = 0;
    std::regex returnPattern("\\breturn\\b");
    std::sregex_iterator iter(loopBody.begin(), loopBody.end(), returnPattern);
    std::sregex_iterator end;

    for (; iter != end; ++iter) {
        count++;
    }

    return count;
}

void LoopAnalyzer::analyzeBoundDependencies(std::vector<LoopInfo>& loops, const std::string& code) {

    for (auto& loop : loops) {
        size_t loopPos = CodeUtils::lineToPosition(code, loop.startLine);
        if (loopPos == std::string::npos) continue;

        size_t forPos = code.find("for", loopPos);
        if (forPos == std::string::npos || forPos > loopPos + 100) continue;

        size_t parenStart = code.find('(', forPos);
        if (parenStart == std::string::npos) continue;

        size_t parenEnd = CodeUtils::findMatchingParen(code, parenStart);
        if (parenEnd == std::string::npos) continue;

        std::string forStatement = code.substr(parenStart + 1, parenEnd - parenStart - 1);

        std::regex dependentPattern("(\\w+)\\s*=\\s*(\\w+)\\s*;");
        std::smatch match;

        if (std::regex_search(forStatement, match, dependentPattern)) {
            std::string initVar = match[1].str();
            std::string dependsOn = match[2].str();

            for (const auto& otherLoop : loops) {
                if (otherLoop.variable == dependsOn && otherLoop.startLine < loop.startLine) {
                    loop.hasDependentBounds = true;
                    break;
                }
            }
        }
    }
}

std::string LoopAnalyzer::getLoopBody(const std::string& code, size_t loopStart) {
    if (loopStart == std::string::npos) {
        return "";
    }

    size_t bracePos = code.find('{', loopStart);
    if (bracePos == std::string::npos) {
        return "";
    }

    size_t endBrace = CodeUtils::findMatchingBrace(code, bracePos);
    if (endBrace == std::string::npos) {
        return "";
    }

    return code.substr(bracePos + 1, endBrace - bracePos - 1);
}
