#ifndef UART_H
#define UART_H
#include "../structure/uart.h"
#include "../drivers/gpio.h"

void initialize_uart(u8_t uart_nth, u8_t crts_en);

void uart_settings(u8_t uart_nth, u8_t word_length, u8_t enable_fifos, u8_t threshold, u8_t enable_unit, u8_t communication_settings);

void uart_write(u8_t uart_nth, u8_t *buffer, u64_t length);
void uart_read(u8_t uart_nth, u8_t *buffer, u64_t maximum_length, u64_t timeout);

void uart_write_ch(u8_t uart_nth, u8_t ch);
u8_t uart_read_ch(u8_t uart_nth);
#endif