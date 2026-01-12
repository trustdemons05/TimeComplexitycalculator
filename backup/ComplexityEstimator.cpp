#include "ComplexityTypes.h"
#include <iostream>
#include <algorithm>

class ComplexityEstimator {
public:
    ComplexityEstimator() {}
    
    ComplexityResult estimateComplexity(ComplexityType loopComplexity, 
                                      ComplexityType recursionComplexity,
                                      const std::vector<LoopInfo>& loops,
                                      const std::vector<RecursionInfo>& recursions) {
        
        ComplexityResult result;
        
        ComplexityType finalComplexity = combineComplexities(loopComplexity, recursionComplexity);
        
        result.type = finalComplexity;
        result.notation = complexityToString(finalComplexity);
        result.explanation = generateExplanation(loops, recursions, finalComplexity);
        result.confidence = calculateConfidence(loops, recursions);
        
        return result;
    }
    
private:
    ComplexityType combineComplexities(ComplexityType loop, ComplexityType recursion) {
        if (loop == ComplexityType::CONSTANT) {
            return recursion;
        }
        
        if (recursion == ComplexityType::CONSTANT) {
            return loop;
        }
        
        int loopRank = getComplexityRank(loop);
        int recursionRank = getComplexityRank(recursion);
        
        return (loopRank >= recursionRank) ? loop : recursion;
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
    
    std::string complexityToString(ComplexityType type) {
        switch (type) {
            case ComplexityType::CONSTANT: return "O(1)";
            case ComplexityType::LOGARITHMIC: return "O(log n)";
            case ComplexityType::LINEAR: return "O(n)";
            case ComplexityType::LINEARITHMIC: return "O(n log n)";
            case ComplexityType::QUADRATIC: return "O(n²)";
            case ComplexityType::CUBIC: return "O(n³)";
            case ComplexityType::EXPONENTIAL: return "O(2^n)";
            case ComplexityType::FACTORIAL: return "O(n!)";
            default: return "O(?)";
        }
    }
    
    std::string generateExplanation(const std::vector<LoopInfo>& loops,
                                  const std::vector<RecursionInfo>& recursions,
                                  ComplexityType finalComplexity) {
        std::string explanation = "Analysis: ";
        
        if (!loops.empty()) {
            explanation += "Found " + std::to_string(loops.size()) + " loop(s). ";
            
            int maxNesting = 0;
            for (const auto& loop : loops) {
                maxNesting = std::max(maxNesting, loop.nestingLevel);
            }
            
            if (maxNesting > 1) {
                explanation += "Maximum nesting level: " + std::to_string(maxNesting) + ". ";
            }
        }
        
        if (!recursions.empty()) {
            explanation += "Found " + std::to_string(recursions.size()) + " recursive function(s). ";
            
            for (const auto& rec : recursions) {
                if (rec.recursiveCalls > 1) {
                    explanation += rec.functionName + " makes " + 
                                 std::to_string(rec.recursiveCalls) + " recursive calls. ";
                }
            }
        }
        
        explanation += "Estimated complexity: " + complexityToString(finalComplexity);
        
        return explanation;
    }
    
    int calculateConfidence(const std::vector<LoopInfo>& loops,
                          const std::vector<RecursionInfo>& recursions) {
        int confidence = 70;
        int totalPatterns = 0;
        int recognizedPatterns = 0;
        
        for (const auto& loop : loops) {
            totalPatterns++;
            
            if (loop.increment == "++" || loop.increment == "--" || loop.increment == "+=") {
                confidence += 10;
                recognizedPatterns++;
            } else if (loop.increment == "*=" || loop.increment == "/=") {
                confidence += 15;
                recognizedPatterns++;
            } else if (loop.increment == "unknown") {
                confidence -= 10;
            }
            
            if (loop.nestingLevel > 1) {
                confidence += 15;
                recognizedPatterns++;
            }
        }
        
        for (const auto& rec : recursions) {
            totalPatterns++;
            
            if (rec.recursiveCalls == 1) {
                confidence += 10;
                recognizedPatterns++;
                
                if (rec.isTailRecursion) {
                    confidence += 5;
                }
            } else if (rec.recursiveCalls == 2) {
                confidence += 15;
                recognizedPatterns++;
            } else if (rec.recursiveCalls > 2) {
                confidence += 10;
                recognizedPatterns++;
            }
        }
        
        if (totalPatterns > 0) {
            double recognitionRatio = (double)recognizedPatterns / totalPatterns;
            if (recognitionRatio < 0.5) {
                confidence -= 10;
            }
        }
        
        if (loops.empty() && recursions.empty()) {
            confidence = 95;
        }
        
        return std::min(std::max(confidence, 30), 95);
    }
};