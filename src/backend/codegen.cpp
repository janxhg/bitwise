#include "bitwise/codegen.hpp"

namespace bitwise::backend {

void NanoBackend::emit(const bitwise::bir::Module& module, std::ostream& out) {
    out << "; --- Bitwise Nano Backend: Target Assembly ---\n";
    out << "; Optimization Level: Minimalist\n\n";

    for (const auto& func : module.functions) {
        emit_function(*func, out);
    }
}

void NanoBackend::emit_function(const bitwise::bir::Function& func, std::ostream& out) {
    out << ".global " << func.name << "\n";
    out << func.name << ":\n";

    for (const auto& block : func.blocks) {
        if (block->label != "entry") {
            out << block->label << ":\n";
        }

        for (const auto& inst : block->instructions) {
            out << "  ";
            emit_instruction(inst, out);
        }
    }
    out << "\n";
}

void NanoBackend::emit_instruction(const bitwise::bir::Instruction& inst, std::ostream& out) {
    switch (inst.opcode) {
        case bitwise::bir::OpCode::LoadInt:
            out << "MOV " << inst.result << ", #" << inst.args[0] << "\n";
            break;
        case bitwise::bir::OpCode::LoadVar:
            out << "LDR " << inst.result << ", [" << inst.args[0] << "]\n";
            break;
        case bitwise::bir::OpCode::StoreVar:
            out << "STR " << inst.args[0] << ", [" << inst.result << "]\n";
            break;
        case bitwise::bir::OpCode::Add:
            out << "ADD " << inst.result << ", " << inst.args[0] << ", " << inst.args[1] << "\n";
            break;
        case bitwise::bir::OpCode::Sub:
            out << "SUB " << inst.result << ", " << inst.args[0] << ", " << inst.args[1] << "\n";
            break;
        case bitwise::bir::OpCode::Mul:
            out << "MUL " << inst.result << ", " << inst.args[0] << ", " << inst.args[1] << "\n";
            break;
        case bitwise::bir::OpCode::Div:
            out << "DIV " << inst.result << ", " << inst.args[0] << ", " << inst.args[1] << "\n";
            break;
        case bitwise::bir::OpCode::Mod:
            out << "MOD " << inst.result << ", " << inst.args[0] << ", " << inst.args[1] << "\n";
            break;
        case bitwise::bir::OpCode::BitAnd:
            out << "AND " << inst.result << ", " << inst.args[0] << ", " << inst.args[1] << "\n";
            break;
        case bitwise::bir::OpCode::BitOr:
            out << "ORR " << inst.result << ", " << inst.args[0] << ", " << inst.args[1] << "\n";
            break;
        case bitwise::bir::OpCode::BitXor:
            out << "EOR " << inst.result << ", " << inst.args[0] << ", " << inst.args[1] << "\n";
            break;
        case bitwise::bir::OpCode::BitNot:
            out << "MVN " << inst.result << ", " << inst.args[0] << "\n";
            break;
        case bitwise::bir::OpCode::Neg:
            out << "NEG " << inst.result << ", " << inst.args[0] << "\n";
            break;
        case bitwise::bir::OpCode::Not:
            out << "NOT " << inst.result << ", " << inst.args[0] << "\n";
            break;
        case bitwise::bir::OpCode::ShiftLeft:
            out << "LSL " << inst.result << ", " << inst.args[0] << ", " << inst.args[1] << "\n";
            break;
        case bitwise::bir::OpCode::ShiftRight:
            out << "LSR " << inst.result << ", " << inst.args[0] << ", " << inst.args[1] << "\n";
            break;
        case bitwise::bir::OpCode::CmpLT:
            out << "SLT " << inst.result << ", " << inst.args[0] << ", " << inst.args[1] << "\n";
            break;
        case bitwise::bir::OpCode::CmpGT:
            out << "SGT " << inst.result << ", " << inst.args[0] << ", " << inst.args[1] << "\n";
            break;
        case bitwise::bir::OpCode::CmpLE:
            out << "SLE " << inst.result << ", " << inst.args[0] << ", " << inst.args[1] << "\n";
            break;
        case bitwise::bir::OpCode::CmpGE:
            out << "SGE " << inst.result << ", " << inst.args[0] << ", " << inst.args[1] << "\n";
            break;
        case bitwise::bir::OpCode::CmpEQ:
            out << "SEQ " << inst.result << ", " << inst.args[0] << ", " << inst.args[1] << "\n";
            break;
        case bitwise::bir::OpCode::CmpNE:
            out << "SNE " << inst.result << ", " << inst.args[0] << ", " << inst.args[1] << "\n";
            break;
        case bitwise::bir::OpCode::Br:
            out << "CMP " << inst.args[0] << ", #0\n";
            out << "BNE " << inst.args[1] << "\n";
            out << "B " << inst.args[2] << "\n";
            break;
        case bitwise::bir::OpCode::Jump:
            out << "B " << inst.args[0] << "\n";
            break;
        case bitwise::bir::OpCode::LoadMember:
            out << "LDR " << inst.result << ", [" << inst.args[0] << ", #" << inst.args[1] << "]\n";
            break;
        case bitwise::bir::OpCode::StoreVolatile:
            out << "STR " << inst.args[0] << ", [" << inst.result << "] ; [VOLATILE]\n";
            break;
        case bitwise::bir::OpCode::StoreMember:
            // StoreMember args: value, object_reg, member_name
            out << "STR " << inst.args[0] << ", [" << inst.args[1] << ", #" << inst.args[2] << "]\n";
            break;
        case bitwise::bir::OpCode::Ret:
            if (!inst.args.empty()) {
                out << "MOV R0, " << inst.args[0] << "\n";
            }
            out << "BX LR\n";
            break;
        default:
            out << "; UNKNOWN_OPCODE\n";
            break;
    }
}

} // namespace bitwise::backend
