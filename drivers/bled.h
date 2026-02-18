#ifndef BLED_H
#define BLED_H
#include "../structure/base.h"
#include "../drivers/gpio.h"

// on RPI4 (B model):
// GPIO42 --> green led.
// GPIO45 --> red led.

void initialize_bleds();
void brled_on();
void bgled_on();
void brled_off();
void bgled_off();
void brled_set(u8_t value);
void bgled_set(u8_t value);
#endif