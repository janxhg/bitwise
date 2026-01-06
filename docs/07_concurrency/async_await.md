# Async / Await

Bitwise provides a zero-allocation `async`/`await` system.

## The Problem with Traditional Async
Most async runtimes require a "Heap" to store the state of a suspended function (the Future). This is not feasible on a system with 32KB of RAM.

## The Bitwise Solution
In Bitwise, the compiler calculates the maximum size of the state machine at compile-time and reserves space on the stack or in a pre-allocated region.

```bitwise
async fn wait_for_signal() -> u8 {
    while gpio.read(PIN_A) == 0 {
        yield; // Give control back to the executor
    }
    return gpio.read(PIN_A);
}

fn main() {
    let result = block_on(wait_for_signal());
}
```

## Cooperative Multitasking
Since there is no OS, `yield` is how you manually allow other tasks to run. This is extremely predictable and deterministic.

---
Next: [Interrupt Handlers](interrupt_handlers.md)
