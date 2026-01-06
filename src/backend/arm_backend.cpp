#include "bitwise/arm_backend.hpp"
#include <sstream>

namespace bitwise::backend {

void ArmBackend::emit(const bitwise::bir::Module& module, std::ostream& out) {
    out << ".syntax unified\n";
    out << ".cpu cortex-m4\n";
    out << ".thumb\n\n";

    for (const auto& func : module.functions) {
        emit_function(*func, out);
    }
}

void ArmBackend::emit_function(const bitwise::bir::Function& func, std::ostream& out) {
    out << ".global " << func.name << "\n";
    out << ".type " << func.name << ", %function\n";
    out << func.name << ":\n";
    out << "  PUSH {r7, lr}\n";
    out << "  ADD r7, sp, #0\n";
    
    // Simple stack spill area for virtual regs
    // In a real compiler, we would analyze stack usage.
    out << "  SUB sp, sp, #64\n"; 

    for (const auto& block : func.blocks) {
        if (block->label != "entry") {
            out << block->label << ":\n";
        }

        for (const auto& inst : block->instructions) {
            out << "  ";
            emit_instruction(inst, out);
        }
    }
    
    // Function epilogue (should be unreachable if RET is used, but good safety)
    out << "  ADD sp, r7, #0\n"; 
    out << "  POP {r7, pc}\n\n";
}

void ArmBackend::emit_instruction(const bitwise::bir::Instruction& inst, std::ostream& out) {
    // Map virtual registers to real ARM registers or stack slots
    // For this prototype, we'll maintain a simple direct mapping where possible,
    // but mainly rely on immediates and R0-R3 for scratch.
    
    // Helpers
    auto get_op = [&](int idx) -> std::string {
        if (inst.args[idx].find("%") != std::string::npos) {
            return map_reg(inst.args[idx]); 
        }
        return "#" + inst.args[idx];
    };
    
    auto get_dest = [&]() -> std::string {
        return map_reg(inst.result);
    };

    switch (inst.opcode) {
        case bitwise::bir::OpCode::LoadInt:
            // MOVS Rd, #imm
            // Note: Large immediates need LDR/MOVW, but MOVS handles 0-255
            // For safety in this demo, we use LDR for big numbers
            {
                int val = std::stoi(inst.args[0]);
                if (val >= 0 && val <= 255) {
                    out << "MOVS " << get_dest() << ", #" << val << "\n";
                } else {
                    out << "LDR " << get_dest() << ", =" << val << "\n";
                }
            }
            break;
            
        case bitwise::bir::OpCode::LoadVar:
            // Load from stack slot or memory address kept in reg
            // Bitwise "LoadVar" loads the ADDRESS of the variable if it's not a register
            // Actually, in our BIR, LoadVar usually loads the VALUE. 
            // NOTE: Our BIR semantics are a bit high level. 
            // Let's assume LoadVar loads from a stack spill slot for now.
            // SIMPLIFICATION: We map global/local vars to registers in map_reg.
            out << "MOV " << get_dest() << ", " << map_reg(inst.args[0]) << "\n";
            break;
            
        case bitwise::bir::OpCode::StoreVar:
             // STR src, [dest] (if dest is pointer) or MOV dest, src (if reg)
             out << "MOV " << map_reg(inst.result) << ", " << get_op(0) << "\n";
             break;
             
        case bitwise::bir::OpCode::StoreMember:
             // STR val, [base, #offset]
             out << "STR " << get_op(0) << ", [" << map_reg(inst.args[1]) << ", #" << inst.args[2] << "]\n";
             break;

        case bitwise::bir::OpCode::LoadMember:
             // LDR dest, [base, #offset]
             out << "LDR " << get_dest() << ", [" << map_reg(inst.args[0]) << ", #" << inst.args[1] << "]\n";
             break;
        
        case bitwise::bir::OpCode::Add:
            out << "ADDS " << get_dest() << ", " << get_op(0) << ", " << get_op(1) << "\n";
            break;
            
        case bitwise::bir::OpCode::Sub:
            out << "SUBS " << get_dest() << ", " << get_op(0) << ", " << get_op(1) << "\n";
            break;
            
        case bitwise::bir::OpCode::CmpEQ:
        case bitwise::bir::OpCode::CmpNE:
        case bitwise::bir::OpCode::CmpLT:
        case bitwise::bir::OpCode::CmpGT:
             // Implementing boolean set is tricky in Thumb-2 without IT blocks or conditional movs
             // We will do a CMP and then simple branching or conditional move if available (ARMv7E-M)
             // Using "ITE" block for boolean result
             out << "CMP " << get_op(0) << ", " << get_op(1) << "\n";
             out << "IT NE\n"; // Predicate
             out << "MOVNE " << get_dest() << ", #1\n"; // Placeholder logic
             out << "MOVEQ " << get_dest() << ", #0\n";
             break;
             
        case bitwise::bir::OpCode::Br:
            out << "CMP " << get_op(0) << ", #0\n";
            out << "BNE " << inst.args[1] << "\n";
            out << "B " << inst.args[2] << "\n";
            break;
            
        case bitwise::bir::OpCode::Jump:
            out << "B " << inst.args[0] << "\n";
            break;
            
        case bitwise::bir::OpCode::StoreVolatile:
             out << "STR " << get_op(0) << ", [" << get_dest() << "]\n";
             break;

        case bitwise::bir::OpCode::LoadVolatile:
             out << "LDR " << get_dest() << ", [" << get_op(0) << "]\n";
             break;
             
        case bitwise::bir::OpCode::Ret:
            if (!inst.args.empty()) {
                out << "MOV r0, " << get_op(0) << "\n";
            }
            out << "ADD sp, r7, #0\n";
            out << "POP {r7, pc}\n";
            break;
            
        default:
            out << "@ UNIMPLEMENTED OPCODE\n";
            break;
    }
}

std::string ArmBackend::map_reg(const std::string& virtual_reg) {
    // Basic hash mapping to R0-R6
    // R7 is Frame Pointer, R13 SP, R14 LR, R15 PC
    // We leave R0-R6 mostly free.
    
    // Simple direct mapping for demo:
    // %r0 -> r0
    // %r1 -> r1 ...
    
    if (virtual_reg.empty()) return "r0";
    
    // Check if it is a number register %r123
    if (virtual_reg[0] == '%' && virtual_reg[1] == 'r') {
        int reg_num = std::stoi(virtual_reg.substr(2));
        return "r" + std::to_string(reg_num % 7); // Reuse r0-r6 cyclically
    }
    
    // Named variables -> Hash to register
    size_t hash = 0;
    for(char c : virtual_reg) hash = hash * 31 + c;
    return "r" + std::to_string(hash % 7);
}

} // namespace bitwise::backend
