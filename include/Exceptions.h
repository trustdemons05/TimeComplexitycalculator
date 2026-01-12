#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <stdexcept>
#include <string>

class AnalyzerException : public std::runtime_error {
public:
    explicit AnalyzerException(const std::string& message)
        : std::runtime_error(message), message_(message), file_(""), line_(0) {}

    AnalyzerException(const std::string& message, const std::string& file, int line)
        : std::runtime_error(message), message_(message), file_(file), line_(line) {}

    virtual ~AnalyzerException() noexcept {}

    const char* what() const noexcept override {
        return message_.c_str();
    }

    const std::string& getMessage() const { return message_; }
    const std::string& getFile() const { return file_; }
    int getLine() const { return line_; }

    std::string getFullMessage() const {
        std::string full = message_;
        if (!file_.empty()) {
            full += " (at " + file_;
            if (line_ > 0) {
                full += ":" + std::to_string(line_);
            }
            full += ")";
        }
        return full;
    }

protected:
    std::string message_;
    std::string file_;
    int line_;
};

class ParseException : public AnalyzerException {
public:
    explicit ParseException(const std::string& message)
        : AnalyzerException(message) {}

    ParseException(const std::string& message, int line)
        : AnalyzerException(message, "", line) {}

    ParseException(const std::string& message, const std::string& file, int line)
        : AnalyzerException(message, file, line) {}
};

class CompilationException : public AnalyzerException {
public:
    explicit CompilationException(const std::string& message)
        : AnalyzerException(message), compilerOutput_("") {}

    CompilationException(const std::string& message, const std::string& compilerOutput)
        : AnalyzerException(message), compilerOutput_(compilerOutput) {}

    const std::string& getCompilerOutput() const { return compilerOutput_; }

    std::string getFullMessage() const {
        std::string full = AnalyzerException::getFullMessage();
        if (!compilerOutput_.empty()) {
            full += "\nCompiler output:\n" + compilerOutput_;
        }
        return full;
    }

private:
    std::string compilerOutput_;
};

class ExecutionException : public AnalyzerException {
public:
    explicit ExecutionException(const std::string& message)
        : AnalyzerException(message), exitCode_(-1) {}

    ExecutionException(const std::string& message, int exitCode)
        : AnalyzerException(message), exitCode_(exitCode) {}

    int getExitCode() const { return exitCode_; }

private:
    int exitCode_;
};

class TimeoutException : public ExecutionException {
public:
    explicit TimeoutException(int seconds)
        : ExecutionException("Execution timed out after " + std::to_string(seconds) + " seconds"),
          timeoutSeconds_(seconds) {}

    int getTimeoutSeconds() const { return timeoutSeconds_; }

private:
    int timeoutSeconds_;
};

class MemoryException : public ExecutionException {
public:
    explicit MemoryException(size_t limitBytes)
        : ExecutionException("Execution exceeded memory limit of " +
                           std::to_string(limitBytes / (1024*1024)) + " MB"),
          limitBytes_(limitBytes) {}

    size_t getLimitBytes() const { return limitBytes_; }

private:
    size_t limitBytes_;
};

class FileException : public AnalyzerException {
public:
    explicit FileException(const std::string& message)
        : AnalyzerException(message), filename_("") {}

    FileException(const std::string& message, const std::string& filename)
        : AnalyzerException(message), filename_(filename) {}

    const std::string& getFilename() const { return filename_; }

    std::string getFullMessage() const {
        std::string full = AnalyzerException::getFullMessage();
        if (!filename_.empty()) {
            full += " (file: " + filename_ + ")";
        }
        return full;
    }

private:
    std::string filename_;
};

class AnalysisException : public AnalyzerException {
public:
    explicit AnalysisException(const std::string& message)
        : AnalyzerException(message) {}

    AnalysisException(const std::string& message, const std::string& component)
        : AnalyzerException(message), component_(component) {}

    const std::string& getComponent() const { return component_; }

private:
    std::string component_;
};

class InvalidInputException : public AnalyzerException {
public:
    explicit InvalidInputException(const std::string& message)
        : AnalyzerException(message), parameterName_("") {}

    InvalidInputException(const std::string& message, const std::string& parameterName)
        : AnalyzerException(message), parameterName_(parameterName) {}

    const std::string& getParameterName() const { return parameterName_; }

private:
    std::string parameterName_;
};

#endif
