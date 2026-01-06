#include "bitwise/parser.hpp"
#include <stdexcept>

namespace bitwise::frontend {

std::unique_ptr<bitwise::ast::Program> Parser::parse() {
    auto program = std::make_unique<bitwise::ast::Program>();
    while (!is_at_end()) {
        if (check(TokenType::Fn)) {
            program->functions.push_back(parse_function());
        } else if (match({TokenType::Packed})) {
            program->structs.push_back(parse_struct(true));
        } else if (check(TokenType::Switch)) { // Switch is used for struct in this simple hack
             program->structs.push_back(parse_struct(false));
        } else if (match({TokenType::Identifier}) && previous().lexeme == "struct") {
             program->structs.push_back(parse_struct(false));
        } else {
            advance(); 
        }
    }
    return program;
}

std::unique_ptr<bitwise::ast::FunctionDecl> Parser::parse_function() {
    consume(TokenType::Fn, "Expect 'fn' at start of function declaration.");
    const Token& name = consume(TokenType::Identifier, "Expect function name.");

    consume(TokenType::OpenParen, "Expect '(' after function name.");
    std::vector<bitwise::ast::Parameter> params;
    // Parameter parsing logic here in future
    consume(TokenType::CloseParen, "Expect ')' after parameters.");

    std::string return_type = "void";
    if (match({TokenType::Arrow})) {
        return_type = consume(TokenType::Identifier, "Expect return type after '->'.").lexeme;
    }

    auto body = parse_block();
    return std::make_unique<bitwise::ast::FunctionDecl>(name.lexeme, std::move(params), return_type, std::move(body));
}

std::unique_ptr<bitwise::ast::BlockStmt> Parser::parse_block() {
    consume(TokenType::OpenBrace, "Expect '{' to start block.");
    auto block = std::make_unique<bitwise::ast::BlockStmt>();

    while (!check(TokenType::CloseBrace) && !is_at_end()) {
        block->statements.push_back(parse_statement());
    }

    consume(TokenType::CloseBrace, "Expect '}' after block.");
    return block;
}

std::unique_ptr<bitwise::ast::Stmt> Parser::parse_statement() {
    bool is_linear = false;
    bool is_volatile = false;
    
    while (true) {
        if (match({TokenType::Linear})) is_linear = true;
        else if (match({TokenType::Volatile})) is_volatile = true;
        else break;
    }

    if (match({TokenType::Let})) {
        const Token& name = consume(TokenType::Identifier, "Expect variable name.");
        consume(TokenType::Equal, "Expect '=' for initialization.");
        auto init = parse_expression();
        consume(TokenType::Semicolon, "Expect ';' after variable declaration.");
        return std::make_unique<bitwise::ast::VarDeclStmt>(name.lexeme, "auto", std::move(init), is_linear, is_volatile);
    }

    if (match({TokenType::Region})) {
        const Token& name = consume(TokenType::Identifier, "Expect region name.");
        auto body = parse_block();
        return std::make_unique<bitwise::ast::RegionStmt>(name.lexeme, std::move(body));
    }

    if (match({TokenType::If})) {
        return parse_if();
    }

    if (match({TokenType::While})) {
        auto condition = parse_expression();
        auto body = parse_block();
        return std::make_unique<bitwise::ast::WhileStmt>(std::move(condition), std::move(body));
    }

    if (match({TokenType::Return})) {
        auto value = parse_expression();
        consume(TokenType::Semicolon, "Expect ';' after return.");
        return std::make_unique<bitwise::ast::ReturnStmt>(std::move(value));
    }

    // Handle expressions as statements (assignments)
    if (auto lhs = parse_expression()) {
        if (match({TokenType::Equal})) {
            auto rhs = parse_expression();
            consume(TokenType::Semicolon, "Expect ';' after assignment.");
            return std::make_unique<bitwise::ast::AssignmentStmt>(std::move(lhs), std::move(rhs));
        }
        consume(TokenType::Semicolon, "Expect ';' after expression.");
        return nullptr;
    }

    diags_.report(bitwise::common::DiagnosticLevel::Error, "unknown", peek().line, peek().column, "Unexpected token in statement: " + std::string(peek().lexeme));
    advance();
    return nullptr;
}

std::unique_ptr<bitwise::ast::IfStmt> Parser::parse_if() {
    auto condition = parse_expression();
    auto then_branch = parse_block();
    std::unique_ptr<bitwise::ast::BlockStmt> else_branch = nullptr;

    if (match({TokenType::Else})) {
        else_branch = parse_block();
    }

    return std::make_unique<bitwise::ast::IfStmt>(std::move(condition), std::move(then_branch), std::move(else_branch));
}

std::unique_ptr<bitwise::ast::Expr> Parser::parse_expression() {
    return parse_binary(0);
}

int get_precedence(TokenType type) {
    switch (type) {
        case TokenType::Plus: case TokenType::Minus: return 10;
        case TokenType::Star: case TokenType::Slash: return 20;
        case TokenType::EqualEqual: case TokenType::BangEqual: return 5;
        case TokenType::Less: case TokenType::Greater: return 5;
        default: return -1;
    }
}

std::unique_ptr<bitwise::ast::Expr> Parser::parse_binary(int min_precedence) {
    auto left = parse_unary();

    while (true) {
        int precedence = get_precedence(peek().type);
        if (precedence < min_precedence) break;

        Token op = advance();
        auto right = parse_binary(precedence + 1);
        left = std::make_unique<bitwise::ast::BinaryExpr>(std::move(left), op, std::move(right));
    }

    return left;
}

std::unique_ptr<bitwise::ast::Expr> Parser::parse_unary() {
    // For now, only primary expressions. Unary ops (!, -) can be added here.
    return parse_primary();
}

std::unique_ptr<bitwise::ast::Expr> Parser::parse_primary() {
    std::unique_ptr<bitwise::ast::Expr> expr = nullptr;

    if (match({TokenType::IntegerLiteral})) {
        expr = std::make_unique<bitwise::ast::IntLiteralExpr>(std::stoll(previous().lexeme, nullptr, 0));
    } else if (match({TokenType::Identifier})) {
        expr = std::make_unique<bitwise::ast::VariableExpr>(previous().lexeme);
    } else if (match({TokenType::OpenParen})) {
        expr = parse_expression();
        consume(TokenType::CloseParen, "Expect ')' after expression.");
    } else {
        throw std::runtime_error("Expect expression.");
    }

    // Handle member access
    while (match({TokenType::Dot})) {
        const Token& name = consume(TokenType::Identifier, "Expect member name after '.'.");
        expr = std::make_unique<bitwise::ast::MemberExpr>(std::move(expr), name.lexeme);
    }

    return expr;
}

std::unique_ptr<bitwise::ast::StructDecl> Parser::parse_struct(bool packed) {
    if (!packed) {
        // If not already consumed by match({TokenType::Packed})
        if (peek().lexeme == "struct") advance();
    } else {
        consume(TokenType::Identifier, "Expect 'struct' after 'packed'.");
    }

    const Token& name = consume(TokenType::Identifier, "Expect struct name.");
    consume(TokenType::OpenBrace, "Expect '{' before struct body.");

    std::vector<bitwise::ast::StructField> fields;
    while (!check(TokenType::CloseBrace) && !is_at_end()) {
        const Token& field_name = consume(TokenType::Identifier, "Expect field name.");
        consume(TokenType::Colon, "Expect ':' after field name.");
        const Token& field_type = consume(TokenType::Identifier, "Expect field type.");
        consume(TokenType::Semicolon, "Expect ';' after field declaration.");
        fields.push_back({field_name.lexeme, field_type.lexeme});
    }

    consume(TokenType::CloseBrace, "Expect '}' after struct body.");
    return std::make_unique<bitwise::ast::StructDecl>(name.lexeme, std::move(fields), packed);
}

// Helpers

const Token& Parser::advance() {
    if (!is_at_end()) current_++;
    return previous();
}

const Token& Parser::peek() const {
    return tokens_[current_];
}

const Token& Parser::previous() const {
    return tokens_[current_ - 1];
}

bool Parser::check(TokenType type) const {
    if (is_at_end()) return false;
    return peek().type == type;
}

bool Parser::match(const std::vector<TokenType>& types) {
    for (auto type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

const Token& Parser::consume(TokenType type, std::string_view message) {
    if (check(type)) return advance();
    
    diags_.report(bitwise::common::DiagnosticLevel::Error, "unknown", peek().line, peek().column, message);
    throw std::runtime_error(std::string(message));
}

bool Parser::is_at_end() const {
    return peek().type == TokenType::EndOfFile;
}

} // namespace bitwise::frontend
