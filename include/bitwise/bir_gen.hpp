#pragma once

#include "bitwise/ast.hpp"
#include "bitwise/bir.hpp"
#include "bitwise/diagnostic.hpp"

namespace bitwise::bir {

class BIRGenerator {
public:
    BIRGenerator(bitwise::common::DiagnosticEngine* diags = nullptr) : diags_(diags) {}
    std::unique_ptr<Module> generate(const bitwise::ast::Program& program);

private:
    std::unique_ptr<Function> lower_function(const bitwise::ast::FunctionDecl& func);
    void lower_statement(const bitwise::ast::Stmt& stmt, Function& bir_func);
    std::string lower_expression(const bitwise::ast::Expr& expr, Function& bir_func);

    int next_reg_ = 0;
    std::string new_reg() { return "%r" + std::to_string(next_reg_++); }
    bitwise::common::DiagnosticEngine* diags_ = nullptr;
};

} // namespace bitwise::bir
