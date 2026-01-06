# If / Else in Bitwise

Control flow in Bitwise is expression-based.

## Simple If

There are no parentheses needed around the condition.

```bitwise
if temperature > 100 {
    cool_down();
}
```

## If as an Expression

You can use `if` to assign values, but both branches **must** return the same type.

```bitwise
let state = if voltage > 500 { 1 } else { 0 };
```

## Else if

```bitwise
if speed == 0 {
    halt();
} else if speed < 10 {
    slow();
} else {
    normal();
}
```

---
Next: [Loops](loops.md)
