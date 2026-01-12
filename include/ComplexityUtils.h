#ifndef COMPLEXITY_UTILS_H
#define COMPLEXITY_UTILS_H

#include "ComplexityTypes.h"
#include <string>

class ComplexityUtils {
public:

    static int getComplexityRank(ComplexityType type);

    static std::string complexityToString(ComplexityType type);

    static std::string complexityToName(ComplexityType type);

    static int compareComplexity(ComplexityType a, ComplexityType b);

    static ComplexityType maxComplexity(ComplexityType a, ComplexityType b);

    static ComplexityType combineComplexities(ComplexityType loopComplexity,
                                              ComplexityType recursionComplexity);

    static std::string getComplexityColor(ComplexityType type);

private:
    ComplexityUtils() = delete;
};

#endif
