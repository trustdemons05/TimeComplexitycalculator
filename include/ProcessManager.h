#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include "ComplexityTypes.h"
#include <string>
#include <vector>

class ProcessManager {
public:

    ProcessManager();

    ~ProcessManager();

    ExecutionResult execute(
        const std::string& executable,
        int timeoutSeconds,
        size_t memoryLimitBytes
    );

    std::vector<DataPoint> parseOutput(const std::string& output);

private:

    ExecutionResult executeWindows(const std::string& exe, int timeout, size_t memLimit);

    ExecutionResult executeUnix(const std::string& exe, int timeout, size_t memLimit);

    void killProcess(int pid);

    size_t getProcessMemory(int pid);
};

#endif
