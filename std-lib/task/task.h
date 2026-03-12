#ifndef STDLIB_TASK_H
#define STDLIB_TASK_H
#include "../base.h"

extern u64_t taskid();
extern u64_t terminate(u64_t fault_code, u64_t fault_dump);
extern u64_t sleep(u32_t us);
extern u64_t wait(u64_t instruction, u8_t type);
#endif