# Interrupt Handlers (ISRs)

Interrupts are the "events" of the embedded world. Bitwise makes them safe.

## Defining an Interrupt

Use the `interrupt` keyword. The compiler will automatically handle the context saving and restoring (pushing/popping registers).

```bitwise
interrupt fn on_timer_fire() {
    led.toggle();
    TIMER0_CTRL.clear_flag();
}
```

## Safety in Interrupts

Bitwise enforces strict rules for ISRs:
- **No Blocking**: You cannot `await` or `loop` indefinitely inside an ISR.
- **Shared State**: To access data outside the ISR, you must use a `SharedAtom` or a `GlobalRegion`.

```bitwise
let mut click_count: SharedAtom<u32> = SharedAtom::new(0);

interrupt fn on_button_push() {
    click_count.increment();
}
```

This prevents race conditions between your main code and your hardware interrupts.

---
Next: [Roadmap](..\08_roadmap\milestones.md)
