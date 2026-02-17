#ifndef BLED_H
#define BLED_H
#include "../structure/base.h"
#include "../drivers/gpio.h"

// on RPI4 (B model):
// GPIO42 --> green led.
// GPIO45 --> red led.

void initialize_bleds();
void bled_on();
void bled_off();
void bled_set(u8_t value);
#endif