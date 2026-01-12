#include "../include/CompilerWrapper.h"
#include <cstdlib>
#include <cstdio>
#include <sstream>
#include <fstream>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#endif

CompilerWrapper::CompilerWrapper() {
    compilerPath = detectCompiler();

    defaultFlags = "-std=c++11 -O0 -Wall";
}

CompilerWrapper::~CompilerWrapper() {}

CompilationResult CompilerWrapper::compile(
    const std::string& sourceFile,
    const std::string& outputFile,
    const std::string& flags
) {
    CompilationResult result;
    result.success = false;

    if (compilerPath.empty()) {
        result.errorMessage = "No C++ compiler found. Please install g++, clang++, or MSVC.";
        return result;
    }

    std::ostringstream cmd;
    cmd << compilerPath << " ";

    if (!flags.empty()) {
        cmd << flags << " ";
    } else {
        cmd << defaultFlags << " ";
    }

    cmd << sourceFile << " -o " << outputFile;

    std::string output;
    int exitCode = executeCommand(cmd.str(), output);

    if (exitCode == 0) {
        result.success = true;
        result.outputFile = outputFile;
    } else {
        result.success = false;
        result.errorMessage = "Compilation failed with exit code " + std::to_string(exitCode);
    }

    parseCompilerOutput(output, result);

    return result;
}

std::string CompilerWrapper::detectCompiler() {

    std::string gcc = findGCC();
    if (!gcc.empty()) {
        return gcc;
    }

    std::string clang = findClang();
    if (!clang.empty()) {
        return clang;
    }

    std::string msvc = findMSVC();
    if (!msvc.empty()) {
        return msvc;
    }

    return "";
}

std::string CompilerWrapper::findGCC() {
    std::string output;
    int result = executeCommand("g++ --version", output);

    if (result == 0) {
        return "g++";
    }

    return "";
}

std::string CompilerWrapper::findClang() {
    std::string output;
    int result = executeCommand("clang++ --version", output);

    if (result == 0) {
        return "clang++";
    }

    return "";
}

std::string CompilerWrapper::findMSVC() {
    std::string output;
    executeCommand("cl", output);

    if (output.find("Microsoft") != std::string::npos) {
        return "cl";
    }

    return "";
}

std::string CompilerWrapper::getCompilerVersion() {
    if (compilerPath.empty()) {
        return "No compiler detected";
    }

    std::string output;
    executeCommand(compilerPath + " --version", output);

    size_t newlinePos = output.find('\n');
    if (newlinePos != std::string::npos) {
        return output.substr(0, newlinePos);
    }

    return output;
}

std::string CompilerWrapper::getDefaultFlags() const {
    return defaultFlags;
}

int CompilerWrapper::executeCommand(const std::string& command, std::string& output) {

    std::string tempFile = "temp_compiler_output.txt";
    std::string fullCommand = command + " > " + tempFile + " 2>&1";

    int exitCode = system(fullCommand.c_str());

    std::ifstream file(tempFile);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            output += line + "\n";
        }
        file.close();

        remove(tempFile.c_str());
    }

    return exitCode;
}

void CompilerWrapper::parseCompilerOutput(const std::string& output, CompilationResult& result) {
    if (output.empty()) {
        return;
    }

    std::istringstream stream(output);
    std::string line;

    while (std::getline(stream, line)) {

        if (line.find("error:") != std::string::npos ||
            line.find("Error:") != std::string::npos ||
            line.find("ERROR:") != std::string::npos) {
            if (!result.errorMessage.empty()) {
                result.errorMessage += "\n";
            }
            result.errorMessage += line;
        }

        else if (line.find("warning:") != std::string::npos ||
                 line.find("Warning:") != std::string::npos ||
                 line.find("WARNING:") != std::string::npos) {
            result.warnings.push_back(line);
        }
    }
}
