#pragma once

#include "bitwise/bir.hpp"
#include <iostream>
#include <unordered_map>

namespace bitwise::backend {

class ArmBackend {
public:
    void emit(const bitwise::bir::Module& module, std::ostream& out);

private:
    void emit_function(const bitwise::bir::Function& func, std::ostream& out);
    void emit_instruction(const bitwise::bir::Instruction& inst, std::ostream& out);
    
    std::string map_reg(const std::string& virtual_reg);

    std::unordered_map<std::string, std::string> var_regs_;
    int next_var_reg_ = 4;
};

} // namespace bitwise::backend
