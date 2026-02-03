#ifndef TYPES_UART_H
#define TYPES_UART_H
#include "./base.h"

typedef struct
    muart_settings_t
{
    u16_t baudrate;
    u8_t data_bits;
    u8_t enablation;
    u32_t padding;
};

typedef struct muart_metadata_t
{
    struct muart_settings_t *settings;
    u64_t owner_task;
    u8_t *write_buffer;
    u64_t write_length;
    u8_t *read_buffer;
    u64_t read_maximum_length;
};

#endif