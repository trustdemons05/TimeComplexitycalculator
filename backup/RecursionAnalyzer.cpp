#include "ComplexityTypes.h"
#include <iostream>
#include <regex>
#include <vector>
#include <string>
#include <unordered_set>

class RecursionAnalyzer {
private:
    std::vector<RecursionInfo> detectedRecursions;
    std::unordered_set<std::string> functionNames;
    
public:
    RecursionAnalyzer() {}
    
    std::vector<RecursionInfo> analyzeRecursion(const std::string& code) {
        detectedRecursions.clear();
        functionNames.clear();
        
        extractFunctionNames(code);
        
        findRecursiveCalls(code);
        
        return detectedRecursions;
    }
    
private:
    void extractFunctionNames(const std::string& code) {
        std::regex funcRegex("(int|void|char|float|double|long)\\s+(\\w+)\\s*\\(");
        std::sregex_iterator iter(code.begin(), code.end(), funcRegex);
        std::sregex_iterator end;
        
        for (; iter != end; ++iter) {
            std::string funcName = (*iter)[2].str();
            if (funcName != "if" && funcName != "while" && funcName != "for" && 
                funcName != "return" && funcName != "printf" && funcName != "scanf") {
                functionNames.insert(funcName);
            }
        }
    }
    
    void findRecursiveCalls(const std::string& code) {
        for (const std::string& funcName : functionNames) {
            RecursionInfo recursion;
            recursion.functionName = funcName;
            recursion.recursiveCalls = 0;
            recursion.isTailRecursion = false;
            
            std::string funcDefPattern = "(int|void|char|float|double|long)\\s+" + funcName + "\\s*\\(";
            std::regex funcDefRegex(funcDefPattern);
            std::smatch funcMatch;
            
            if (std::regex_search(code, funcMatch, funcDefRegex)) {
                size_t bracePos = code.find('{', funcMatch.position());
                if (bracePos != std::string::npos) {
                    size_t funcEnd = findMatchingBrace(code, bracePos);
                    
                    if (funcEnd != std::string::npos) {
                        std::string funcBody = code.substr(bracePos + 1, funcEnd - bracePos - 1);
                        
                        std::regex callRegex(funcName + "\\s*\\(");
                        std::sregex_iterator callIter(funcBody.begin(), funcBody.end(), callRegex);
                        std::sregex_iterator callEnd;
                        
                        for (; callIter != callEnd; ++callIter) {
                            recursion.recursiveCalls++;
                        }
                        
                        recursion.isTailRecursion = checkTailRecursion(funcBody, funcName);
                        
                        if (recursion.recursiveCalls > 0) {
                            detectedRecursions.push_back(recursion);
                        }
                    }
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
    
    bool checkTailRecursion(const std::string& funcBody, const std::string& funcName) {
        std::regex returnRegex("return\\s+" + funcName + "\\s*\\(");
        return std::regex_search(funcBody, returnRegex);
    }
    
public:
    ComplexityType estimateRecursionComplexity() {
        if (detectedRecursions.empty()) {
            return ComplexityType::CONSTANT;
        }
        
        ComplexityType maxComplexity = ComplexityType::CONSTANT;
        
        for (const auto& recursion : detectedRecursions) {
            ComplexityType currentComplexity = analyzeRecursionPattern(recursion);
            if (getComplexityRank(currentComplexity) > getComplexityRank(maxComplexity)) {
                maxComplexity = currentComplexity;
            }
        }
        
        return maxComplexity;
    }
    
private:
    ComplexityType analyzeRecursionPattern(const RecursionInfo& recursion) {
        std::string funcName = recursion.functionName;
        
        if ((funcName.find("binary") != std::string::npos || 
             funcName.find("Binary") != std::string::npos ||
             funcName.find("search") != std::string::npos) && 
            recursion.recursiveCalls <= 2) {
            return ComplexityType::LOGARITHMIC;
        }
        
        if ((funcName.find("merge") != std::string::npos || 
             funcName.find("Merge") != std::string::npos ||
             funcName.find("sort") != std::string::npos) && 
            recursion.recursiveCalls == 2) {
            return ComplexityType::LINEARITHMIC;
        }
        
        if ((funcName.find("fib") != std::string::npos || 
             funcName.find("Fib") != std::string::npos ||
             funcName.find("tower") != std::string::npos) && 
            recursion.recursiveCalls == 2) {
            return ComplexityType::EXPONENTIAL;
        }
        
        if (recursion.recursiveCalls == 1) {
            if (recursion.isTailRecursion) {
                return ComplexityType::LINEAR;
            } else {
                return ComplexityType::LINEAR;
            }
        } else if (recursion.recursiveCalls == 2) {
            return ComplexityType::EXPONENTIAL;
        } else if (recursion.recursiveCalls > 2) {
            return ComplexityType::EXPONENTIAL;
        }
        
        return ComplexityType::LINEAR;
    }
    
    int getComplexityRank(ComplexityType type) {
        switch (type) {
            case ComplexityType::CONSTANT: return 0;
            case ComplexityType::LOGARITHMIC: return 1;
            case ComplexityType::LINEAR: return 2;
            case ComplexityType::LINEARITHMIC: return 3;
            case ComplexityType::QUADRATIC: return 4;
            case ComplexityType::CUBIC: return 5;
            case ComplexityType::EXPONENTIAL: return 6;
            case ComplexityType::FACTORIAL: return 7;
            default: return -1;
        }
    }
    
    std::vector<RecursionInfo> getDetectedRecursions() const {
        return detectedRecursions;
    }
};