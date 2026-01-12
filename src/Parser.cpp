#include "../include/Parser.h"
#include "../include/CodeUtils.h"
#include "../include/Exceptions.h"
#include <fstream>
#include <sstream>
#include <iostream>

const std::regex Parser::singleCommentRegex("
const std::regex Parser::multiCommentRegex("/\\*[\\s\\S]*?\\*/");
const std::regex Parser::preprocessorRegex("#.*");
const std::regex Parser::tokenRegex("\\w+|\\d+");
const std::regex Parser::functionRegex("([\\w:]+(?:<[^>]+>)?)\\s+(\\w+)\\s*\\(");

Parser::Parser() {}

Parser::~Parser() {}

bool Parser::loadFile(const std::string& filename) {
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw FileException("Cannot open file", filename);
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        originalCode = buffer.str();
        file.close();

        if (originalCode.empty()) {
            throw FileException("File is empty", filename);
        }

        sourceCode = preprocessCode(originalCode);
        return true;
    }
    catch (const FileException& e) {
        std::cerr << "Error: " << e.getFullMessage() << std::endl;
        return false;
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading file: " << e.what() << std::endl;
        return false;
    }
}

void Parser::loadFromString(const std::string& code) {
    originalCode = code;
    sourceCode = preprocessCode(code);
}

std::vector<std::string> Parser::tokenize() {
    tokens.clear();

    std::sregex_iterator iter(sourceCode.begin(), sourceCode.end(), tokenRegex);
    std::sregex_iterator end;

    tokens.reserve(1000);

    for (; iter != end; ++iter) {
        std::string token = iter->str();
        tokens.push_back(token);
    }

    return tokens;
}

const std::string& Parser::getSourceCode() const {
    return sourceCode;
}

const std::string& Parser::getOriginalCode() const {
    return originalCode;
}

const std::vector<std::string>& Parser::getTokens() const {
    return tokens;
}

std::vector<std::string> Parser::extractFunctionSignatures() {
    std::vector<std::string> signatures;
    std::sregex_iterator iter(sourceCode.begin(), sourceCode.end(), functionRegex);
    std::sregex_iterator end;

    for (; iter != end; ++iter) {
        size_t pos = iter->position() + iter->length();
        size_t bracePos = sourceCode.find('{', pos);
        size_t semicolonPos = sourceCode.find(';', pos);

        if (bracePos != std::string::npos &&
            (semicolonPos == std::string::npos || bracePos < semicolonPos)) {
            signatures.push_back(iter->str());
        }
    }

    return signatures;
}

std::vector<std::string> Parser::extractFunctionNames() {
    std::vector<std::string> names;
    std::sregex_iterator iter(sourceCode.begin(), sourceCode.end(), functionRegex);
    std::sregex_iterator end;

    static const std::vector<std::string> keywords = {
        "if", "while", "for", "switch", "return", "printf", "scanf",
        "sizeof", "typedef", "struct", "union", "enum", "case"
    };

    for (; iter != end; ++iter) {
        std::string funcName = (*iter)[2].str();

        bool isKeyword = false;
        for (const auto& keyword : keywords) {
            if (funcName == keyword) {
                isKeyword = true;
                break;
            }
        }

        if (!isKeyword) {
            size_t pos = iter->position();
            if (!CodeUtils::isInsideStringLiteral(originalCode, pos) &&
                !CodeUtils::isInsideComment(originalCode, pos)) {
                names.push_back(funcName);
            }
        }
    }

    return names;
}

bool Parser::isValid() const {
    return !sourceCode.empty() && sourceCode.find_first_not_of(" \t\n\r") != std::string::npos;
}

int Parser::getLineCount() const {
    return CodeUtils::countOccurrences(sourceCode, "\n") + 1;
}

std::string Parser::preprocessCode(const std::string& code) {
    std::string cleaned = code;

    cleaned = removeMultiLineComments(cleaned);
    cleaned = removeSingleLineComments(cleaned);
    cleaned = removePreprocessorDirectives(cleaned);

    return cleaned;
}

std::string Parser::removeSingleLineComments(const std::string& code) {
    return std::regex_replace(code, singleCommentRegex, "");
}

std::string Parser::removeMultiLineComments(const std::string& code) {
    return std::regex_replace(code, multiCommentRegex, "");
}

std::string Parser::removePreprocessorDirectives(const std::string& code) {
    return std::regex_replace(code, preprocessorRegex, "");
}
