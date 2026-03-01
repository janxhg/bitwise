#pragma once

#include <vector>
#include <string>
#include <memory>
#include "bitwise/token.hpp"

namespace bitwise::ast {

// Forward declarations
class Expr;
class Stmt;

// Base class for all AST Nodes
class Node {
public:
    virtual ~Node() = default;
};

// Expression base class
class Expr : public Node {};

// Statement base class
class Stmt : public Node {};

// --- Expressions ---

class IntLiteralExpr : public Expr {
public:
    int64_t value;
    explicit IntLiteralExpr(int64_t v) : value(v) {}
};

class BinaryExpr : public Expr {
public:
    std::unique_ptr<Expr> left;
    bitwise::frontend::Token op;
    std::unique_ptr<Expr> right;

    BinaryExpr(std::unique_ptr<Expr> l, bitwise::frontend::Token o, std::unique_ptr<Expr> r)
        : left(std::move(l)), op(std::move(o)), right(std::move(r)) {}
};

class UnaryExpr : public Expr {
public:
    bitwise::frontend::Token op;
    std::unique_ptr<Expr> operand;

    UnaryExpr(bitwise::frontend::Token o, std::unique_ptr<Expr> operand)
        : op(std::move(o)), operand(std::move(operand)) {}
};

class VariableExpr : public Expr {
public:
    std::string name;
    explicit VariableExpr(std::string n) : name(std::move(n)) {}
};

class MemberExpr : public Expr {
public:
    std::unique_ptr<Expr> object;
    std::string member_name;

    MemberExpr(std::unique_ptr<Expr> obj, std::string member)
        : object(std::move(obj)), member_name(std::move(member)) {}
};

// --- Statements ---

class VarDeclStmt : public Stmt {
public:
    std::string name;
    std::string type_name; 
    std::unique_ptr<Expr> initializer;
    bool is_linear = false;
    bool is_volatile = false;

    VarDeclStmt(std::string n, std::string t, std::unique_ptr<Expr> init, bool linear = false, bool v = false)
        : name(std::move(n)), type_name(std::move(t)), initializer(std::move(init)), is_linear(linear), is_volatile(v) {}
};

class BlockStmt : public Stmt {
public:
    std::vector<std::unique_ptr<Stmt>> statements;
};

class RegionStmt : public Stmt {
public:
    std::string region_name;
    std::unique_ptr<BlockStmt> body;

    RegionStmt(std::string name, std::unique_ptr<BlockStmt> b)
        : region_name(std::move(name)), body(std::move(b)) {}
};

class IfStmt : public Stmt {
public:
    std::unique_ptr<Expr> condition;
    std::unique_ptr<BlockStmt> then_branch;
    std::unique_ptr<BlockStmt> else_branch;

    IfStmt(std::unique_ptr<Expr> cond, std::unique_ptr<BlockStmt> then_b, std::unique_ptr<BlockStmt> else_b)
        : condition(std::move(cond)), then_branch(std::move(then_b)), else_branch(std::move(else_b)) {}
};

class ReturnStmt : public Stmt {
public:
    std::unique_ptr<Expr> value;
    explicit ReturnStmt(std::unique_ptr<Expr> v) : value(std::move(v)) {}
};

class AssignmentStmt : public Stmt {
public:
    std::unique_ptr<Expr> target;
    std::unique_ptr<Expr> value;

    AssignmentStmt(std::unique_ptr<Expr> t, std::unique_ptr<Expr> v)
        : target(std::move(t)), value(std::move(v)) {}
};

class ExpressionStmt : public Stmt {
public:
    std::unique_ptr<Expr> expression;

    explicit ExpressionStmt(std::unique_ptr<Expr> e)
        : expression(std::move(e)) {}
};

class WhileStmt : public Stmt {
public:
    std::unique_ptr<Expr> condition;
    std::unique_ptr<BlockStmt> body;

    WhileStmt(std::unique_ptr<Expr> cond, std::unique_ptr<BlockStmt> b)
        : condition(std::move(cond)), body(std::move(b)) {}
};

// --- Top Level ---

struct Parameter {
    std::string name;
    std::string type;
    bool is_linear = false;
};

class FunctionDecl : public Node {
public:
    std::string name;
    std::vector<Parameter> params;
    std::string return_type;
    std::unique_ptr<BlockStmt> body;

    FunctionDecl(std::string n, std::vector<Parameter> p, std::string rt, std::unique_ptr<BlockStmt> b)
        : name(std::move(n)), params(std::move(p)), return_type(std::move(rt)), body(std::move(b)) {}
};

struct StructField {
    std::string name;
    std::string type;
};

class StructDecl : public Node {
public:
    std::string name;
    std::vector<StructField> fields;
    bool is_packed = false;

    StructDecl(std::string n, std::vector<StructField> f, bool packed = false)
        : name(std::move(n)), fields(std::move(f)), is_packed(packed) {}
};

class Program : public Node {
public:
    std::vector<std::unique_ptr<StructDecl>> structs;
    std::vector<std::unique_ptr<FunctionDecl>> functions;
};

} // namespace bitwise::ast
