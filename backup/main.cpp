#include "ComplexityTypes.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <regex>
#include <vector>
#include <unordered_set>
#include <algorithm>

#include "Parser.cpp"
#include "LoopAnalyzer.cpp"
#include "RecursionAnalyzer.cpp"
#include "ComplexityEstimator.cpp"

int main(int argc, char* argv[]) {
    std::cout << "=== C Code Complexity Analyzer v4.0 ===" << std::endl;
    std::cout << "Full-featured: Function-aware analysis + Advanced pattern recognition" << std::endl;
    
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <filename.c>" << std::endl;
        std::cout << "Or enter code directly (type 'direct' as argument)" << std::endl;
        return 1;
    }
    
    std::string input = argv[1];
    std::string sourceCode;
    
    Parser parser;
    LoopAnalyzer loopAnalyzer;
    RecursionAnalyzer recursionAnalyzer;
    ComplexityEstimator estimator;
    
    if (input == "direct") {
        std::cout << "Enter your C code (end with 'END' on a new line):" << std::endl;
        std::string line;
        while (std::getline(std::cin, line) && line != "END") {
            sourceCode += line + "\n";
        }
        parser.loadFromString(sourceCode);
    } else {
        if (!parser.loadFile(input)) {
            return 1;
        }
        sourceCode = parser.getSourceCode();
    }
    
    std::cout << "\n=== Analysis Results ===" << std::endl;
    
    auto tokens = parser.tokenize();
    std::cout << "Tokens found: " << tokens.size() << std::endl;
    
    auto loops = loopAnalyzer.analyzeLoops(sourceCode);
    ComplexityType loopComplexity = loopAnalyzer.estimateLoopComplexity();
    std::cout << "Loops detected: " << loops.size() << std::endl;
    
    if (!loops.empty()) {
        std::cout << "Loop details:" << std::endl;
        for (size_t i = 0; i < loops.size(); ++i) {
            const auto& loop = loops[i];
            std::cout << "  Loop " << (i+1) << ": Variable=" << loop.variable 
                      << ", Increment=" << loop.increment 
                      << ", Nesting=" << loop.nestingLevel 
                      << ", Line=" << loop.startLine << std::endl;
        }
    }
    
    auto recursions = recursionAnalyzer.analyzeRecursion(sourceCode);
    ComplexityType recursionComplexity = recursionAnalyzer.estimateRecursionComplexity();
    std::cout << "Recursive functions: " << recursions.size() << std::endl;
    
    if (!recursions.empty()) {
        std::cout << "Recursion details:" << std::endl;
        for (size_t i = 0; i < recursions.size(); ++i) {
            const auto& rec = recursions[i];
            std::cout << "  Function " << (i+1) << ": " << rec.functionName 
                      << ", Calls=" << rec.recursiveCalls 
                      << ", Tail=" << (rec.isTailRecursion ? "Yes" : "No") << std::endl;
        }
    }
    
    ComplexityResult result = estimator.estimateComplexity(
        loopComplexity, recursionComplexity, loops, recursions);
    
    std::cout << "\n=== Final Result ===" << std::endl;
    std::cout << "Time Complexity: " << result.notation << std::endl;
    std::cout << "Confidence: " << result.confidence << "%" << std::endl;
    std::cout << "Explanation: " << result.explanation << std::endl;
    
    return 0;
}