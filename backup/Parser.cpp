#include "ComplexityTypes.h"
#include <iostream>
#include <fstream>
#include <regex>
#include <sstream>

class Parser {
private:
    std::string sourceCode;
    std::vector<std::string> tokens;
    
public:
    Parser() {}
    
    bool loadFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file " << filename << std::endl;
            return false;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        sourceCode = buffer.str();
        file.close();
        
        sourceCode = preprocessCode(sourceCode);
        return true;
    }
    
    void loadFromString(const std::string& code) {
        sourceCode = preprocessCode(code);
    }
    
    std::vector<std::string> tokenize() {
        tokens.clear();
        
        std::regex tokenRegex("\\w+|\\d+");
        std::sregex_iterator iter(sourceCode.begin(), sourceCode.end(), tokenRegex);
        std::sregex_iterator end;
        
        for (; iter != end; ++iter) {
            std::string token = iter->str();
            if (token != " " && token != "\t" && token != "\n") {
                tokens.push_back(token);
            }
        }
        return tokens;
    }
    
    std::string preprocessCode(const std::string& code) {
        std::string cleaned = code;
        
        std::regex singleComment("//.*");
        cleaned = std::regex_replace(cleaned, singleComment, "");
        
        std::regex multiComment("/\\*.*?\\*/");
        cleaned = std::regex_replace(cleaned, multiComment, "");
        
        std::regex preprocessor("#.*");
        cleaned = std::regex_replace(cleaned, preprocessor, "");
        
        return cleaned;
    }
    
    std::vector<std::string> extractFunctionSignatures() {
        std::vector<std::string> signatures;
        std::string cleanCode = preprocessCode(sourceCode);
        
        std::regex funcRegex("(int|void|char|float|double|long|short|unsigned)\\s+(\\w+)\\s*\\(");
        std::sregex_iterator iter(cleanCode.begin(), cleanCode.end(), funcRegex);
        std::sregex_iterator end;
        
        for (; iter != end; ++iter) {
            signatures.push_back(iter->str());
        }
        return signatures;
    }
    
    std::vector<std::string> extractFunctions() {
        std::vector<std::string> functions;
        std::regex funcRegex("\\w+\\s+\\w+\\s*\\(");
        std::sregex_iterator iter(sourceCode.begin(), sourceCode.end(), funcRegex);
        std::sregex_iterator end;
        
        for (; iter != end; ++iter) {
            functions.push_back(iter->str());
        }
        return functions;
    }
    
    std::string getSourceCode() const {
        return sourceCode;
    }
    
    std::vector<std::string> getTokens() const {
        return tokens;
    }
};