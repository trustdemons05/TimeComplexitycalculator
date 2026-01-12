#ifndef COMPILER_WRAPPER_H
#define COMPILER_WRAPPER_H

#include "ComplexityTypes.h"
#include <string>

class CompilerWrapper {
public:

    CompilerWrapper();

    ~CompilerWrapper();

    CompilationResult compile(
        const std::string& sourceFile,
        const std::string& outputFile,
        const std::string& flags = ""
    );

    std::string detectCompiler();

    std::string getCompilerVersion();

    std::string getDefaultFlags() const;

private:

    std::string findGCC();

    std::string findClang();

    std::string findMSVC();

    int executeCommand(const std::string& command, std::string& output);

    void parseCompilerOutput(const std::string& output, CompilationResult& result);

    std::string compilerPath;
    std::string defaultFlags;
};

#endif
