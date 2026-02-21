#ifndef __LIB_CORE_H__
#define __LIB_CORE_H__
#include "../structure/base.h"
#include "../structure/task.h"

u8_t core_id();
void terminate_context(struct pcb_t *task);

extern void multi_core_enable();
extern void restore_context(struct pcb_t *task);
#endif