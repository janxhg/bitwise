# Setting Up the Bitwise Environment

The Bitwise toolchain is designed to be lightweight and portable.

## Prerequisites

To compile Bitwise programs, you only need:
- A host OS (Windows, Linux, or macOS).
- `bwc` (The Bitwise Compiler).
- For advanced targets: `lld` or `arm-none-eabi-gcc` (though Bitwise aims to be self-contained in the future).

## Installing the Compiler

Currently, Bitwise is in its early stages. You can download the latest pre-release binary of `bwc` from the official repository or build it from source.

### Build from Source
```bash
git clone https://github.com/bitwise-lang/bitwise
cd bitwise
make install
```

## The Workspace

Bitwise projects are managed by the `bit` tool. Initialize your first project:

```bash
bit new hello_sensor
cd hello_sensor
```

This will create a `bit.toml` configuration file and a `src/main.bw` entry point.

---
Next: [Variables](..\02_basic_syntax\variables.md)
