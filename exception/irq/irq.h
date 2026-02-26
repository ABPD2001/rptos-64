#ifndef IRQ_H
#define IRQ_H
#include "../../structure/muart.h"
#include "../../lib/core.h"

extern void wakeup_service();

volatile struct muart_statistics_t *global_mini_uart_statistics;

// MUART

void muart_receiver_overrun();
void muart_tx_empty();
void muart_valid_byte();

#endif