#ifndef TYPES_UART_H
#define TYPES_UART_H
#include "./base.h"
#include "./softwarelock.h"

typedef struct uart_statistics_t
{
    u64_t overrun_errors;
    u64_t parity_errors;
    u64_t break_errors;
};

typedef struct uart_settings_t
{
    u8_t communication_settings; // 0: parity enable, 1: Even parity select, 2: two stop bits select, 3: sticky parity select, 4: send break (communication is always remain connected), 5-7: reserved.
    u8_t word_length;            // (0 -> 5, 1 -> 6, 2 -> 7, 3 -> 8) bits word length.
    u8_t enable_fifos;           // set to 1 to enable fifos (rx & tx).
    u8_t threshold;              // 0-2: tx fifo threshold, 3-5: rx fifo threshold, 6-7: reserved.
    u8_t enable_unit;            // set to one for enabling current uart unit.
};

typedef struct uart_metadata_t
{
    struct uart_settings_t *settings;
    u64_t owner_task;
    u8_t *write_buffer;
    u64_t write_length;
    u64_t written_length;
    u8_t *read_buffer;
    u64_t read_maximum_length;
    u64_t read_length;
    u64_t timeout;
    mutex_t access_mutex; // this used for every kind of access to this struct.
    u8_t *delimiter;
};

#endif