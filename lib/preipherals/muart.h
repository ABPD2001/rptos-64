#ifndef MUART_H
#define MUART_H
#include "../types/base.h"
#include "../types/uart.h"

u64_t muart_write(u8_t *buffer, u64_t length);
u64_t muart_write_char(u8_t ch);
u64_t muart_read(u8_t *buffer, u64_t maximum_length);
u64_t muart_read_char(u8_t *ch);
u8_t muart_availablity();
u64_t muart_settings(u16_t baudrate, u8_t data_bits, u8_t enablation);
#endif