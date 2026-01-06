# The Ownership Model

The core of Bitwise's safety is its **Ownership System**. It ensures memory safety without a runtime cost (no GC).

## One Owner, One Lifetime

Every piece of data has exactly one owner at any given time.

```bitwise
fn main() {
    let message = String::from("Blink"); // message owns the string
} // message goes out of scope, string is freed automatically
```

## Moving Ownership

When you pass a variable to a function, you are **moving** the ownership. The original variable is invalidated.

```bitwise
fn log(s: String) {
    print(s);
}

fn main() {
    let my_data = String::from("Sensor data");
    log(my_data); // my_data is moved to the log function
    // print(my_data); // COMPILE ERROR: my_data has been moved
}
```

## Why move?
Moving ensures that there is only ever one pointer to a location that can be freed. This prevents "Double Free" and "Use After Free" bugs by design.

---
Next: [Linear Types](linear_types.md)
