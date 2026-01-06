# Data Types

Bitwise provides a strict set of types to ensure portability across different architectures.

## Integers (Fixed Width)

You must be explicit about the size of your data.

| Type | Bits | Range |
| :--- | :--- | :--- |
| `u8`  | 8    | 0 to 255 |
| `i8`  | 8    | -128 to 127 |
| `u16` | 16   | 0 to 65,535 |
| `i16` | 16   | -32,768 to 32,767 |
| `u32` | 32   | 0 to 4.2B |
| `i32` | 32   | -2.1B to 2.1B |

## Booleans and Bits

In low-level programming, we often need to represent a single bit.

```bitwise
let is_active: bool = true;
let flag: bit = 1; // Explicit 1-bit value
```

## Floating Point

Floats are only available if the target hardware supports an FPU (Floating Point Unit), otherwise, your code will fail to compile unless you use the `softfloat` library.

```bitwise
let pressure: f32 = 1013.25;
```

## Literals

Bitwise supports binary and hex literals natively, which are essential for bit-masking.

```bitwise
let mask = 0b1010_0000;
let addr = 0x4000_1000;
```

---
Next: [Control Flow](..\03_control_flow\if_else.md)
