#include <iostream>
#include <string>
#include <vector>
#include "bitwise/lexer.hpp"
#include "bitwise/parser.hpp"
#include "bitwise/sema.hpp"
#include "bitwise/bir_gen.hpp"
#include "bitwise/codegen.hpp"
#include "bitwise/arm_backend.hpp"
#include "bitwise/diagnostic.hpp"
#include "bitwise/file_utils.hpp"
#include "bitwise/version.hpp"

void print_version() {
    std::cout << "Bitwise Compiler (bwc) v" << BITWISE_VERSION_STR << "\n";
    std::cout << "Target Hardware: Edge & Micro-sensors (<1MB RAM)\n";
}

void print_help() {
    std::cout << "Usage: bwc [options] <file.bw>\n";
    std::cout << "Options:\n";
    std::cout << "  --help       Show this message\n";
    std::cout << "  --version    Show version info\n";
    std::cout << "  --arm        Emit ARM Thumb-2 Assembly\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_help();
        return 1;
    }

    std::string arg = argv[1];
    bool use_arm = false;
    
    if (argc > 2 && std::string(argv[1]) == "--arm") {
        use_arm = true;
        arg = argv[2];
    }

    if (arg == "--version") {
        print_version();
        return 0;
    } else if (arg == "--help") {
        print_help();
        return 0;
    }

    std::cerr << "Compiling: " << arg << "...\n";
    
    auto source = bitwise::common::read_file(arg);
    if (!source) {
        std::cerr << "[ERROR] Could not open file: " << arg << "\n";
        return 1;
    }

    bitwise::common::DiagnosticEngine diags;
    bitwise::frontend::Lexer lexer(*source);
    auto tokens = lexer.tokenize();

    std::cerr << "[INFO] Tokenization complete. Found " << tokens.size() << " tokens.\n";
    
    try {
        bitwise::frontend::Parser parser(std::move(tokens), diags);
        auto program = parser.parse();
        std::cerr << "[INFO] Parsing complete.\n";

        std::cerr << "[INFO] Running Semantic Analysis (Ownership Check)...\n";
        bitwise::sema::SemanticAnalyzer sema(diags);
        sema.analyze(*program);
        
        if (!diags.has_errors()) {
            std::cerr << "[INFO] Semantic analysis complete. Program is memory-safe.\n";
            
            std::cerr << "[INFO] Generating BIR (Intermediate Representation)...\n";
            bitwise::bir::BIRGenerator bir_gen(&diags);
            auto bir_module = bir_gen.generate(*program);
            
            std::cerr << "[INFO] BIR Generation complete. Function count: " << bir_module->functions.size() << "\n";
            
            if (use_arm) {
                std::cerr << "[INFO] Generating Final Code (ARM Cortex-M Backend)...\n";
                bitwise::backend::ArmBackend backend;
                backend.emit(*bir_module, std::cout);
            } else {
                std::cerr << "[INFO] Generating Final Code (Nano Backend)...\n";
                bitwise::backend::NanoBackend backend;
                backend.emit(*bir_module, std::cout);
            }
            
            std::cerr << "\n[SUCCESS] Compilation Pipeline Finished.\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "[FATAL] Parsing halted: " << e.what() << "\n";
        return 1;
    }

    if (diags.has_errors()) {
        std::cerr << "[ERROR] Compilation failed with " << diags.error_count() << " errors.\n";
        return 1;
    }
}
