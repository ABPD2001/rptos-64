#ifndef TYPES_SEMAPHORE_H
#define TYPES_SEMAPHORE_H
#include "./base.h"

typedef u64_t semaphore_t;
typedef u64_t mutex_t; // for more simplicy in usage and least cycles.

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