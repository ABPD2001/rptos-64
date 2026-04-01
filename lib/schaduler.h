#ifndef LIB_BUILT_IN_SCHADULER_H
#define LIB_BUILT_IN_SCHADULER_H

#include "../lib/core.h"
#include "../lib/fwlist.h"
#include "../lib/ipcmailbox.h"
#include "../lib/math.h"
#include "../lib/mmu.h"

#include "../drivers/gic400.h"

#include "../structure/base.h"
#include "../structure/extern.h"
#include "../structure/muart.h"

u64_t __built_in_task_waiting(volatile struct pcb_t *task);
void task_schaduler();  // sorts tasks and groups them to a standard and calculatable way.
void task_dispatcher(); // dispatces task for its core.
void wakeup_service();  // services timer requests.
void memoryvisor();     // monitors if memory eviction threshold must be triggred or not.
#endif