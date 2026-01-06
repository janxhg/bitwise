# Linear Types

In Bitwise, some types are **Linear**, meaning they must be used exactly once.

## Resource Management

Linear types are perfect for representing hardware resources that shouldn't be shared or "lost" (leaked).

```bitwise
// Let's imagine a WifiConnection is a linear type
let conn = wifi.connect("HOME");

// You MUST call disconnect or pass it to a handler
// If the variable goes out of scope without being used,
// the compiler will issue an error.
```

## Eliminating Resource Leaks

A linear type is a guarantee from the programmer to the compiler: "I will handle this resource". This is a step beyond Rust's affine types (which can be dropped). In Bitwise, for high-criticality modules, you can mark a struct as `linear`.

```bitwise
linear struct UrgentPacket {
    data: [u8; 128],
    priority: u8,
}
```

If you create an `UrgentPacket` and forget to send it or free it properly, the compiler will catch it.

---
Next: [Regions and Scopes](regions_and_scopes.md)
