#ifndef PARSER_H
#define PARSER_H

#include "ComplexityTypes.h"
#include <string>
#include <vector>
#include <regex>
#include <memory>

class Parser {
public:

    Parser();

    ~Parser();

    bool loadFile(const std::string& filename);

    void loadFromString(const std::string& code);

    std::vector<std::string> tokenize();

    const std::string& getSourceCode() const;

    const std::string& getOriginalCode() const;

    const std::vector<std::string>& getTokens() const;

    std::vector<std::string> extractFunctionSignatures();

    std::vector<std::string> extractFunctionNames();

    bool isValid() const;

    int getLineCount() const;

private:

    std::string preprocessCode(const std::string& code);

    std::string removeSingleLineComments(const std::string& code);

    std::string removeMultiLineComments(const std::string& code);

    std::string removePreprocessorDirectives(const std::string& code);

    std::string sourceCode;
    std::string originalCode;
    std::vector<std::string> tokens;

    static const std::regex singleCommentRegex;
    static const std::regex multiCommentRegex;
    static const std::regex preprocessorRegex;
    static const std::regex tokenRegex;
    static const std::regex functionRegex;
};

#endif
