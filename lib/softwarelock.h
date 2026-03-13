#ifndef LIB_SOFTWARE_LOCK_H
#define LIB_SOFTWARE_LOCK_H
#include "../structure/base.h"
#include "../structure/softwarelock.h"

// -- mutex --

extern u8_t gain_mutex(mutex_t *mlock);
extern u8_t release_mutex(mutex_t *mlock);
extern void spinwait_mutex(mutex_t *mlock);

// -- semaphore --

extern u8_t gain_semaphore(semaphore_t *sem);
extern u8_t release_semaphore(semaphore_t *sem);
extern void spinwait_semaphore(semaphore_t *sem);

#endif