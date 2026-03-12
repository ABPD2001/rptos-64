#ifndef STDLIB_SERIAL_H
#define STDLIB_SERIAL_H
#include "../structure/base.h"

extern u64_t serial_put(u8_t *buffer, u64_t length);
extern u64_t serial_read(u8_t *buffer, u64_t maximum_length);

extern u64_t allocate_serial();
extern u64_t release_serial();
extern u64_t serial_available();
#endif