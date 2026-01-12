#include "../include/TestGenerator.h"
#include "../include/CodeUtils.h"
#include <regex>
#include <sstream>
#include <algorithm>
#include <iostream>

TestGenerator::TestGenerator() {}

TestGenerator::~TestGenerator() {}

std::string TestGenerator::generateTestCode(
    const std::string& sourceCode,
    const std::string& functionName,
    const std::vector<int>& inputSizes
) {

    FunctionSignature signature = parseFunctionSignature(sourceCode, functionName);

    if (signature.functionName.empty()) {
        return "
    }

    std::ostringstream code;

    code << "#include <iostream>\n";
    code << "#include <chrono>\n";
    code << "#include <cstdlib>\n";
    code << "#include <cstring>\n";
    code << "#include <ctime>\n\n";

    code << "
    code << sourceCode << "\n\n";

    code << "int main() {\n";
    code << "    srand(time(NULL));\n\n";

    for (int size : inputSizes) {
        code << "
        code << "    {\n";

        for (size_t i = 0; i < signature.parameters.size(); i++) {
            const Parameter& param = signature.parameters[i];
            std::string varName = param.name.empty() ? ("arg" + std::to_string(i)) : param.name;

            if (param.isArray || param.isPointer) {
                std::string baseType = extractBaseType(param.type);

                code << "        " << baseType << "* " << varName << ";\n";
                code << "        " << generateArrayInput(baseType, size, varName);
            } else if (param.type.find("int") != std::string::npos ||
                       param.type.find("long") != std::string::npos) {
                code << "        " << generateIntegerInput(param.type, varName);
            } else if (param.type.find("char") != std::string::npos &&
                       (param.isPointer || param.type.find("*") != std::string::npos)) {
                code << "        char* " << varName << ";\n";
                code << "        " << generateStringInput(size, varName);
            } else {
                code << "        " << param.type << " " << varName << " = " << size << ";\n";
            }
        }

        code << "\n        auto start = std::chrono::high_resolution_clock::now();\n";
        code << "        " << generateFunctionCall(signature, size);
        code << "        auto end = std::chrono::high_resolution_clock::now();\n\n";

        code << "        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);\n";
        code << "        std::cout << \"SIZE:\" << " << size << " << \",TIME:\" << duration.count() << std::endl;\n\n";

        code << generateCleanupCode(signature);

        code << "    }\n\n";
    }

    code << "    return 0;\n";
    code << "}\n";

    return code.str();
}

FunctionSignature TestGenerator::parseFunctionSignature(
    const std::string& code,
    const std::string& functionName
) {
    FunctionSignature signature;

    std::string pattern = "\\n\\s*(\\w+(?:\\s*\\*)?|\\w+\\s+\\w+)\\s+" +
                         functionName + "\\s*\\(([^)]*)\\)";
    std::regex funcRegex(pattern);
    std::smatch match;

    if (std::regex_search(code, match, funcRegex)) {
        signature.returnType = CodeUtils::trim(match[1].str());
        signature.functionName = functionName;

        std::string paramsStr = match[2].str();
        if (!paramsStr.empty() && paramsStr != "void") {
            std::istringstream paramStream(paramsStr);
            std::string paramStr;

            while (std::getline(paramStream, paramStr, ',')) {
                paramStr = CodeUtils::trim(paramStr);
                if (!paramStr.empty()) {
                    signature.parameters.push_back(parseParameter(paramStr));
                }
            }
        }
    }

    return signature;
}

Parameter TestGenerator::parseParameter(const std::string& paramStr) {
    Parameter param;

    param.isArray = (paramStr.find('[') != std::string::npos);
    param.isPointer = (paramStr.find('*') != std::string::npos);

    std::istringstream iss(paramStr);
    std::vector<std::string> tokens;
    std::string token;

    while (iss >> token) {

        size_t bracketPos = token.find('[');
        if (bracketPos != std::string::npos) {
            token = token.substr(0, bracketPos);
        }
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }

    if (tokens.size() >= 2) {

        param.name = tokens.back();
        param.type = "";
        for (size_t i = 0; i < tokens.size() - 1; i++) {
            if (i > 0) param.type += " ";
            param.type += tokens[i];
        }
    } else if (tokens.size() == 1) {

        param.type = tokens[0];
        param.name = "";
    }

    return param;
}

std::string TestGenerator::extractBaseType(const std::string& type) {
    std::string baseType = type;

    size_t starPos = baseType.find('*');
    if (starPos != std::string::npos) {
        baseType = baseType.substr(0, starPos);
    }

    size_t bracketPos = baseType.find('[');
    if (bracketPos != std::string::npos) {
        baseType = baseType.substr(0, bracketPos);
    }

    return CodeUtils::trim(baseType);
}

std::string TestGenerator::generateArrayInput(const std::string& type, int size, const std::string& varName) {
    std::ostringstream code;

    code << varName << " = new " << type << "[" << size << "];\n";
    code << "        for(int i = 0; i < " << size << "; i++) {\n";

    if (type == "int" || type == "long") {
        code << "            " << varName << "[i] = rand() % 1000;\n";
    } else if (type == "float" || type == "double") {
        code << "            " << varName << "[i] = (rand() % 1000) / 10.0;\n";
    } else if (type == "char") {
        code << "            " << varName << "[i] = 'a' + (rand() % 26);\n";
    } else {
        code << "            " << varName << "[i] = rand() % 100;\n";
    }

    code << "        }\n";

    return code.str();
}

std::string TestGenerator::generateIntegerInput(const std::string& type, const std::string& varName) {
    return type + " " + varName + " = rand() % 1000;\n";
}

std::string TestGenerator::generateStringInput(int size, const std::string& varName) {
    std::ostringstream code;

    code << varName << " = new char[" << (size + 1) << "];\n";
    code << "        for(int i = 0; i < " << size << "; i++) {\n";
    code << "            " << varName << "[i] = 'a' + (rand() % 26);\n";
    code << "        }\n";
    code << "        " << varName << "[" << size << "] = '\\0';\n";

    return code.str();
}

std::string TestGenerator::generateTimingCode() {
    return "auto start = std::chrono::high_resolution_clock::now();\n";
}

std::string TestGenerator::generateMemoryTrackingCode() {

    return "
}

std::string TestGenerator::generateFunctionCall(const FunctionSignature& signature, int size) {
    std::ostringstream code;

    std::string trimmedReturnType = CodeUtils::trim(signature.returnType);
    if (trimmedReturnType != "void" && !trimmedReturnType.empty()) {
        code << "auto result = ";
    }

    code << signature.functionName << "(";

    for (size_t i = 0; i < signature.parameters.size(); i++) {
        if (i > 0) code << ", ";

        const Parameter& param = signature.parameters[i];
        std::string varName = param.name.empty() ? ("arg" + std::to_string(i)) : param.name;

        bool isSizeParam = false;
        if (i > 0) {
            const Parameter& prevParam = signature.parameters[i - 1];
            if ((prevParam.isArray || prevParam.isPointer) &&
                (param.type.find("int") != std::string::npos) &&
                (param.name.find("size") != std::string::npos ||
                 param.name == "n" ||
                 param.name.find("len") != std::string::npos)) {
                isSizeParam = true;
            }
        }

        if (isSizeParam) {
            code << size;
        } else {
            code << varName;
        }
    }

    code << ");\n";

    return code.str();
}

std::string TestGenerator::generateCleanupCode(const FunctionSignature& signature) {
    std::ostringstream code;

    for (size_t i = 0; i < signature.parameters.size(); i++) {
        const Parameter& param = signature.parameters[i];
        std::string varName = param.name.empty() ? ("arg" + std::to_string(i)) : param.name;

        if (param.isArray || param.isPointer) {
            code << "        delete[] " << varName << ";\n";
        }
    }

    return code.str();
}
