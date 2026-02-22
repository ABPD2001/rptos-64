#ifndef IRQ_H
#define IRQ_H
#include "../../structure/muart.h"

void muart_receiver_overrun();
void muart_tx_empty();
#endif