#include "../include/CodeUtils.h"
#include <algorithm>
#include <sstream>

size_t CodeUtils::findMatchingBrace(const std::string& code, size_t start) {
    if (start >= code.length() || code[start] != '{') {
        return std::string::npos;
    }

    int braceCount = 1;
    bool inString = false;
    bool inChar = false;
    bool inSingleComment = false;
    bool inMultiComment = false;

    for (size_t i = start + 1; i < code.length(); ++i) {
        char c = code[i];
        char prev = (i > 0) ? code[i-1] : '\0';

        if (inSingleComment) {
            if (c == '\n') inSingleComment = false;
            continue;
        }

        if (inMultiComment) {
            if (c == '/' && prev == '*') inMultiComment = false;
            continue;
        }

        if (!inString && !inChar) {
            if (c == '/' && i + 1 < code.length()) {
                if (code[i+1] == '/') {
                    inSingleComment = true;
                    continue;
                } else if (code[i+1] == '*') {
                    inMultiComment = true;
                    i++;
                    continue;
                }
            }
        }

        if (c == '"' && prev != '\\' && !inChar) {
            inString = !inString;
            continue;
        }

        if (c == '\'' && prev != '\\' && !inString) {
            inChar = !inChar;
            continue;
        }

        if (!inString && !inChar) {
            if (c == '{') {
                braceCount++;
            } else if (c == '}') {
                braceCount--;
                if (braceCount == 0) {
                    return i;
                }
            }
        }
    }

    return std::string::npos;
}

size_t CodeUtils::findMatchingParen(const std::string& code, size_t start) {
    if (start >= code.length() || code[start] != '(') {
        return std::string::npos;
    }

    int parenCount = 1;
    bool inString = false;
    bool inChar = false;

    for (size_t i = start + 1; i < code.length(); ++i) {
        char c = code[i];
        char prev = (i > 0) ? code[i-1] : '\0';

        if (c == '"' && prev != '\\' && !inChar) {
            inString = !inString;
            continue;
        }

        if (c == '\'' && prev != '\\' && !inString) {
            inChar = !inChar;
            continue;
        }

        if (!inString && !inChar) {
            if (c == '(') {
                parenCount++;
            } else if (c == ')') {
                parenCount--;
                if (parenCount == 0) {
                    return i;
                }
            }
        }
    }

    return std::string::npos;
}

std::vector<std::string> CodeUtils::splitLines(const std::string& code) {
    std::vector<std::string> lines;
    std::istringstream stream(code);
    std::string line;

    while (std::getline(stream, line)) {
        lines.push_back(line);
    }

    return lines;
}

size_t CodeUtils::lineToPosition(const std::string& code, int lineNumber) {
    if (lineNumber < 1) return std::string::npos;

    int currentLine = 1;
    for (size_t i = 0; i < code.length(); ++i) {
        if (currentLine == lineNumber) {
            return i;
        }
        if (code[i] == '\n') {
            currentLine++;
        }
    }

    return std::string::npos;
}

int CodeUtils::positionToLine(const std::string& code, size_t position) {
    if (position >= code.length()) {
        return -1;
    }

    int lineNumber = 1;
    for (size_t i = 0; i < position && i < code.length(); ++i) {
        if (code[i] == '\n') {
            lineNumber++;
        }
    }

    return lineNumber;
}

bool CodeUtils::isInsideStringLiteral(const std::string& code, size_t position) {
    if (position >= code.length()) return false;

    bool inString = false;
    for (size_t i = 0; i < position; ++i) {
        char c = code[i];
        char prev = (i > 0) ? code[i-1] : '\0';

        if (c == '"' && prev != '\\') {
            inString = !inString;
        }
    }

    return inString;
}

bool CodeUtils::isInsideComment(const std::string& code, size_t position) {
    if (position >= code.length()) return false;

    bool inSingleComment = false;
    bool inMultiComment = false;

    for (size_t i = 0; i < position; ++i) {
        char c = code[i];
        char next = (i + 1 < code.length()) ? code[i+1] : '\0';
        char prev = (i > 0) ? code[i-1] : '\0';

        if (inSingleComment) {
            if (c == '\n') inSingleComment = false;
            continue;
        }

        if (inMultiComment) {
            if (c == '/' && prev == '*') {
                inMultiComment = false;
            }
            continue;
        }

        if (c == '/' && next == '/') {
            inSingleComment = true;
        } else if (c == '/' && next == '*') {
            inMultiComment = true;
        }
    }

    return inSingleComment || inMultiComment;
}

std::string CodeUtils::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) {
        return "";
    }

    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

int CodeUtils::countOccurrences(const std::string& text, const std::string& pattern) {
    if (pattern.empty()) return 0;

    int count = 0;
    size_t pos = 0;

    while ((pos = text.find(pattern, pos)) != std::string::npos) {
        count++;
        pos += pattern.length();
    }

    return count;
}

std::string CodeUtils::extractFunctionBody(const std::string& code, size_t functionStart) {
    size_t bracePos = code.find('{', functionStart);
    if (bracePos == std::string::npos) {
        return "";
    }

    size_t endBrace = findMatchingBrace(code, bracePos);
    if (endBrace == std::string::npos) {
        return "";
    }

    return code.substr(bracePos + 1, endBrace - bracePos - 1);
}
