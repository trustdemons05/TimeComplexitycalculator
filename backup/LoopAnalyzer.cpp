#include "ComplexityTypes.h"
#include <iostream>
#include <regex>
#include <vector>
#include <string>
#include <algorithm>

class LoopAnalyzer {
private:
    std::vector<LoopInfo> detectedLoops;
    std::vector<std::pair<std::string, std::pair<size_t, size_t>>> functionBounds;
    
public:
    LoopAnalyzer() {}
    
    std::vector<LoopInfo> analyzeLoops(const std::string& code) {
        detectedLoops.clear();
        functionBounds.clear();
        
        detectFunctionBoundaries(code);
        
        analyzeForLoops(code);
        
        analyzeWhileLoops(code);
        
        fixNestingLevelsWithFunctions(code);
        
        return detectedLoops;
    }
    
private:
    void analyzeForLoops(const std::string& code) {
        std::regex forRegex("for\\s*\\([^)]+\\)");
        std::sregex_iterator iter(code.begin(), code.end(), forRegex);
        std::sregex_iterator end;
        
        for (; iter != end; ++iter) {
            LoopInfo loop;
            std::string forStatement = iter->str();
            
            extractLoopDetails(forStatement, loop);
            loop.nestingLevel = calculateNestingLevel(code, iter->position());
            loop.startLine = std::count(code.begin(), code.begin() + iter->position(), '\n') + 1;
            
            detectedLoops.push_back(loop);
        }
    }
    
    void extractLoopDetails(const std::string& forStatement, LoopInfo& loop) {
        std::regex varRegex("(\\w+)\\s*=");
        std::smatch varMatch;
        if (std::regex_search(forStatement, varMatch, varRegex)) {
            loop.variable = varMatch[1].str();
        } else {
            loop.variable = "i";
        }
        
        if (forStatement.find("++") != std::string::npos || 
            forStatement.find("+=") != std::string::npos) {
            loop.increment = "++";
        } else if (forStatement.find("*=") != std::string::npos) {
            loop.increment = "*=";
        } else if (forStatement.find("/=") != std::string::npos) {
            loop.increment = "/=";
        } else if (forStatement.find("--") != std::string::npos) {
            loop.increment = "--";
        } else {
            loop.increment = "unknown";
        }
    }
    
    void analyzeWhileLoops(const std::string& code) {
        std::regex whileRegex("while\\s*\\(");
        std::sregex_iterator iter(code.begin(), code.end(), whileRegex);
        std::sregex_iterator end;
        
        for (; iter != end; ++iter) {
            LoopInfo loop;
            loop.variable = "unknown";
            loop.increment = "unknown";
            loop.nestingLevel = calculateNestingLevel(code, iter->position());
            loop.startLine = std::count(code.begin(), code.begin() + iter->position(), '\n') + 1;
            
            detectedLoops.push_back(loop);
        }
    }
    
    int calculateNestingLevel(const std::string& code, size_t position) {
        int nestingLevel = 1;
        int braceDepth = 0;
        
        for (int i = (int)position - 1; i >= 0; i--) {
            char c = code[i];
            
            if (c == '}') {
                braceDepth++;
            } else if (c == '{') {
                braceDepth--;
                
                if (braceDepth >= 0) {
                    for (int j = i - 1; j >= 0 && j > i - 50; j--) {
                        if (j + 3 < (int)code.length() && code.substr(j, 3) == "for") {
                            size_t parenPos = code.find(')', j);
                            if (parenPos != std::string::npos && parenPos < (size_t)i) {
                                nestingLevel++;
                                break;
                            }
                        } else if (j + 5 < (int)code.length() && code.substr(j, 5) == "while") {
                            size_t parenPos = code.find(')', j);
                            if (parenPos != std::string::npos && parenPos < (size_t)i) {
                                nestingLevel++;
                                break;
                            }
                        }
                    }
                }
            }
        }
        
        return nestingLevel;
    }
    
public:
    ComplexityType estimateLoopComplexity() {
        if (detectedLoops.empty()) {
            return ComplexityType::CONSTANT;
        }
        
        int maxNesting = 0;
        bool hasLogIncrement = false;
        bool hasLinearithmic = false;
        
        for (const auto& loop : detectedLoops) {
            maxNesting = std::max(maxNesting, loop.nestingLevel);
            
            if (loop.increment == "*=" || loop.increment == "/=") {
                hasLogIncrement = true;
            }
        }
        
        if (maxNesting >= 2 && hasLogIncrement) {
            hasLinearithmic = true;
        }
        
        if (hasLinearithmic) {
            return ComplexityType::LINEARITHMIC;
        } else if (hasLogIncrement && maxNesting == 1) {
            return ComplexityType::LOGARITHMIC;
        } else {
            switch (maxNesting) {
                case 1: return ComplexityType::LINEAR;
                case 2: return ComplexityType::QUADRATIC;
                case 3: return ComplexityType::CUBIC;
                default: return ComplexityType::EXPONENTIAL;
            }
        }
    }
    
    std::vector<LoopInfo> getDetectedLoops() const {
        return detectedLoops;
    }

private:
    void detectFunctionBoundaries(const std::string& code) {
        std::regex funcRegex("(int|void|char|float|double|long)\\s+(\\w+)\\s*\\([^)]*\\)\\s*\\{");
        std::sregex_iterator iter(code.begin(), code.end(), funcRegex);
        std::sregex_iterator end;
        
        for (; iter != end; ++iter) {
            std::string funcName = (*iter)[2].str();
            size_t funcStart = iter->position();
            
            size_t bracePos = code.find('{', funcStart);
            if (bracePos != std::string::npos) {
                size_t funcEnd = findMatchingBrace(code, bracePos);
                if (funcEnd != std::string::npos) {
                    functionBounds.push_back({funcName, {bracePos, funcEnd}});
                }
            }
        }
    }
    
    size_t findMatchingBrace(const std::string& code, size_t start) {
        int braceCount = 1;
        for (size_t i = start + 1; i < code.length(); ++i) {
            if (code[i] == '{') braceCount++;
            else if (code[i] == '}') braceCount--;
            
            if (braceCount == 0) return i;
        }
        return std::string::npos;
    }
    
    void fixNestingLevelsWithFunctions(const std::string& code) {
        std::sort(detectedLoops.begin(), detectedLoops.end(), 
                  [](const LoopInfo& a, const LoopInfo& b) {
                      return a.startLine < b.startLine;
                  });
        
        for (auto& loop : detectedLoops) {
            loop.nestingLevel = calculateNestingWithinFunction(code, loop.startLine);
        }
    }
    
    int calculateNestingWithinFunction(const std::string& code, int lineNumber) {
        size_t loopPosition = 0;
        int currentLine = 1;
        
        for (size_t i = 0; i < code.length(); ++i) {
            if (currentLine == lineNumber) {
                loopPosition = i;
                break;
            }
            if (code[i] == '\n') {
                currentLine++;
            }
        }
        
        std::string containingFunction = "";
        size_t functionStart = 0;
        size_t functionEnd = code.length();
        
        for (const auto& func : functionBounds) {
            if (loopPosition >= func.second.first && loopPosition <= func.second.second) {
                containingFunction = func.first;
                functionStart = func.second.first;
                functionEnd = func.second.second;
                break;
            }
        }
        
        int nestingLevel = 1;
        int braceDepth = 0;
        
        for (size_t i = functionStart; i < loopPosition && i < functionEnd; ++i) {
            if (code[i] == '{') {
                braceDepth++;
            } else if (code[i] == '}') {
                braceDepth--;
            }
            
            if (i + 3 < code.length() && code.substr(i, 3) == "for") {
                size_t bracePos = code.find('{', i);
                if (bracePos != std::string::npos && bracePos < loopPosition && bracePos < functionEnd) {
                    nestingLevel++;
                }
            } else if (i + 5 < code.length() && code.substr(i, 5) == "while") {
                size_t bracePos = code.find('{', i);
                if (bracePos != std::string::npos && bracePos < loopPosition && bracePos < functionEnd) {
                    nestingLevel++;
                }
            }
        }
        
        return std::max(1, nestingLevel);
    }
};