#include "../include/ResultComparator.h"
#include "../include/ComplexityUtils.h"
#include <cmath>
#include <sstream>

ResultComparator::ResultComparator() {}

ResultComparator::~ResultComparator() {}

ComparisonReport ResultComparator::compare(
    const ComplexityResult& staticResult,
    const ProfilingResult& runtimeResult
) {
    ComparisonReport report;

    report.staticComplexity = staticResult.type;
    report.runtimeComplexity = runtimeResult.detectedComplexity;
    report.match = (staticResult.type == runtimeResult.detectedComplexity);
    report.accuracy = calculateAccuracy(staticResult.type, runtimeResult.detectedComplexity);

    std::ostringstream oss;
    oss << "Static analysis predicted " << ComplexityUtils::complexityToString(staticResult.type);
    oss << " with " << staticResult.confidence << "% confidence. ";
    oss << "Runtime profiling measured " << ComplexityUtils::complexityToString(runtimeResult.detectedComplexity);
    oss << " with " << runtimeResult.confidence << "% confidence. ";

    if (report.match) {
        oss << "Results match!";
    } else {
        oss << explainDiscrepancy(staticResult.type, runtimeResult.detectedComplexity);
    }

    report.explanation = oss.str();
    report.recommendations = generateRecommendations(staticResult, runtimeResult);

    return report;
}

double ResultComparator::calculateAccuracy(
    ComplexityType predicted,
    ComplexityType actual
) {
    if (predicted == actual) {
        return 100.0;
    }

    int predictedRank = ComplexityUtils::getComplexityRank(predicted);
    int actualRank = ComplexityUtils::getComplexityRank(actual);

    if (predictedRank < 0 || actualRank < 0) {
        return 0.0;
    }

    int difference = std::abs(predictedRank - actualRank);

    double accuracy = 100.0 - (difference * 20.0);

    return std::max(20.0, accuracy);
}

std::string ResultComparator::explainDiscrepancy(
    ComplexityType predicted,
    ComplexityType actual
) {
    int predictedRank = ComplexityUtils::getComplexityRank(predicted);
    int actualRank = ComplexityUtils::getComplexityRank(actual);

    std::ostringstream oss;

    if (predictedRank < actualRank) {
        oss << "Static analysis underestimated complexity. ";
        oss << "This may be due to hidden patterns or runtime-specific behavior.";
    } else {
        oss << "Static analysis overestimated complexity. ";
        oss << "This may be due to early termination, optimizations, or small input sizes.";
    }

    return oss.str();
}

std::vector<std::string> ResultComparator::generateRecommendations(
    const ComplexityResult& staticResult,
    const ProfilingResult& runtimeResult
) {
    std::vector<std::string> recommendations;

    int staticRank = ComplexityUtils::getComplexityRank(staticResult.type);
    int runtimeRank = ComplexityUtils::getComplexityRank(runtimeResult.detectedComplexity);

    if (runtimeRank >= 4) {
        recommendations.push_back("Consider optimizing algorithm - current complexity is high");

        if (runtimeResult.detectedComplexity == ComplexityType::QUADRATIC) {
            recommendations.push_back("Look for opportunities to reduce nested loops");
        } else if (runtimeResult.detectedComplexity == ComplexityType::EXPONENTIAL) {
            recommendations.push_back("Consider using dynamic programming or memoization");
        }
    }

    if (staticRank != runtimeRank) {
        if (staticRank < runtimeRank) {
            recommendations.push_back("Static analysis may have missed some complexity factors");
            recommendations.push_back("Review code for hidden loops or recursive calls");
        } else {
            recommendations.push_back("Runtime may benefit from optimizations not visible in static analysis");
            recommendations.push_back("Consider profiling with larger input sizes");
        }
    }

    if (staticResult.confidence < 70) {
        recommendations.push_back("Static analysis confidence is low - manual review recommended");
    }

    if (runtimeResult.confidence < 70) {
        recommendations.push_back("Runtime profiling confidence is low - try more test sizes");
    }

    if (recommendations.empty()) {
        recommendations.push_back("Complexity analysis looks good - no immediate optimizations needed");
    }

    return recommendations;
}
