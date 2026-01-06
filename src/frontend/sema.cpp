#include "bitwise/sema.hpp"

namespace bitwise::sema {

SemanticAnalyzer::SemanticAnalyzer(bitwise::common::DiagnosticEngine& diags)
    : diags_(diags) {
    auto root = std::make_unique<Scope>();
    active_scope_ = root.get();
    scopes_.push_back(std::move(root));
}

void SemanticAnalyzer::analyze(bitwise::ast::Program& program) {
    for (auto& s : program.structs) {
        analyze_struct(*s);
    }
    for (auto& func : program.functions) {
        analyze_function(*func);
    }
}

void SemanticAnalyzer::analyze_function(bitwise::ast::FunctionDecl& func) {
    enter_scope();
    
    // In future: Add parameters to scope here
    
    if (func.body) {
        for (auto& stmt : func.body->statements) {
            if (stmt) {
                analyze_statement(*stmt);
            }
        }
    }
    
    exit_scope();
}

void SemanticAnalyzer::analyze_statement(bitwise::ast::Stmt& stmt) {
    if (auto* var_decl = dynamic_cast<bitwise::ast::VarDeclStmt*>(&stmt)) {
        analyze_expression(*var_decl->initializer);
        declare_symbol(var_decl->name, var_decl->type_name, 0, var_decl->is_linear, var_decl->is_volatile); 
    } 
    else if (auto* region_stmt = dynamic_cast<bitwise::ast::RegionStmt*>(&stmt)) {
        enter_scope();
        analyze_statement(*region_stmt->body);
        exit_scope();
    }
    else if (auto* if_stmt = dynamic_cast<bitwise::ast::IfStmt*>(&stmt)) {
        analyze_expression(*if_stmt->condition);
        analyze_statement(*if_stmt->then_branch);
        if (if_stmt->else_branch) {
            analyze_statement(*if_stmt->else_branch);
        }
    }
    else if (auto* block_stmt = dynamic_cast<bitwise::ast::BlockStmt*>(&stmt)) {
        enter_scope();
        for (auto& s : block_stmt->statements) {
            if (s) analyze_statement(*s);
        }
        exit_scope();
    }
    else if (auto* ret_stmt = dynamic_cast<bitwise::ast::ReturnStmt*>(&stmt)) {
        if (ret_stmt->value) {
            analyze_expression(*ret_stmt->value);
        }
    }
    else if (auto* assign_stmt = dynamic_cast<bitwise::ast::AssignmentStmt*>(&stmt)) {
        analyze_expression(*assign_stmt->target);
        analyze_expression(*assign_stmt->value);
    }
    else if (auto* while_stmt = dynamic_cast<bitwise::ast::WhileStmt*>(&stmt)) {
        analyze_expression(*while_stmt->condition);
        enter_scope();
        for (auto& s : while_stmt->body->statements) {
            if (s) analyze_statement(*s);
        }
        exit_scope();
    }
}

void SemanticAnalyzer::analyze_expression(bitwise::ast::Expr& expr) {
    if (auto* member_expr = dynamic_cast<bitwise::ast::MemberExpr*>(&expr)) {
        analyze_expression(*member_expr->object);
        // In a real compiler we'd check if member_name exists in the object's type
    }
    else if (auto* binary_expr = dynamic_cast<bitwise::ast::BinaryExpr*>(&expr)) {
        analyze_expression(*binary_expr->left);
        analyze_expression(*binary_expr->right);
    }
    else if (auto* var_expr = dynamic_cast<bitwise::ast::VariableExpr*>(&expr)) {
        Symbol* sym = lookup_symbol(var_expr->name);
        if (!sym) {
            diags_.report(bitwise::common::DiagnosticLevel::Error, "sema", 0, 0, 
                "Undefined variable: '" + var_expr->name + "'");
            return;
        }

        if (sym->state == OwnershipState::Moved) {
            diags_.report(bitwise::common::DiagnosticLevel::Error, "sema", 0, 0, 
                "Use after move: variable '" + var_expr->name + "' was previously moved.");
        }

        // In Bitwise, access usually moves ownership if it's a linear type
        if (sym->is_linear) {
            sym->state = OwnershipState::Moved;
        }
    }
    // In future: Binary expressions, assignment moves ownership, etc.
}

// Scope management

void SemanticAnalyzer::analyze_struct(bitwise::ast::StructDecl& s) {
    // Register struct type in a global type table (omitted here for brevity)
}

void SemanticAnalyzer::enter_scope() {
    auto new_scope = std::make_unique<Scope>(active_scope_);
    active_scope_ = new_scope.get();
    scopes_.push_back(std::move(new_scope));
}

void SemanticAnalyzer::exit_scope() {
    if (active_scope_) {
        // Enforce linear types: check for leaks
        for (const auto& [name, sym] : active_scope_->symbols) {
            if (sym.is_linear && sym.state == OwnershipState::Available) {
                diags_.report(bitwise::common::DiagnosticLevel::Error, "sema", sym.line_defined, 0,
                    "Linear type leak: variable '" + name + "' was never consumed.");
            }
        }

        if (active_scope_->parent) {
            active_scope_ = active_scope_->parent;
        }
    }
}

void SemanticAnalyzer::declare_symbol(const std::string& name, const std::string& type, int line, bool is_linear, bool is_volatile) {
    if (active_scope_->symbols.find(name) != active_scope_->symbols.end()) {
        diags_.report(bitwise::common::DiagnosticLevel::Error, "sema", line, 0, 
            "Redefinition of variable '" + name + "'");
        return;
    }
    active_scope_->symbols.emplace(name, Symbol(name, type, line, is_linear, is_volatile));
}

Symbol* SemanticAnalyzer::lookup_symbol(const std::string& name) {
    Scope* current = active_scope_;
    while (current) {
        auto it = current->symbols.find(name);
        if (it != current->symbols.end()) {
            return &it->second;
        }
        current = current->parent;
    }
    return nullptr;
}

} // namespace bitwise::sema
