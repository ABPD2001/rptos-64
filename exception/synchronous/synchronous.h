#ifndef SYNCHRONOUS_H
#define SYNCHRONOUS_H
#include "../../structure/base.h"
#include "../../structure/task.h"
#include "../../lib/core.h"

extern void task_dispatcher();
extern void task_schaduler();

void lower_el_wfi_wfe_handler();
void lower_el_unkown_handler();
void lower_el_sp_alignment_handler();
void lower_el_pc_alignment_handler();
#endif