#include "../include/ComplexityEstimator.h"
#include "../include/ComplexityUtils.h"
#include "../include/Exceptions.h"
#include <sstream>
#include <iostream>

ComplexityEstimator::ComplexityEstimator() {}

ComplexityEstimator::~ComplexityEstimator() {}

ComplexityResult ComplexityEstimator::estimateWithConfidence(const std::string& code) {
    ComplexityResult result;

    try {
        if (code.empty()) {
            throw InvalidInputException("Code cannot be empty", "code");
        }

        parser.loadFromString(code);

        if (!parser.isValid()) {
            result.type = ComplexityType::UNKNOWN;
            result.notation = "O(?)";
            result.explanation = "Invalid or empty code";
            result.confidence = 0;
            return result;
        }

        auto loops = loopAnalyzer.analyzeLoops(code);
        ComplexityType loopComplexity = loopAnalyzer.estimateLoopComplexity();

        auto recursions = recursionAnalyzer.analyzeRecursion(code);
        ComplexityType recursionComplexity = recursionAnalyzer.estimateRecursionComplexity();

    auto patterns = patternDetector.detectPatterns(code);
    ComplexityType patternComplexity = ComplexityType::UNKNOWN;

    if (!patterns.empty()) {
        patternComplexity = patterns[0].expectedComplexity;
    }

    lastBreakdown.loopComplexity = loopComplexity;
    lastBreakdown.recursionComplexity = recursionComplexity;
    lastBreakdown.patternComplexity = patternComplexity;

    ComplexityType combined = combineComplexities(loopComplexity, recursionComplexity);

    if (patternComplexity != ComplexityType::UNKNOWN) {

        combined = ComplexityUtils::maxComplexity(combined, patternComplexity);
    }

    lastBreakdown.combinedComplexity = combined;
    lastBreakdown.confidence = calculateConfidence(loops, recursions, patterns);

    lastBreakdown.explanations.clear();

    if (!loops.empty()) {
        std::ostringstream oss;
        oss << "Found " << loops.size() << " loop(s)";
        if (loops.size() > 0) {
            int maxNesting = 0;
            for (const auto& loop : loops) {
                maxNesting = std::max(maxNesting, loop.nestingLevel);
            }
            oss << " with max nesting level " << maxNesting;
        }
        lastBreakdown.explanations.push_back(oss.str());
    }

    if (!recursions.empty()) {
        std::ostringstream oss;
        oss << "Found " << recursions.size() << " recursive function(s)";
        lastBreakdown.explanations.push_back(oss.str());
    }

    if (!patterns.empty()) {
        std::ostringstream oss;
        oss << "Detected pattern: " << patterns[0].patternName;
        lastBreakdown.explanations.push_back(oss.str());
    }

        result.type = combined;
        result.notation = ComplexityUtils::complexityToString(combined);
        result.explanation = generateExplanation(lastBreakdown);
        result.confidence = lastBreakdown.confidence;

        return result;
    }
    catch (const InvalidInputException& e) {
        std::cerr << "Input validation error: " << e.getFullMessage() << std::endl;
        result.type = ComplexityType::UNKNOWN;
        result.notation = "O(?)";
        result.explanation = "Error: " + std::string(e.what());
        result.confidence = 0;
        return result;
    }
    catch (const AnalysisException& e) {
        std::cerr << "Analysis error: " << e.getFullMessage() << std::endl;
        result.type = ComplexityType::UNKNOWN;
        result.notation = "O(?)";
        result.explanation = "Analysis failed: " + std::string(e.what());
        result.confidence = 0;
        return result;
    }
    catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        result.type = ComplexityType::UNKNOWN;
        result.notation = "O(?)";
        result.explanation = "Unexpected error during analysis";
        result.confidence = 0;
        return result;
    }
}

ComplexityBreakdown ComplexityEstimator::getDetailedBreakdown() {
    return lastBreakdown;
}

ComplexityType ComplexityEstimator::combineComplexities(
    ComplexityType loopComplexity,
    ComplexityType recursionComplexity
) {
    return ComplexityUtils::combineComplexities(loopComplexity, recursionComplexity);
}

int ComplexityEstimator::calculateConfidence(
    const std::vector<LoopInfo>& loops,
    const std::vector<RecursionInfo>& recursions,
    const std::vector<AlgorithmPattern>& patterns
) {
    int confidence = 50;

    if (!loops.empty()) {
        confidence += 15;

        bool hasComplexPatterns = false;
        for (const auto& loop : loops) {
            if (loop.hasEarlyTermination || loop.hasDependentBounds) {
                hasComplexPatterns = true;
                break;
            }
        }

        if (!hasComplexPatterns) {
            confidence += 10;
        }
    }

    if (!recursions.empty()) {
        confidence += 15;

        for (const auto& rec : recursions) {
            if (!rec.recurrenceRelation.empty()) {
                confidence += 5;
                break;
            }
        }
    }

    if (!patterns.empty()) {
        confidence += patterns[0].confidence / 5;
    }

    return std::min(100, confidence);
}

std::string ComplexityEstimator::generateExplanation(const ComplexityBreakdown& breakdown) {
    std::ostringstream oss;

    oss << "Combined analysis: ";

    if (breakdown.loopComplexity != ComplexityType::CONSTANT) {
        oss << "Loops contribute " << ComplexityUtils::complexityToString(breakdown.loopComplexity);
    }

    if (breakdown.recursionComplexity != ComplexityType::CONSTANT) {
        if (breakdown.loopComplexity != ComplexityType::CONSTANT) {
            oss << ", ";
        }
        oss << "Recursion contributes " << ComplexityUtils::complexityToString(breakdown.recursionComplexity);
    }

    if (breakdown.patternComplexity != ComplexityType::UNKNOWN) {
        oss << ". Pattern suggests " << ComplexityUtils::complexityToString(breakdown.patternComplexity);
    }

    oss << ". ";

    for (size_t i = 0; i < breakdown.explanations.size(); i++) {
        if (i > 0) oss << " ";
        oss << breakdown.explanations[i] << ".";
    }

    return oss.str();
}
