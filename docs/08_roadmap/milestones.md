# Bitwise Roadmap

The path toward a production-ready system for high-reliability IoT.

## Milestone 1: The Core (In Progress)
- [x] Language Vision and Design.
- [ ] Specification of the BIR (Bitwise Intermediate Representation).
- [ ] Initial Parser for ARM Cortex-M0.

## Milestone 2: Toolchain (`bit` v0.1)
- [ ] Compiler frontend (Syntax + Type checking).
- [ ] Backend for LLVM.
- [ ] `bit` tool for project management.

## Milestone 3: Stability & Ecosystem
- [ ] Standard Library (`core`) containing math, bit-field manipulation, and basic drivers.
- [ ] Integration with VS Code (LSP).
- [ ] Official Simulator for testing without hardware.

## Long-term Vision
- **Medical Certification**: Tools help Bitwise code meet IEC 62304 standards.
- **Industrial Adoption**: Partnerships with PLC and sensor manufacturers.
- **Formal Verification**: A mathematical proof of the memory safety model.
