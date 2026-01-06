# Loops

Bitwise provides three primary ways to repeat code.

## 1. The `loop` (Infinite)

Essential for the main logic of a microcontroller.

```bitwise
loop {
    let data = read_sensor();
    process(data);
    if data == 0xFF {
        break; // Exit the loop
    }
}
```

## 2. The `while` loop

Repeat as long as a condition is met.

```bitwise
let counter = 0;
while counter < 10 {
    blink_led();
    counter += 1;
}
```

## 3. The `for` loop (Iterators)

Used to iterate over arrays or ranges. Note that ranges are inclusive by default.

```bitwise
let buffer = [0; 10];
for i in 0..10 {
    buffer[i] = i * 2;
}
```

---
Next: [Pattern Matching](match_pattern.md)
