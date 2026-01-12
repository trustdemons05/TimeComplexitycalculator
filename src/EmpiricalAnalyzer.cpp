#include "../include/EmpiricalAnalyzer.h"
#include "../include/ComplexityUtils.h"
#include <cmath>
#include <algorithm>
#include <numeric>

EmpiricalAnalyzer::EmpiricalAnalyzer() {}

EmpiricalAnalyzer::~EmpiricalAnalyzer() {}

ComplexityResult EmpiricalAnalyzer::analyzeComplexity(const std::vector<DataPoint>& data) {
    ComplexityResult result;

    if (data.size() < 2) {
        result.type = ComplexityType::UNKNOWN;
        result.notation = "O(?)";
        result.explanation = "Insufficient data points for analysis";
        result.confidence = 0;
        return result;
    }

    ComplexityType bestFit = findBestFit(data);
    CurveFitResult fitResult = fitCurve(data, bestFit);

    if (fitResult.rSquared < 0.90) {
        result.type = ComplexityType::UNKNOWN;
        result.notation = "O(?)";
        result.explanation = "No complexity class fits well (best R² = " +
                            std::to_string(fitResult.rSquared) +
                            " < 0.90 threshold). Data may be noisy or complexity is non-standard.";
        result.confidence = static_cast<int>(fitResult.rSquared * 100);
        return result;
    }

    result.type = bestFit;
    result.notation = ComplexityUtils::complexityToString(bestFit);
    result.confidence = static_cast<int>(fitResult.rSquared * 100);

    std::string confidenceLevel;
    if (fitResult.rSquared > 0.95) {
        confidenceLevel = "High";
    } else if (fitResult.rSquared > 0.90) {
        confidenceLevel = "Medium";
    } else {
        confidenceLevel = "Low";
    }

    result.explanation = "Empirical analysis (" + confidenceLevel + " confidence): R² = " +
                        std::to_string(fitResult.rSquared) +
                        ", coefficient = " + std::to_string(fitResult.coefficient);

    return result;
}

std::vector<double> EmpiricalAnalyzer::calculateGrowthRatios(const std::vector<DataPoint>& data) {
    std::vector<double> ratios;

    for (size_t i = 1; i < data.size(); i++) {
        if (data[i-1].executionTime_us > 0) {
            double ratio = data[i].executionTime_us / data[i-1].executionTime_us;
            ratios.push_back(ratio);
        }
    }

    return ratios;
}

CurveFitResult EmpiricalAnalyzer::fitCurve(
    const std::vector<DataPoint>& data,
    ComplexityType targetComplexity
) {
    CurveFitResult result;
    result.complexity = targetComplexity;
    result.rSquared = calculateRSquared(data, targetComplexity);

    std::vector<double> coefficients;
    for (const auto& point : data) {
        double theoretical = evaluateComplexityFunction(targetComplexity, point.inputSize);
        if (theoretical > 0) {
            coefficients.push_back(point.executionTime_us / theoretical);
        }
    }

    result.coefficient = coefficients.empty() ? 1.0 : calculateMean(coefficients);

    return result;
}

ComplexityType EmpiricalAnalyzer::findBestFit(const std::vector<DataPoint>& data) {

    std::vector<ComplexityType> types = {
        ComplexityType::CONSTANT,
        ComplexityType::LOGARITHMIC,
        ComplexityType::LINEAR,
        ComplexityType::LINEARITHMIC,
        ComplexityType::QUADRATIC,
        ComplexityType::CUBIC
    };

    ComplexityType bestType = ComplexityType::LINEAR;
    double bestRSquared = -1.0;
    ComplexityType secondBestType = ComplexityType::LINEAR;
    double secondBestRSquared = -1.0;

    for (ComplexityType type : types) {
        double rSquared = calculateRSquared(data, type);

        if (rSquared > bestRSquared) {

            secondBestType = bestType;
            secondBestRSquared = bestRSquared;
            bestType = type;
            bestRSquared = rSquared;
        } else if (rSquared > secondBestRSquared) {

            secondBestType = type;
            secondBestRSquared = rSquared;
        }
    }

    if (bestRSquared - secondBestRSquared < 0.05) {

        int bestRank = ComplexityUtils::getComplexityRank(bestType);
        int secondRank = ComplexityUtils::getComplexityRank(secondBestType);

        if (secondRank < bestRank) {

            bestType = secondBestType;
            bestRSquared = secondBestRSquared;
        }
    }

    std::vector<double> ratios = calculateGrowthRatios(data);
    ComplexityType ratioType = classifyByGrowthRatio(ratios);

    if (ratioType != bestType) {
        double ratioRSquared = calculateRSquared(data, ratioType);

        if (ratioRSquared > bestRSquared + 0.05) {
            bestType = ratioType;
        }
    }

    return bestType;
}

double EmpiricalAnalyzer::calculateRSquared(
    const std::vector<DataPoint>& data,
    ComplexityType complexity
) {
    if (data.empty()) return 0.0;

    std::vector<double> actualTimes;
    for (const auto& point : data) {
        actualTimes.push_back(point.executionTime_us);
    }
    double meanActual = calculateMean(actualTimes);

    std::vector<double> predictedTimes;
    for (const auto& point : data) {
        predictedTimes.push_back(evaluateComplexityFunction(complexity, point.inputSize));
    }

    double scale = 0.0;
    double sumPredicted = 0.0;
    for (double pred : predictedTimes) {
        sumPredicted += pred;
    }
    if (sumPredicted > 0) {
        scale = std::accumulate(actualTimes.begin(), actualTimes.end(), 0.0) / sumPredicted;
    }

    for (double& pred : predictedTimes) {
        pred *= scale;
    }

    double ssRes = 0.0;
    double ssTot = 0.0;

    for (size_t i = 0; i < data.size(); i++) {
        double residual = actualTimes[i] - predictedTimes[i];
        ssRes += residual * residual;

        double deviation = actualTimes[i] - meanActual;
        ssTot += deviation * deviation;
    }

    if (ssTot == 0.0) return 0.0;

    double rSquared = 1.0 - (ssRes / ssTot);

    return std::max(0.0, std::min(1.0, rSquared));
}

double EmpiricalAnalyzer::evaluateComplexityFunction(ComplexityType type, int n) {
    if (n <= 0) return 1.0;

    switch (type) {
        case ComplexityType::CONSTANT:
            return 1.0;

        case ComplexityType::LOGARITHMIC:
            return std::log2(n);

        case ComplexityType::LINEAR:
            return n;

        case ComplexityType::LINEARITHMIC:
            return n * std::log2(n);

        case ComplexityType::QUADRATIC:
            return n * n;

        case ComplexityType::CUBIC:
            return n * n * n;

        case ComplexityType::EXPONENTIAL:

            return std::pow(2.0, std::min(n, 20));

        case ComplexityType::FACTORIAL:

            return std::exp(n * std::log(n) - n);

        default:
            return n;
    }
}

double EmpiricalAnalyzer::calculateMean(const std::vector<double>& values) {
    if (values.empty()) return 0.0;

    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    return sum / values.size();
}

ComplexityType EmpiricalAnalyzer::classifyByGrowthRatio(const std::vector<double>& ratios) {
    if (ratios.empty()) return ComplexityType::LINEAR;

    double avgRatio = calculateMean(ratios);

    if (avgRatio < 1.2) {
        return ComplexityType::CONSTANT;
    } else if (avgRatio < 1.7) {
        return ComplexityType::LOGARITHMIC;
    } else if (avgRatio < 2.3) {
        return ComplexityType::LINEAR;
    } else if (avgRatio < 3.0) {
        return ComplexityType::LINEARITHMIC;
    } else if (avgRatio < 6.0) {
        return ComplexityType::QUADRATIC;
    } else {
        return ComplexityType::CUBIC;
    }
}
