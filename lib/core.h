#ifndef __LIB_CORE_H__
#define __LIB_CORE_H__
#include "../drivers/stimer.h"
#include "../structure/core.h"
#include "../structure/dump.h"
#include "../structure/base.h"
#include "../structure/extern.h"
#include "../structure/ipcmailbox.h"
#include "../structure/gpio.h"
#include "../structure/muart.h"
#include "../structure/schaduler.h"
#include "../structure/softwarelock.h"
#include "../structure/task.h"
#include "../lib/fwlist.h"
#include "../lib/softwarelock.h"

typedef void *(__built_in_free_preipheral_t)(u64_t);

// Core.

u8_t core_id();
extern void system_panic();
extern void void_event_loop();
extern void multi_core_enable();

extern void restore_core_context(volatile struct cccb_t *core_context);
extern void save_core_context();

void core_event_number(u8_t number);

// Context.

extern void restore_context(volatile struct pcb_t *task);
void terminate_context(volatile struct pcb_t *task);
void core_terminate();

// DAIF

extern void enable_irq();
extern void disable_irq();

extern void enable_fiq();
extern void disable_fiq();

extern void enable_daif();
extern void disable_daif();

// PSCI

extern s64_t psci_cpu_off();                                                   // if returns, means failed.
extern s64_t psci_cpu_on(u8_t core_id, u64_t entry_address, u64_t context_id); // turn on a core.

extern s64_t psci_system_reset(); // if returns, means failed.
extern s64_t psci_system_off();   // if returns, means failed.

extern s64_t psci_cpu_suspend(u16_t stateid, u8_t statetype, u8_t powerlevel); // turn current core into an idle mode.

s64_t psci_cpu_powerdown(); // a abstracted layer of "psci_cpu_suspend" function to enter power-down mode.
s64_t psci_cpu_standby();   // a abstract layer of "psci_cpu_suspend" function to enter standby mode.

extern void cpu_standby_wfe(); // a standby entering with "wfe" instruction.
extern void cpu_standby_wfi(); // a standby entering with "wfi" instruction.

#endif