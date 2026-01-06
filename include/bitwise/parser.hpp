#pragma once

#include <vector>
#include <optional>
#include "bitwise/token.hpp"
#include "bitwise/ast.hpp"
#include "bitwise/diagnostic.hpp"

namespace bitwise::frontend {

class Parser {
public:
    Parser(std::vector<Token> tokens, bitwise::common::DiagnosticEngine& diags)
        : tokens_(std::move(tokens)), diags_(diags) {}

    std::unique_ptr<bitwise::ast::Program> parse();

private:
    // Parsing methods
    std::unique_ptr<bitwise::ast::FunctionDecl> parse_function();
    std::unique_ptr<bitwise::ast::BlockStmt> parse_block();
    std::unique_ptr<bitwise::ast::Stmt> parse_statement();
    std::unique_ptr<bitwise::ast::IfStmt> parse_if();
    std::unique_ptr<bitwise::ast::StructDecl> parse_struct(bool packed);
    std::unique_ptr<bitwise::ast::Expr> parse_expression();
    std::unique_ptr<bitwise::ast::Expr> parse_binary(int min_precedence);
    std::unique_ptr<bitwise::ast::Expr> parse_unary();
    std::unique_ptr<bitwise::ast::Expr> parse_primary();

    // Helpers
    const Token& advance();
    const Token& peek() const;
    const Token& previous() const;
    bool check(TokenType type) const;
    bool match(const std::vector<TokenType>& types);
    const Token& consume(TokenType type, std::string_view message);
    bool is_at_end() const;

    std::vector<Token> tokens_;
    bitwise::common::DiagnosticEngine& diags_;
    int current_ = 0;
};

} // namespace bitwise::frontend
