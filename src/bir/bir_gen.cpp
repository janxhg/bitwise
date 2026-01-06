#include "bitwise/bir_gen.hpp"

namespace bitwise::bir {

std::unique_ptr<Module> BIRGenerator::generate(const bitwise::ast::Program& program) {
    auto module = std::make_unique<Module>();
    for (const auto& func : program.functions) {
        module->functions.push_back(lower_function(*func));
    }
    return module;
}

std::unique_ptr<Function> BIRGenerator::lower_function(const bitwise::ast::FunctionDecl& func) {
    auto bir_func = std::make_unique<Function>(func.name);
    auto entry_block = std::make_unique<BasicBlock>("entry");

    // In a real implementation we'd manage blocks better, 
    // for now we put everything in one entry block for simple functions
    Function& ref = *bir_func;
    ref.blocks.push_back(std::move(entry_block));

    if (func.body) {
        for (const auto& stmt : func.body->statements) {
            lower_statement(*stmt, ref);
        }
    }

    return bir_func;
}

void BIRGenerator::lower_statement(const bitwise::ast::Stmt& stmt, Function& bir_func) {
    auto& block = *bir_func.blocks.back();

    if (auto* var_decl = dynamic_cast<const bitwise::ast::VarDeclStmt*>(&stmt)) {
        std::string reg = lower_expression(*var_decl->initializer, bir_func);
        OpCode op = var_decl->is_volatile ? OpCode::StoreVolatile : OpCode::StoreVar;
        block.instructions.emplace_back(op, std::vector<std::string>{reg}, var_decl->name);
    } 
    else if (auto* region_stmt = dynamic_cast<const bitwise::ast::RegionStmt*>(&stmt)) {
        // For now, BIR just lowers the region body into the current function flow
        for (const auto& s : region_stmt->body->statements) {
            lower_statement(*s, bir_func);
        }
    }
    else if (auto* if_stmt = dynamic_cast<const bitwise::ast::IfStmt*>(&stmt)) {
        std::string cond_reg = lower_expression(*if_stmt->condition, bir_func);
        
        auto then_block = std::make_unique<BasicBlock>("then_" + std::to_string(next_reg_));
        auto else_block = (if_stmt->else_branch) ? std::make_unique<BasicBlock>("else_" + std::to_string(next_reg_)) : nullptr;
        auto merge_block = std::make_unique<BasicBlock>("merge_" + std::to_string(next_reg_));

        // Branching instruction
        bir_func.blocks.back()->instructions.emplace_back(OpCode::Br, 
            std::vector<std::string>{cond_reg, then_block->label, (else_block ? else_block->label : merge_block->label)});

        // Lower Then branch
        bir_func.blocks.push_back(std::move(then_block));
        for (const auto& s : if_stmt->then_branch->statements) lower_statement(*s, bir_func);
        bir_func.blocks.back()->instructions.emplace_back(OpCode::Jump, std::vector<std::string>{merge_block->label});

        // Lower Else branch
        if (else_block) {
            bir_func.blocks.push_back(std::move(else_block));
            for (const auto& s : if_stmt->else_branch->statements) lower_statement(*s, bir_func);
            bir_func.blocks.back()->instructions.emplace_back(OpCode::Jump, std::vector<std::string>{merge_block->label});
        }

        bir_func.blocks.push_back(std::move(merge_block));
    }
    else if (auto* ret_stmt = dynamic_cast<const bitwise::ast::ReturnStmt*>(&stmt)) {
        if (ret_stmt->value) {
            std::string reg = lower_expression(*ret_stmt->value, bir_func);
            block.instructions.emplace_back(OpCode::Ret, std::vector<std::string>{reg});
        } else {
            block.instructions.emplace_back(OpCode::Ret, std::vector<std::string>{});
        }
    }
    else if (auto* assign_stmt = dynamic_cast<const bitwise::ast::AssignmentStmt*>(&stmt)) {
        std::string rhs = lower_expression(*assign_stmt->value, bir_func);
        if (auto* var_expr = dynamic_cast<const bitwise::ast::VariableExpr*>(assign_stmt->target.get())) {
            block.instructions.emplace_back(OpCode::StoreVar, std::vector<std::string>{rhs}, var_expr->name);
        } else if (auto* member_expr = dynamic_cast<const bitwise::ast::MemberExpr*>(assign_stmt->target.get())) {
             std::string obj_reg = lower_expression(*member_expr->object, bir_func);
             block.instructions.emplace_back(OpCode::StoreMember, std::vector<std::string>{rhs, obj_reg, member_expr->member_name});
        }
    }
    else if (auto* while_stmt = dynamic_cast<const bitwise::ast::WhileStmt*>(&stmt)) {
        auto cond_label = "while_cond_" + std::to_string(next_reg_++);
        auto body_label = "while_body_" + std::to_string(next_reg_++);
        auto merge_label = "while_merge_" + std::to_string(next_reg_++);

        bir_func.blocks.back()->instructions.emplace_back(OpCode::Jump, std::vector<std::string>{cond_label});

        // Condition block
        auto cond_block = std::make_unique<BasicBlock>(cond_label);
        bir_func.blocks.push_back(std::move(cond_block));
        std::string cond_reg = lower_expression(*while_stmt->condition, bir_func);
        bir_func.blocks.back()->instructions.emplace_back(OpCode::Br, 
            std::vector<std::string>{cond_reg, body_label, merge_label});

        // Body block
        auto body_block = std::make_unique<BasicBlock>(body_label);
        bir_func.blocks.push_back(std::move(body_block));
        for (const auto& s : while_stmt->body->statements) lower_statement(*s, bir_func);
        bir_func.blocks.back()->instructions.emplace_back(OpCode::Jump, std::vector<std::string>{cond_label});

        // Merge block
        auto merge_block = std::make_unique<BasicBlock>(merge_label);
        bir_func.blocks.push_back(std::move(merge_block));
    }
}

std::string BIRGenerator::lower_expression(const bitwise::ast::Expr& expr, Function& bir_func) {
    auto& block = *bir_func.blocks.back();

    if (auto* binary_expr = dynamic_cast<const bitwise::ast::BinaryExpr*>(&expr)) {
        std::string lhs = lower_expression(*binary_expr->left, bir_func);
        std::string rhs = lower_expression(*binary_expr->right, bir_func);
        std::string res = new_reg();
        OpCode op = OpCode::Add; // Default
        
        switch (binary_expr->op.type) {
            case bitwise::frontend::TokenType::Plus: op = OpCode::Add; break;
            case bitwise::frontend::TokenType::Minus: op = OpCode::Sub; break;
            case bitwise::frontend::TokenType::Less: op = OpCode::CmpLT; break;
            case bitwise::frontend::TokenType::Greater: op = OpCode::CmpGT; break;
            case bitwise::frontend::TokenType::EqualEqual: op = OpCode::CmpEQ; break;
            case bitwise::frontend::TokenType::BangEqual: op = OpCode::CmpNE; break;
            default: break; 
        }
        
        bir_func.blocks.back()->instructions.emplace_back(op, std::vector<std::string>{lhs, rhs}, res);
        return res;
    }
    
    if (auto* int_lit = dynamic_cast<const bitwise::ast::IntLiteralExpr*>(&expr)) {
        std::string reg = new_reg();
        block.instructions.emplace_back(OpCode::LoadInt, std::vector<std::string>{std::to_string(int_lit->value)}, reg);
        return reg;
    }
    
    if (auto* member_expr = dynamic_cast<const bitwise::ast::MemberExpr*>(&expr)) {
        std::string obj_reg = lower_expression(*member_expr->object, bir_func);
        std::string res = new_reg();
        block.instructions.emplace_back(OpCode::LoadMember, std::vector<std::string>{obj_reg, member_expr->member_name}, res);
        return res;
    }
    
    if (auto* var_expr = dynamic_cast<const bitwise::ast::VariableExpr*>(&expr)) {
        std::string reg = new_reg();
        block.instructions.emplace_back(OpCode::LoadVar, std::vector<std::string>{var_expr->name}, reg);
        return reg;
    }

    return ""; // Should handle binary expressions, etc.
}

} // namespace bitwise::bir
