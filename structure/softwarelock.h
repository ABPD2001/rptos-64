#ifndef TYPES_SOFTWARELOCK_H
#define TYPES_SOFTWARELOCK_H
#include "./base.h"

typedef u64_t semaphore_t;
typedef u64_t mutex_t; // for more simplicy in usage and least cycles.

struct slcb_t
{
    u64_t owner_task;  // id of create by which task.
    u64_t gained_task; // id of gained by which task.
    u64_t *lock;
    u16_t id;       // unique identifier.
    u8_t type;      // 0: mutex, 1: semaphore.
    u32_t reserved; // padding (4 Bytes).
}; // software lock control block;

#endif