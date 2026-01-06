# Pattern Matching

To replace complex `if/else` chains, Bitwise uses `switch`.

## Basic Switch

The `switch` statement must be exhaustive, meaning you must cover all possible values (or use a default).

```bitwise
let command: u8 = serial.read();

switch command {
    0x01 => start_motor(),
    0x02 => stop_motor(),
    0x03 => emergency_braking(),
    _    => log_error("Unknown command"), // Default case
}
```

## Range Matching

You can also match against ranges of values.

```bitwise
switch voltage {
    0..100   => print("Low"),
    101..500 => print("Normal"),
    _        => print("High"),
}
```

---
Next: [Memory Management](..\04_memory_management\ownership_model.md)
