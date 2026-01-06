# MMIO (Memory Mapped I/O)

In embedded systems, you interact with hardware by reading and writing to specific memory addresses.

## Defining Hardware Registers

Bitwise allows you to map structs directly to memory addresses with the `volatile` keyword.

```bitwise
// Define the layout of a timer register
packed struct TimerConfig {
    enabled: bit,
    prescaler: u3,
    reserved: u4,
}

// Map it to a specific address in the MCU
volatile let TIMER0_CTRL: TimerConfig @ 0x4000_1000;
```

## Atomic Safe Access

The `volatile` keyword tells the compiler to never optimize out reads or writes to this location. Bitwise also ensures that bit-level access to registers is converted into the most efficient atomic instruction the hardware supports (e.g., `BFI` on ARM).

```bitwise
TIMER0_CTRL.enabled = 1; // Generates a single atomic write instruction
```

---
Next: [Concurrency](..\07_concurrency\async_await.md)
