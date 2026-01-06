# Functions in Bitwise

Functions are the building blocks of Bitwise logic.

## Declaration

Functions are declared with the `fn` keyword. Parameters and return types are always explicit.

```bitwise
fn add(a: i32, b: i32) -> i32 {
    return a + b;
}
```

## Borrowing (Views)

If you don't want to move ownership of a variable, you can pass it as a `view`. This is equivalent to an immutable reference in Rust, but with simpler syntax.

```bitwise
fn calculate_sum(view buffer: [u8; 128]) -> u32 {
    let mut sum: u32 = 0;
    for byte in buffer {
        sum += byte as u32;
    }
    return sum;
}
```

## Mutable Borrows

To modify a value without taking ownership, use `view mut`.

```bitwise
fn reset_counter(view mut counter: u32) {
    counter = 0;
}
```

---
Next: [Modules](modules.md)
