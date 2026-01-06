#pragma once

#include <vector>
#include <string>
#include <memory>

namespace bitwise::bir {

// BIR focuses on three address code and basic blocks
enum class OpCode {
    LoadInt,
    StoreVar,
    LoadVar,
    Add,
    Sub,
    CmpLT,
    CmpGT,
    CmpEQ,
    CmpNE,
    Ret,
    Call,
    Br,      
    Jump,
    LoadMember,
    StoreMember,
    LoadVolatile,
    StoreVolatile
};

struct Instruction {
    OpCode opcode;
    std::vector<std::string> args;
    std::string result; // Target register or variable

    Instruction(OpCode op, std::vector<std::string> a, std::string res = "")
        : opcode(op), args(std::move(a)), result(std::move(res)) {}
};

struct BasicBlock {
    std::string label;
    std::vector<Instruction> instructions;

    explicit BasicBlock(std::string l) : label(std::move(l)) {}
};

struct Function {
    std::string name;
    std::vector<std::unique_ptr<BasicBlock>> blocks;

    explicit Function(std::string n) : name(std::move(n)) {}
};

struct Module {
    std::vector<std::unique_ptr<Function>> functions;
};

} // namespace bitwise::bir
