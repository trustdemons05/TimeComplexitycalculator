#ifndef EMPIRICAL_ANALYZER_H
#define EMPIRICAL_ANALYZER_H

#include "ComplexityTypes.h"
#include <vector>

class EmpiricalAnalyzer {
public:

    EmpiricalAnalyzer();

    ~EmpiricalAnalyzer();

    ComplexityResult analyzeComplexity(const std::vector<DataPoint>& data);

    std::vector<double> calculateGrowthRatios(const std::vector<DataPoint>& data);

    CurveFitResult fitCurve(
        const std::vector<DataPoint>& data,
        ComplexityType targetComplexity
    );

    ComplexityType findBestFit(const std::vector<DataPoint>& data);

private:

    double calculateRSquared(
        const std::vector<DataPoint>& data,
        ComplexityType complexity
    );

    double evaluateComplexityFunction(ComplexityType type, int n);

    double calculateMean(const std::vector<double>& values);

    ComplexityType classifyByGrowthRatio(const std::vector<double>& ratios);
};

#endif
