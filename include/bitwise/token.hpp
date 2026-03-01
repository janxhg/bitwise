#pragma once

#include <string>
#include <string_view>

namespace bitwise::frontend {

enum class TokenType {
    // Keywords
    Let, Const, Fn, If, Else, Loop, While, Switch, Return, 
    Pub, Linear, Region, Using, Volatile, Packed, Async, Await, Interrupt,

    // Literals
    Identifier,
    IntegerLiteral,
    FloatLiteral,
    StringLiteral,

    // Operators
    Plus, Minus, Star, Slash, Equal, EqualEqual, Bang, BangEqual,
    Less, LessEqual, Greater, GreaterEqual, 
    Ampersand, Pipe, Tilde, LShift, RShift,
    Arrow, Caret, // ->, ^
    
    // Punctuation
    OpenParen, CloseParen, OpenBrace, CloseBrace, OpenBracket, CloseBracket,
    Semicolon, Colon, Comma, Dot, At,

    // Special
    EndOfFile,
    Invalid
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;

    Token(TokenType t, std::string l, int ln, int col) 
        : type(t), lexeme(std::move(l)), line(ln), column(col) {}
};

} // namespace bitwise::frontend
