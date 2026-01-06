# Bitwise: The Post-C Philosophy

Bitwise is born from a simple observation: The world is full of software that is either **unsafe** (C/C++) or **unnecessarily heavy** (Rust) for the smallest of devices.

## Why Bitwise?

Governments and security agencies are demanding a move away from memory-unsafe languages. However, in the realm of micro-sensors, wearables, and industrial IoT, every byte of RAM is a precious resource.

### The Problem with C
C is fast and small, but it's a "minefield". A single pointer error can crash a medical implant or leave an industrial sensor open to remote exploitation.

### The Problem with Rust
Rust is safe, but its binary size and memory footprint can be too large for hardware with less than 256KB of RAM. Furthermore, its borrow-checker has a steep learning curve that many embedded engineers find daunting.

## The Bitwise Vision

Bitwise aims to be the **Safety Minimalist** alternative:
1. **Predictable**: No Hidden GC, no hidden allocations.
2. **Safe**: Memory safety by construction, not by garbage collection.
3. **Ergonomic**: Syntax that feels like a modern upgrade to C.
4. **Transparent**: You always know exactly what happens at the bit level.

---
Next: [Installation Guide](installation.md)
