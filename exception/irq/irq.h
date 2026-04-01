#ifndef IRQ_H
#define IRQ_H
#include "../../structure/extern.h"
#include "../../structure/muart.h"
#include "../../lib/core.h"

extern void wakeup_service();

// MUART

void muart_receiver_overrun();
void muart_tx_empty();
void muart_valid_byte();

// SGI

void task_migrated();
#endif