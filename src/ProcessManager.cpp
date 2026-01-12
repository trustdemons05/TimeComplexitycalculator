#include "../include/ProcessManager.h"
#include <cstdlib>
#include <cstdio>
#include <sstream>
#include <fstream>
#include <iostream>
#include <chrono>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <signal.h>
#endif

ProcessManager::ProcessManager() {}

ProcessManager::~ProcessManager() {}

ExecutionResult ProcessManager::execute(
    const std::string& executable,
    int timeoutSeconds,
    size_t memoryLimitBytes
) {
#ifdef _WIN32
    return executeWindows(executable, timeoutSeconds, memoryLimitBytes);
#else
    return executeUnix(executable, timeoutSeconds, memoryLimitBytes);
#endif
}

ExecutionResult ProcessManager::executeWindows(const std::string& exe, int timeout, size_t memLimit) {
    ExecutionResult result;
    result.success = false;
    result.timeout = false;
    result.memoryExceeded = false;
    result.crashed = false;
    result.exitCode = -1;

#ifdef _WIN32

    HANDLE hStdoutRead, hStdoutWrite;
    HANDLE hStderrRead, hStderrWrite;
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    if (!CreatePipe(&hStdoutRead, &hStdoutWrite, &sa, 0)) {
        result.errorOutput = "Failed to create stdout pipe";
        return result;
    }
    if (!CreatePipe(&hStderrRead, &hStderrWrite, &sa, 0)) {
        CloseHandle(hStdoutRead);
        CloseHandle(hStdoutWrite);
        result.errorOutput = "Failed to create stderr pipe";
        return result;
    }

    SetHandleInformation(hStdoutRead, HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation(hStderrRead, HANDLE_FLAG_INHERIT, 0);

    HANDLE hJob = CreateJobObject(NULL, NULL);
    if (hJob == NULL) {
        CloseHandle(hStdoutRead);
        CloseHandle(hStdoutWrite);
        CloseHandle(hStderrRead);
        CloseHandle(hStderrWrite);
        result.errorOutput = "Failed to create job object";
        return result;
    }

    JOBOBJECT_EXTENDED_LIMIT_INFORMATION extendedLimits;
    ZeroMemory(&extendedLimits, sizeof(extendedLimits));

    extendedLimits.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_PROCESS_TIME | JOB_OBJECT_LIMIT_PROCESS_MEMORY;
    extendedLimits.BasicLimitInformation.PerProcessUserTimeLimit.QuadPart = static_cast<LONGLONG>(timeout) * 10000000LL;
    extendedLimits.ProcessMemoryLimit = memLimit;

    if (!SetInformationJobObject(hJob, JobObjectExtendedLimitInformation, &extendedLimits, sizeof(extendedLimits))) {
        CloseHandle(hJob);
        CloseHandle(hStdoutRead);
        CloseHandle(hStdoutWrite);
        CloseHandle(hStderrRead);
        CloseHandle(hStderrWrite);
        result.errorOutput = "Failed to set job limits";
        return result;
    }

    STARTUPINFOA si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdOutput = hStdoutWrite;
    si.hStdError = hStderrWrite;
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    std::string cmdLine = exe;
    BOOL success = CreateProcessA(
        NULL,
        const_cast<char*>(cmdLine.c_str()),
        NULL,
        NULL,
        TRUE,
        CREATE_SUSPENDED | CREATE_NO_WINDOW,
        NULL,
        NULL,
        &si,
        &pi
    );

    if (!success) {
        DWORD error = GetLastError();
        CloseHandle(hJob);
        CloseHandle(hStdoutRead);
        CloseHandle(hStdoutWrite);
        CloseHandle(hStderrRead);
        CloseHandle(hStderrWrite);
        result.errorOutput = "Failed to create process. Error code: " + std::to_string(error);
        return result;
    }

    if (!AssignProcessToJobObject(hJob, pi.hProcess)) {
        TerminateProcess(pi.hProcess, 1);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        CloseHandle(hJob);
        CloseHandle(hStdoutRead);
        CloseHandle(hStdoutWrite);
        CloseHandle(hStderrRead);
        CloseHandle(hStderrWrite);
        result.errorOutput = "Failed to assign process to job";
        return result;
    }

    ResumeThread(pi.hThread);

    CloseHandle(hStdoutWrite);
    CloseHandle(hStderrWrite);

    DWORD waitResult = WaitForSingleObject(pi.hProcess, timeout * 1000);

    if (waitResult == WAIT_TIMEOUT) {

        result.timeout = true;
        TerminateProcess(pi.hProcess, 1);
        result.errorOutput = "Execution timed out after " + std::to_string(timeout) + " seconds";
    } else if (waitResult == WAIT_OBJECT_0) {

        DWORD exitCode;
        if (GetExitCodeProcess(pi.hProcess, &exitCode)) {
            result.exitCode = static_cast<int>(exitCode);
            result.success = (exitCode == 0);
            if (exitCode != 0) {
                result.crashed = true;
            }
        }
    } else {

        result.errorOutput = "Wait failed with error: " + std::to_string(GetLastError());
        TerminateProcess(pi.hProcess, 1);
    }

    char buffer[4096];
    DWORD bytesRead;
    while (ReadFile(hStdoutRead, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        result.output += buffer;
    }

    while (ReadFile(hStderrRead, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        result.errorOutput += buffer;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(hJob);
    CloseHandle(hStdoutRead);
    CloseHandle(hStderrRead);

#endif

    return result;
}

ExecutionResult ProcessManager::executeUnix(const std::string& exe, int timeout, size_t memLimit) {
    ExecutionResult result;
    result.success = false;
    result.timeout = false;
    result.memoryExceeded = false;
    result.crashed = false;
    result.exitCode = -1;

#ifndef _WIN32

    int stdoutPipe[2];
    int stderrPipe[2];

    if (pipe(stdoutPipe) == -1) {
        result.errorOutput = "Failed to create stdout pipe";
        return result;
    }
    if (pipe(stderrPipe) == -1) {
        close(stdoutPipe[0]);
        close(stdoutPipe[1]);
        result.errorOutput = "Failed to create stderr pipe";
        return result;
    }

    pid_t pid = fork();

    if (pid == -1) {

        close(stdoutPipe[0]);
        close(stdoutPipe[1]);
        close(stderrPipe[0]);
        close(stderrPipe[1]);
        result.errorOutput = "Failed to fork process";
        return result;
    }

    if (pid == 0) {

        dup2(stdoutPipe[1], STDOUT_FILENO);
        dup2(stderrPipe[1], STDERR_FILENO);

        close(stdoutPipe[0]);
        close(stdoutPipe[1]);
        close(stderrPipe[0]);
        close(stderrPipe[1]);

        if (memLimit > 0) {
            struct rlimit memoryLimit;
            memoryLimit.rlim_cur = memLimit;
            memoryLimit.rlim_max = memLimit;
            if (setrlimit(RLIMIT_AS, &memoryLimit) != 0) {
                perror("setrlimit failed");

            }
        }

        alarm(timeout);

        execl(exe.c_str(), exe.c_str(), (char*)NULL);

        perror("execl failed");
        exit(127);
    }

    close(stdoutPipe[1]);
    close(stderrPipe[1]);

    auto startTime = std::chrono::steady_clock::now();
    int status;
    bool processFinished = false;

    while (!processFinished) {

        pid_t waitResult = waitpid(pid, &status, WNOHANG);

        if (waitResult == pid) {

            processFinished = true;

            if (WIFEXITED(status)) {
                result.exitCode = WEXITSTATUS(status);
                result.success = (result.exitCode == 0);
                if (result.exitCode != 0) {
                    result.crashed = true;
                }
            } else if (WIFSIGNALED(status)) {
                result.crashed = true;
                result.exitCode = -1;
                int sig = WTERMSIG(status);
                if (sig == SIGALRM) {
                    result.timeout = true;
                    result.errorOutput = "Execution timed out after " + std::to_string(timeout) + " seconds";
                } else {
                    result.errorOutput = "Process terminated by signal: " + std::to_string(sig);
                }
            }
        } else if (waitResult == -1) {

            result.errorOutput = "waitpid failed";
            kill(pid, SIGKILL);
            processFinished = true;
        } else {

            auto currentTime = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime);

            if (elapsed.count() >= timeout) {

                result.timeout = true;
                result.errorOutput = "Execution timed out after " + std::to_string(timeout) + " seconds";
                kill(pid, SIGKILL);
                waitpid(pid, &status, 0);
                processFinished = true;
            } else {

                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    }

    char buffer[4096];
    ssize_t bytesRead;
    while ((bytesRead = read(stdoutPipe[0], buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytesRead] = '\0';
        result.output += buffer;
    }

    while ((bytesRead = read(stderrPipe[0], buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytesRead] = '\0';
        result.errorOutput += buffer;
    }

    close(stdoutPipe[0]);
    close(stderrPipe[0]);

#endif

    return result;
}

std::vector<DataPoint> ProcessManager::parseOutput(const std::string& output) {
    std::vector<DataPoint> dataPoints;

    std::istringstream stream(output);
    std::string line;

    while (std::getline(stream, line)) {

        if (line.find("SIZE:") != std::string::npos && line.find("TIME:") != std::string::npos) {
            DataPoint point;
            point.timeout = false;
            point.crashed = false;
            point.memoryUsage_bytes = 0;

            size_t sizePos = line.find("SIZE:");
            size_t commaPos = line.find(",", sizePos);
            if (sizePos != std::string::npos && commaPos != std::string::npos) {
                std::string sizeStr = line.substr(sizePos + 5, commaPos - sizePos - 5);
                point.inputSize = std::stoi(sizeStr);
            }

            size_t timePos = line.find("TIME:");
            if (timePos != std::string::npos) {
                std::string timeStr = line.substr(timePos + 5);

                timeStr.erase(timeStr.find_last_not_of(" \n\r\t") + 1);
                point.executionTime_us = std::stod(timeStr);
            }

            dataPoints.push_back(point);
        }
    }

    return dataPoints;
}

void ProcessManager::killProcess(int pid) {
#ifdef _WIN32
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (hProcess != NULL) {
        TerminateProcess(hProcess, 1);
        CloseHandle(hProcess);
    }
#else
    kill(pid, SIGKILL);
#endif
}

size_t ProcessManager::getProcessMemory(int ) {

    return 0;
}
