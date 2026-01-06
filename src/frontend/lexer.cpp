#include "bitwise/lexer.hpp"
#include <unordered_map>
#include <cctype>

namespace bitwise::frontend {

static const std::unordered_map<std::string, TokenType> keywords = {
    {"let", TokenType::Let},
    {"const", TokenType::Const},
    {"fn", TokenType::Fn},
    {"if", TokenType::If},
    {"else", TokenType::Else},
    {"loop", TokenType::Loop},
    {"while", TokenType::While},
    {"switch", TokenType::Switch},
    {"return", TokenType::Return},
    {"pub", TokenType::Pub},
    {"linear", TokenType::Linear},
    {"region", TokenType::Region},
    {"using", TokenType::Using},
    {"volatile", TokenType::Volatile},
    {"async", TokenType::Async},
    {"await", TokenType::Await},
    {"packed", TokenType::Packed},
    {"interrupt", TokenType::Interrupt}
};

std::vector<Token> Lexer::tokenize() {
    while (!is_at_end()) {
        start_ = current_;
        start_column_ = column_;
        scan_token();
    }

    tokens_.emplace_back(TokenType::EndOfFile, "", line_, column_);
    return tokens_;
}

void Lexer::scan_token() {
    char c = advance();
    switch (c) {
        case '(': tokens_.emplace_back(TokenType::OpenParen, "(", line_, start_column_); break;
        case ')': tokens_.emplace_back(TokenType::CloseParen, ")", line_, start_column_); break;
        case '{': tokens_.emplace_back(TokenType::OpenBrace, "{", line_, start_column_); break;
        case '}': tokens_.emplace_back(TokenType::CloseBrace, "}", line_, start_column_); break;
        case '[': tokens_.emplace_back(TokenType::OpenBracket, "[", line_, start_column_); break;
        case ']': tokens_.emplace_back(TokenType::CloseBracket, "]", line_, start_column_); break;
        case ';': tokens_.emplace_back(TokenType::Semicolon, ";", line_, start_column_); break;
        case ':': tokens_.emplace_back(TokenType::Colon, ":", line_, start_column_); break;
        case ',': tokens_.emplace_back(TokenType::Comma, ",", line_, start_column_); break;
        case '.': tokens_.emplace_back(TokenType::Dot, ".", line_, start_column_); break;
        case '@': tokens_.emplace_back(TokenType::At, "@", line_, start_column_); break;

        case '+': tokens_.emplace_back(TokenType::Plus, "+", line_, start_column_); break;
        case '<': 
            if (peek() == '=') {
                advance();
                tokens_.emplace_back(TokenType::LessEqual, "<=", line_, start_column_);
            } else {
                tokens_.emplace_back(TokenType::Less, "<", line_, start_column_);
            }
            break;
        case '>':
            if (peek() == '=') {
                advance();
                tokens_.emplace_back(TokenType::GreaterEqual, ">=", line_, start_column_);
            } else {
                tokens_.emplace_back(TokenType::Greater, ">", line_, start_column_);
            }
            break;
        case '-': 
            if (peek() == '>') {
                advance();
                tokens_.emplace_back(TokenType::Arrow, "->", line_, start_column_);
            } else {
                tokens_.emplace_back(TokenType::Minus, "-", line_, start_column_);
            }
            break;
        case '*': tokens_.emplace_back(TokenType::Star, "*", line_, start_column_); break;
        case '/': 
            if (peek() == '/') {
                // A comment goes until the end of the line
                while (peek() != '\n' && !is_at_end()) advance();
            } else {
                tokens_.emplace_back(TokenType::Slash, "/", line_, start_column_); 
            }
            break;

        case '=':
            if (peek() == '=') {
                advance();
                tokens_.emplace_back(TokenType::EqualEqual, "==", line_, start_column_);
            } else {
                tokens_.emplace_back(TokenType::Equal, "=", line_, start_column_);
            }
            break;
        case '!':
            if (peek() == '=') {
                advance();
                tokens_.emplace_back(TokenType::BangEqual, "!=", line_, start_column_);
            } else {
                tokens_.emplace_back(TokenType::Bang, "!", line_, start_column_);
            }
            break;

        case ' ':
        case '\r':
        case '\t':
            // Ignore whitespace
            break;

        case '\n':
            line_++;
            column_ = 1;
            break;

        default:
            if (std::isdigit(c)) {
                scan_number();
            } else if (std::isalpha(c) || c == '_') {
                scan_identifier();
            } else {
                // TODO: Report invalid character via diagnostic
            }
            break;
    }
}

void Lexer::scan_identifier() {
    while (std::isalnum(peek()) || peek() == '_') {
        advance();
    }

    std::string text = source_.substr(start_, current_ - start_);
    TokenType type = TokenType::Identifier;
    
    if (auto it = keywords.find(text); it != keywords.end()) {
        type = it->second;
    }

    tokens_.emplace_back(type, text, line_, start_column_);
}

void Lexer::scan_number() {
    bool is_hex = false;
    // previous() was shifted in scan_token, so source_[current_-1] is the first digit
    if (source_[current_ - 1] == '0' && (peek() == 'x' || peek() == 'X')) {
        advance(); // x
        is_hex = true;
        while (std::isxdigit(peek())) {
            advance();
        }
    } else {
        while (std::isdigit(peek())) {
            advance();
        }

        if (peek() == '.' && std::isdigit(peek_next())) {
            advance(); // consume '.'
            while (std::isdigit(peek())) {
                advance();
            }
            std::string text = source_.substr(start_, current_ - start_);
            tokens_.emplace_back(TokenType::FloatLiteral, text, line_, start_column_);
            return;
        }
    }

    std::string text = source_.substr(start_, current_ - start_);
    tokens_.emplace_back(TokenType::IntegerLiteral, text, line_, start_column_);
}

char Lexer::advance() {
    current_++;
    column_++;
    return source_[current_ - 1];
}

char Lexer::peek() const {
    if (is_at_end()) return '\0';
    return source_[current_];
}

char Lexer::peek_next() const {
    if (current_ + 1 >= source_.length()) return '\0';
    return source_[current_ + 1];
}

bool Lexer::is_at_end() const {
    return current_ >= source_.length();
}

} // namespace bitwise::frontend
