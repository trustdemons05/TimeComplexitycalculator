#ifndef PATH_UTILS_H
#define PATH_UTILS_H

#include <string>

class PathUtils {
public:

    static char getPathSeparator() {
#ifdef _WIN32
        return '\\';
#else
        return '/';
#endif
    }

    static std::string getExecutableExtension() {
#ifdef _WIN32
        return ".exe";
#else
        return "";
#endif
    }

    static std::string join(const std::string& base, const std::string& component) {
        if (base.empty()) return component;
        if (component.empty()) return base;

        char sep = getPathSeparator();

        if (base[base.length() - 1] == '/' || base[base.length() - 1] == '\\') {
            return base + component;
        }

        return base + sep + component;
    }

    static std::string normalize(const std::string& path) {
        std::string result = path;
        char correctSep = getPathSeparator();
        char wrongSep = (correctSep == '\\') ? '/' : '\\';

        for (size_t i = 0; i < result.length(); i++) {
            if (result[i] == wrongSep) {
                result[i] = correctSep;
            }
        }

        return result;
    }

    static std::string getDirectory(const std::string& path) {
        size_t lastSep = path.find_last_of("/\\");
        if (lastSep == std::string::npos) {
            return ".";
        }
        return path.substr(0, lastSep);
    }

    static std::string getFilename(const std::string& path) {
        size_t lastSep = path.find_last_of("/\\");
        if (lastSep == std::string::npos) {
            return path;
        }
        return path.substr(lastSep + 1);
    }

    static std::string getExtension(const std::string& path) {
        std::string filename = getFilename(path);
        size_t lastDot = filename.find_last_of('.');
        if (lastDot == std::string::npos || lastDot == 0) {
            return "";
        }
        return filename.substr(lastDot);
    }

    static std::string removeExtension(const std::string& path) {
        size_t lastDot = path.find_last_of('.');
        size_t lastSep = path.find_last_of("/\\");

        if (lastDot != std::string::npos &&
            (lastSep == std::string::npos || lastDot > lastSep)) {
            return path.substr(0, lastDot);
        }
        return path;
    }

    static std::string toForwardSlashes(const std::string& path) {
        std::string result = path;
        for (size_t i = 0; i < result.length(); i++) {
            if (result[i] == '\\') {
                result[i] = '/';
            }
        }
        return result;
    }

    static bool isAbsolute(const std::string& path) {
        if (path.empty()) return false;

#ifdef _WIN32

        if (path.length() >= 2) {
            if (path[1] == ':') return true;
            if (path[0] == '\\' && path[1] == '\\') return true;
        }
        return false;
#else

        return path[0] == '/';
#endif
    }

    static std::string getTempDirectory() {
#ifdef _WIN32
        const char* temp = getenv("TEMP");
        if (temp) return std::string(temp);
        temp = getenv("TMP");
        if (temp) return std::string(temp);
        return "C:\\Temp";
#else
        const char* temp = getenv("TMPDIR");
        if (temp) return std::string(temp);
        return "/tmp";
#endif
    }

private:
    PathUtils() = delete;
};

#endif
