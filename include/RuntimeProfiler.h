#ifndef RUNTIME_PROFILER_H
#define RUNTIME_PROFILER_H

#include "ComplexityTypes.h"
#include "TestGenerator.h"
#include "CompilerWrapper.h"
#include "ProcessManager.h"
#include "EmpiricalAnalyzer.h"
#include <string>
#include <vector>

class RuntimeProfiler {
public:

    RuntimeProfiler();

    ~RuntimeProfiler();

    ProfilingResult profileFunction(
        const std::string& sourceFile,
        const std::string& functionName,
        const std::vector<int>& inputSizes
    );

    void setTimeout(int seconds);

    void setMemoryLimit(size_t bytes);

    void setCompilerFlags(const std::string& flags);

    const ProfilingResult& getLastResult() const;

    void setRunsPerSize(int runs);

private:
    TestGenerator testGen;
    CompilerWrapper compiler;
    ProcessManager procMgr;
    EmpiricalAnalyzer analyzer;

    int timeoutSeconds;
    size_t memoryLimitBytes;
    std::string compilerFlags;
    int runsPerSize;
    ProfilingResult lastResult;
};

#endif
