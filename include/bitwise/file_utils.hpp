#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <optional>

namespace bitwise::common {

inline std::optional<std::string> read_file(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return std::nullopt;

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

} // namespace bitwise::common
