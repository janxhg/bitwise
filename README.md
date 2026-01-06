# Bitwise Compiler 🛡️🐍
> A memory-safe systems programming language built from scratch.

![License](https://img.shields.io/badge/license-MIT-blue.svg)
![Status](https://img.shields.io/badge/status-stable-green.svg)
![Build](https://img.shields.io/badge/build-cmake-orange.svg)

**Bitwise** is a learning project turned into a functional compiler. It features:
*   **Linear Types**: Zero-cost memory safety (Rust-like ownership) without valid-after-move bugs.
*   **Systems Focus**: `packed struct`, `volatile` pointers, and direct memory mapping.
*   **Multi-Target**: Compiles to **Simulated Bytecode** (Nano) and **ARM Thumb-2 Assembly** (Cortex-M).

## Features
*   [x] **Linear Types**: Compile-time check for double-frees and use-after-move.
*   [x] **Hardware Access**: Map structs to memory addresses (`0x4000`) for drivers.
*   [x] **Zero Overhead**: No GC, no runtime, just assembly.
*   [x] **Native Visualizer**: Python-based hardware emulator for testing.

## Quick Start

### 1. Build the Compiler
Requires CMake and a C++20 compiler (MSVC/GCC).
```powershell
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### 2. Run the Snake Demo (Native)
Compile the Bitwise Snake game to assembly and run it on our emulator.
```powershell
# Compile to Nano Assembly
.\Release\bwc.exe ..\tests\demos\snake_native.bw > snake.asm

# Run on Hardware Emulator
cd ..
python tools/emu.py build/snake.asm
```
*Use Arrow Keys to play!* 🐍

### 3. Compile for ARM (STM32/Arduino)
Generate Thumb-2 assembly for real chips.
```powershell
.\Release\bwc.exe --arm ..\tests\demos\snake_native.bw > snake_arm.s
```

## Code Example
```rust
packed struct UART {
    data: i32;
    status: i32;
}

fn main() -> i32 {
    let dev = 0x4000;
    
    // Linear type safety:
    // 'dev' is consumed if passed to a function that takes ownership.
    
    dev.data = 0xFF; // Direct MMIO write
    return 0;
}
```

## Project Structure
*   `src/`: Compiler source (Lexer, Parser, Sema, CodeGen).
*   `tools/`: Python emulator (`emu.py`).
*   `tests/`: 
    *   `demos/`: Runnable visual programs.
    *   `validation/`: Test cases for compiler features.

## License
MIT License. Free to explore and modify.
