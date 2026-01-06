# Regions and Scopes

For microcontrollers with very small RAM, a traditional "Heap" causes fragmentation. Bitwise uses **Region-based allocation** instead.

## What is a Region?

A region is a contiguous block of memory where you can allocate multiple objects.

```bitwise
region SensorData { size: 1024 } // Define a 1KB region

fn process() {
    using SensorData {
        let p1 = new(0); // Allocates at start of region
        let p2 = new(1); // Allocates after p1
    } // Everything in the region is invalidated here!
}
```

## Zero Fragmentation

Because all allocations move forward in the region and the entire region is cleared at once (resetting the pointer), there is **zero fragmentation**. 

## Static Verification

The Bitwise compiler ensures that no pointer (view) to an object inside a region can outlive the region itself.

```bitwise
let leaky_ptr;
using MyRegion {
    let x = new(10);
    // leaky_ptr = x; // COMPILE ERROR: x outlives its region
}
```

---
Next: [Functions and Modules](..\05_functions_and_modules\functions.md)
