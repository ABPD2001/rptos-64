#include "./gpio.h"

void gpfunction(u64_t table, u8_t nth, u8_t functionality)
{
    volatile u32_t *ftable = table; // table in mmio.

    *ftable &= ~(0b111 << nth * 3);      // clear flag of nth.
    *ftable |= functionality << nth * 3; // set flag of nth by functionality.
}

void gpset(u64_t table, u8_t nth)
{
    volatile u32_t *stable = table; // table in mmio.

    *stable |= (1 << nth); // set pin in table by nth.
}

void gpclear(u64_t table, u8_t nth)
{
    volatile u32_t *ctable = table; // table in mmio.

    *ctable |= (1 << nth); // clear pin in table by nth.
}

void gpval(u64_t stable, u64_t ctable, u8_t nth, u8_t value)
{
    volatile u32_t *vtable; // table in mmio.

    if (value)
        vtable = stable; // set "set table" if value.
    else
        vtable = ctable; // else set "clear table".

    *vtable |= 1 << nth; // apply on pin nth in table.
}