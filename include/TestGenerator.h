#ifndef TEST_GENERATOR_H
#define TEST_GENERATOR_H

#include "ComplexityTypes.h"
#include <string>
#include <vector>

class TestGenerator {
public:

    TestGenerator();

    ~TestGenerator();

    std::string generateTestCode(
        const std::string& sourceCode,
        const std::string& functionName,
        const std::vector<int>& inputSizes
    );

    FunctionSignature parseFunctionSignature(
        const std::string& code,
        const std::string& functionName
    );

private:

    std::string generateArrayInput(const std::string& type, int size, const std::string& varName);

    std::string generateIntegerInput(const std::string& type, const std::string& varName);

    std::string generateStringInput(int size, const std::string& varName);

    std::string generateTimingCode();

    std::string generateMemoryTrackingCode();

    std::string generateFunctionCall(const FunctionSignature& signature, int size);

    std::string generateCleanupCode(const FunctionSignature& signature);

    Parameter parseParameter(const std::string& paramStr);

    std::string extractBaseType(const std::string& type);
};

#endif
