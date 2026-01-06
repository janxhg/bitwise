# Bit Manipulation

Bitwise is designed for "Bit-banging" and hardware protocols.

## Bitwise Operators

Bitwise supports standard operators with high optimization.

```bitwise
let a: u8 = 0b1010_1010;
let b: u8 = 0b0000_1111;

let combined = a & b; // AND
let merged   = a | b; // OR
let flipped  = ~a;    // NOT
let shifted  = a << 2; // Left shift
```

## The `bit` Type

Unlike other languages where you use a `u8` and masks to get a bit, Bitwise has a native `bit` type.

```bitwise
let status: [bit; 8] = [0; 8]; // An array of 8 bits, using exactly 1 byte of RAM.
status[0] = 1;

if status[0] == 1 {
    trigger_interrupt();
}
```

## Bit Casting

You can view an integer as a set of bits without copying memory.

```bitwise
let reg: u8 = 0xAF;
let view bits = reg as [bit; 8];
if bits[7] { 
    // ... 
}
```

---
Next: [MMIO Registers](mmio_registers.md)
