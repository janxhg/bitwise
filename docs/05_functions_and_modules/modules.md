# Modules and Visibility

Bitwise uses a simple module system to organize large codebases.

## File-based Modules

Each file in Bitwise is implicitly a module.

```bitwise
// File: sensor.bw
pub fn read_average() -> i32 { ... }
fn internal_calibration() { ... } // Not visible outside
```

## Importing Modules

Use the `use` keyword to bring items into scope.

```bitwise
// File: main.bw
use sensor;

fn main() {
    let avg = sensor::read_average();
}
```

## Visibility (Encapsulation)

- Items are **private** by default.
- Use `pub` to make functions, structs, or constants available to other modules.

```bitwise
pub struct BatteryInfo {
    pub voltage: u16,
    internal_id: u32, // Only visible within this module
}
```

---
Next: [Hardware and Bits](..\06_hardware_and_bits\bit_manipulation.md)
