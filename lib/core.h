#ifndef __LIB_CORE_H__
#define __LIB_CORE_H__
#include "./types/base.h"
#include "./types/task.h"

u8_t core_id();
extern void multi_core_enable();
extern void restore_context(pcb_t *task);
#endif