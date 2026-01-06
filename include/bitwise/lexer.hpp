#pragma once

#include <vector>
#include <string>
#include "bitwise/token.hpp"

namespace bitwise::frontend {

class Lexer {
public:
    explicit Lexer(std::string source) : source_(std::move(source)) {}

    std::vector<Token> tokenize();

private:
    char advance();
    char peek() const;
    char peek_next() const;
    bool is_at_end() const;
    void scan_token();
    void scan_identifier();
    void scan_number();

    std::string source_;
    std::vector<Token> tokens_;
    int start_ = 0;
    int current_ = 0;
    int line_ = 1;
    int column_ = 1;
    int start_column_ = 1;
};

} // namespace bitwise::frontend
