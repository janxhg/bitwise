#pragma once

#include <iostream>
#include <string>
#include <vector>

namespace bitwise::common {

enum class DiagnosticLevel {
    Info,
    Warning,
    Error,
    Fatal
};

struct Diagnostic {
    DiagnosticLevel level;
    std::string message;
    int line;
    int column;
    std::string filename;
};

class DiagnosticEngine {
public:
    void report(DiagnosticLevel level, std::string_view filename, int line, int col, std::string_view message) {
        std::string level_str;
        switch (level) {
            case DiagnosticLevel::Info:    level_str = "[INFO]"; break;
            case DiagnosticLevel::Warning: level_str = "[WARNING]"; break;
            case DiagnosticLevel::Error:   level_str = "[ERROR]"; break;
            case DiagnosticLevel::Fatal:   level_str = "[FATAL]"; break;
        }

        std::cerr << level_str << " " << filename << ":" << line << ":" << col << ": " << message << "\n";
        
        if (level == DiagnosticLevel::Error || level == DiagnosticLevel::Fatal) {
            error_count_++;
        }
    }

    bool has_errors() const { return error_count_ > 0; }
    int error_count() const { return error_count_; }

private:
    int error_count_ = 0;
};

} // namespace bitwise::common
