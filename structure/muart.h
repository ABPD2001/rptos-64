#ifndef TYPES_UART_H
#define TYPES_UART_H
#include "./base.h"
#include "./semaphore.h"

typedef struct
    muart_settings_t // 32 bytes.
{
    u16_t baudrate;
    u8_t data_bits;
    u8_t enablation; // bits --> 0: enable whole uart, 1: tx enable, 2: rx enable, 3: tx irq enable, 4: rx irq enable, 5-7: padding.
};

typedef struct muart_metadata_t // 54 bytes.
{
    struct muart_settings_t *settings; // reminder: this is a pointer.
    u64_t owner_task;
    u8_t *write_buffer;
    u64_t write_length;
    u64_t written_length;
    u8_t *read_buffer;
    u64_t read_maximum_length;
    u64_t read_length;
    u64_t timeout;
    mutex_t access_mutex; // this used for every kind of access to this metadata (when checking for status of muart).
    u8_t *delimiter;
};

typedef struct muart_statistics_t
{
    u64_t receiver_overruns; // times of receiver overrun happens.
    u64_t wt_rejections;     // if buffer/write length/written length isnt valid.
};

#endif