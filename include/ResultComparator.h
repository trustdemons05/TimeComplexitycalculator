#ifndef RESULT_COMPARATOR_H
#define RESULT_COMPARATOR_H

#include "ComplexityTypes.h"
#include <string>

class ResultComparator {
public:

    ResultComparator();

    ~ResultComparator();

    ComparisonReport compare(
        const ComplexityResult& staticResult,
        const ProfilingResult& runtimeResult
    );

    double calculateAccuracy(
        ComplexityType predicted,
        ComplexityType actual
    );

private:

    std::string explainDiscrepancy(
        ComplexityType predicted,
        ComplexityType actual
    );

    std::vector<std::string> generateRecommendations(
        const ComplexityResult& staticResult,
        const ProfilingResult& runtimeResult
    );
};

#endif
