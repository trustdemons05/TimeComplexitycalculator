#ifndef CODE_UTILS_H
#define CODE_UTILS_H

#include <string>
#include <vector>
#include <regex>

class CodeUtils {
public:

    static size_t findMatchingBrace(const std::string& code, size_t start);

    static size_t findMatchingParen(const std::string& code, size_t start);

    static std::vector<std::string> splitLines(const std::string& code);

    static size_t lineToPosition(const std::string& code, int lineNumber);

    static int positionToLine(const std::string& code, size_t position);

    static bool isInsideStringLiteral(const std::string& code, size_t position);

    static bool isInsideComment(const std::string& code, size_t position);

    static std::string trim(const std::string& str);

    static int countOccurrences(const std::string& text, const std::string& pattern);

    static std::string extractFunctionBody(const std::string& code, size_t functionStart);

private:
    CodeUtils() = delete;
};

#endif
