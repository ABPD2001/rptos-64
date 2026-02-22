#ifndef TYPES_SEMAPHORE_H
#define TYPES_SEMAPHORE_H
#include "./base.h"

typedef u64_t semaphore_t;
typedef u64_t mutex_t; // for more simplicy in usage and least cycles.

typedef struct slcb_t
{
    u64_t owner_task;  // id of create by which task.
    u64_t gained_task; // id of gained by which task.
    u64_t *lock;
    u16_t id;       // unique identifier.
    u8_t type;      // 0: mutex, 1: semaphore.
    u32_t reserved; // padding (4 Bytes).
}; // software lock control block;

// -- mutex --

void gain_mutex(mutex_t *mlock);
void release_mutex(mutex_t *mlock);
void spinwait_mutex(mutex_t *mlock);
u8_t availablity_mutex(mutex_t *mlock);

// -- semaphore --

void gain_semaphore(semaphore_t *sem);
void release_semaphore(semaphore_t *sem);
void spinwait_semaphore(semaphore_t *sem);

#endif