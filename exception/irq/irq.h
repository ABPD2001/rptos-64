#ifndef IRQ_H
#define IRQ_H
#include "../../lib/types/muart.h"

void uart_receiver_overrun();
void uart_tx_empty();
#endif