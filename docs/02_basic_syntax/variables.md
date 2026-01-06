# Variables in Bitwise

Variables in Bitwise are designed to eliminate the risks of uninitialized memory.

## Declaration and Initialization

Unlike C, Bitwise **requires** all variables to be initialized at the time of declaration.

```bitwise
// Let defines a mutable variable by default
let temperature = 25; 

// Type inference is supported
let humidity: f32 = 45.5;
```

## Immutable Constants

Use the `const` keyword for values that never change. These are often evaluated at compile-time to save ROM.

```bitwise
const SENSOR_ID = 0xAF;
const BAUD_RATE: u32 = 115200;
```

## Scope and Shadowing

Variables are scoped to the block `{}` they are defined in. Shadowing is allowed to prevent name collision in complex functions.

```bitwise
let x = 10;
{
    let x = 20; // This x shadows the outer one
    print(x); // Outputs 20
}
print(x); // Outputs 10
```

---
Next: [Data Types](data_types.md)
