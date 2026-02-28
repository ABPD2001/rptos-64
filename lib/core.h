#ifndef __LIB_CORE_H__
#define __LIB_CORE_H__
#include "../drivers/stimer.h"
#include "../structure/base.h"
#include "../structure/extern.h"
#include "../structure/gpio.h"
#include "../structure/muart.h"
#include "../structure/schaduler.h"
#include "../structure/softwarelock.h"
#include "../structure/task.h"
#include "../lib/fwlist.h"
#include "../lib/softwarelock.h"

typedef void *(__built_in_free_preipheral_t)(u64_t);

u8_t core_id();
void terminate_context(volatile struct pcb_t *task);

extern void multi_core_enable();
extern void restore_context(struct pcb_t *task);

extern void enable_irq();
extern void disable_irq();
extern void enable_fiq();
extern void disable_fiq();
extern void enable_daif();
extern void disable_daif();
#endif