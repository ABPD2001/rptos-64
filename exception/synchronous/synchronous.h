#ifndef SYNCHRONOUS_H
#define SYNCHRONOUS_H
#include "../../lib/types/base.h"
#include "../../lib/types/task.h"
#include "../../lib/core.h"

extern void task_dispatcher();
extern void task_schaduler();

void lower_el_wfi_wfe_handler();
#endif