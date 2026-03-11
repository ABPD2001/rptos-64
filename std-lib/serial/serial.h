#ifndef SSC_LIB_H
#define SSC_LIB_H
#include "../structure/base.h"

extern u64_t serial_put(u8_t *buffer, u64_t length);
u64_t serial_read(u8_t *buffer, u64_t maximum_length);

u64_t allocate_serial();
u64_t release_serial();
u64_t serial_available();
#endif