#ifndef STDLIB_SERIAL_H
#define STDLIB_SERIAL_H
#include "../base.h"
#include "../ipc/mailbox/mailbox.h"

extern u64_t serial_put(u8_t *buffer, u64_t length);
extern u64_t serial_read(u8_t *buffer, u64_t maximum_length);

extern u64_t allocate_serial();
extern u64_t release_serial();
extern u64_t serial_available();

u64_t printf(u8_t *buffer);                              // automatic print function.
u64_t scanf(u8_t *buffer, u64_t legnth, u8_t delimiter); // automatic read function.
#endif