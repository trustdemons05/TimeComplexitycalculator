#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include "ComplexityTypes.h"
#include <string>
#include <iostream>
#include <cmath>

class TestFramework {
public:
    static int totalTests;
    static int passedTests;
    static int failedTests;

    static void reset() {
        totalTests = 0;
        passedTests = 0;
        failedTests = 0;
    }

    static void printSummary() {
        std::cout << "\n========================================\n";
        std::cout << "Test Summary:\n";
        std::cout << "  Total:  " << totalTests << "\n";
        std::cout << "  Passed: " << passedTests << " (" << (totalTests > 0 ? (passedTests * 100 / totalTests) : 0) << "%)\n";
        std::cout << "  Failed: " << failedTests << "\n";
        std::cout << "========================================\n";
    }

    static int getExitCode() {
        return (failedTests == 0) ? 0 : 1;
    }
};

int TestFramework::totalTests = 0;
int TestFramework::passedTests = 0;
int TestFramework::failedTests = 0;

#define ASSERT_EQUAL(expected, actual) \
    do { \
        TestFramework::totalTests++; \
        if ((expected) == (actual)) { \
            TestFramework::passedTests++; \
            std::cout << "[PASS] " << __FILE__ << ":" << __LINE__ << " - ASSERT_EQUAL\n"; \
        } else { \
            TestFramework::failedTests++; \
            std::cout << "[FAIL] " << __FILE__ << ":" << __LINE__ << " - ASSERT_EQUAL\n"; \
            std::cout << "       Expected: " << (expected) << "\n"; \
            std::cout << "       Actual:   " << (actual) << "\n"; \
        } \
    } while(0)

#define ASSERT_TRUE(condition) \
    do { \
        TestFramework::totalTests++; \
        if (condition) { \
            TestFramework::passedTests++; \
            std::cout << "[PASS] " << __FILE__ << ":" << __LINE__ << " - ASSERT_TRUE\n"; \
        } else { \
            TestFramework::failedTests++; \
            std::cout << "[FAIL] " << __FILE__ << ":" << __LINE__ << " - ASSERT_TRUE\n"; \
            std::cout << "       Condition: " << #condition << "\n"; \
        } \
    } while(0)

#define ASSERT_FALSE(condition) \
    do { \
        TestFramework::totalTests++; \
        if (!(condition)) { \
            TestFramework::passedTests++; \
            std::cout << "[PASS] " << __FILE__ << ":" << __LINE__ << " - ASSERT_FALSE\n"; \
        } else { \
            TestFramework::failedTests++; \
            std::cout << "[FAIL] " << __FILE__ << ":" << __LINE__ << " - ASSERT_FALSE\n"; \
            std::cout << "       Condition: " << #condition << "\n"; \
        } \
    } while(0)

#define ASSERT_NEAR(expected, actual, tolerance) \
    do { \
        TestFramework::totalTests++; \
        double diff = std::abs((expected) - (actual)); \
        if (diff <= (tolerance)) { \
            TestFramework::passedTests++; \
            std::cout << "[PASS] " << __FILE__ << ":" << __LINE__ << " - ASSERT_NEAR\n"; \
        } else { \
            TestFramework::failedTests++; \
            std::cout << "[FAIL] " << __FILE__ << ":" << __LINE__ << " - ASSERT_NEAR\n"; \
            std::cout << "       Expected: " << (expected) << " ± " << (tolerance) << "\n"; \
            std::cout << "       Actual:   " << (actual) << "\n"; \
            std::cout << "       Diff:     " << diff << "\n"; \
        } \
    } while(0)

#define ASSERT_COMPLEXITY(expected, actual) \
    do { \
        TestFramework::totalTests++; \
        if ((expected) == (actual)) { \
            TestFramework::passedTests++; \
            std::cout << "[PASS] " << __FILE__ << ":" << __LINE__ << " - ASSERT_COMPLEXITY\n"; \
        } else { \
            TestFramework::failedTests++; \
            std::cout << "[FAIL] " << __FILE__ << ":" << __LINE__ << " - ASSERT_COMPLEXITY\n"; \
            std::cout << "       Expected: " << ComplexityUtils::complexityToString(expected) << "\n"; \
            std::cout << "       Actual:   " << ComplexityUtils::complexityToString(actual) << "\n"; \
        } \
    } while(0)

#define TEST_SECTION(name) \
    std::cout << "\n--- " << name << " ---\n"

#endif
