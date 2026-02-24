#ifndef __LIB_CORE_H__
#define __LIB_CORE_H__
#include "../structure/base.h"
#include "../structure/task.h"
#include "../structure/gpio.h"
#include "../structure/softwarelock.h"
#include "../lib/softwarelock.h"
#include "../structure/muart.h"

extern volatile struct gpio_ownership_t *global_gpio_bank;
extern volatile struct slcb_t *global_software_locks_bank;

typedef void *(__built_in_free_preipheral_t)(u64_t);

u8_t core_id();
void terminate_context(struct pcb_t *task);

extern void multi_core_enable();
extern void restore_context(struct pcb_t *task);

extern void enable_irq();
extern void disable_irq();
extern void enable_fiq();
extern void disable_fiq();
extern void enable_daif();
extern void disable_daif();
#endif