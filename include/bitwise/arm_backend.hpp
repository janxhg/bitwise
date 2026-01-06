#pragma once

#include "bitwise/bir.hpp"
#include <iostream>

namespace bitwise::backend {

class ArmBackend {
public:
    void emit(const bitwise::bir::Module& module, std::ostream& out);

private:
    void emit_function(const bitwise::bir::Function& func, std::ostream& out);
    void emit_instruction(const bitwise::bir::Instruction& inst, std::ostream& out);
    
    // Simple register allocator: Maps virtual regs (%rX) to ARM regs (r0-r7)
    // In a real compiler, this would use graph coloring.
    // For this prototype, we just hash the string to 0-7 or use a simple rotation.
    std::string map_reg(const std::string& virtual_reg);
};

} // namespace bitwise::backend
