#ifndef LIB_GPIO_H
#define LIB_GPIO_H
#include "../types/base.h"

void gpfunction(u64_t table, u8_t nth, u8_t functionality);
void gpset(u64_t table, u8_t nth);
void gpclear(u64_t table, u8_t nth);
void gpval(u64_t stable, u64_t ctable, u8_t nth, u8_t value);
#endif