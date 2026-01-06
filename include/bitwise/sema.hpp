#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "bitwise/ast.hpp"
#include "bitwise/diagnostic.hpp"

namespace bitwise::sema {

// The state of a value in memory
enum class OwnershipState {
    Available, // Valid to use
    Moved,     // Ownership transferred, cannot use
    Borrowed,  // Temporary view active
    Consumed   // Linear type fulfilled
};

struct Symbol {
    std::string name;
    std::string type;
    OwnershipState state;
    bool is_linear = false;
    bool is_volatile = false;
    int line_defined;

    Symbol(std::string n, std::string t, int line, bool linear = false, bool v = false)
        : name(std::move(n)), type(std::move(t)), state(OwnershipState::Available), is_linear(linear), is_volatile(v), line_defined(line) {}
};

class Scope {
public:
    std::unordered_map<std::string, Symbol> symbols;
    Scope* parent = nullptr;

    explicit Scope(Scope* p = nullptr) : parent(p) {}
};

class SemanticAnalyzer {
public:
    explicit SemanticAnalyzer(bitwise::common::DiagnosticEngine& diags);

    void analyze(bitwise::ast::Program& program);

private:
    void analyze_function(bitwise::ast::FunctionDecl& func);
    void analyze_statement(bitwise::ast::Stmt& stmt);
    void analyze_expression(bitwise::ast::Expr& expr);
    void analyze_struct(bitwise::ast::StructDecl& s);

    void enter_scope();
    void exit_scope();

    void declare_symbol(const std::string& name, const std::string& type, int line, bool is_linear = false, bool is_volatile = false);
    Symbol* lookup_symbol(const std::string& name);

    bitwise::common::DiagnosticEngine& diags_;
    std::vector<std::unique_ptr<Scope>> scopes_;
    Scope* active_scope_ = nullptr; 
};

} // namespace bitwise::sema
