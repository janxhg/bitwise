#pragma once

#include "bitwise/bir.hpp"
#include <ostream>

namespace bitwise::backend {

class CodeGenerator {
public:
    virtual ~CodeGenerator() = default;
    virtual void emit(const bitwise::bir::Module& module, std::ostream& out) = 0;
};

// A minimalist backend that generates a pseudo-assembly for micro-controllers
class NanoBackend : public CodeGenerator {
public:
    void emit(const bitwise::bir::Module& module, std::ostream& out) override;

private:
    void emit_function(const bitwise::bir::Function& func, std::ostream& out);
    void emit_instruction(const bitwise::bir::Instruction& inst, std::ostream& out);
};

} // namespace bitwise::backend
