#ifndef TYPES_TIMER_H
#define TYPES_TIMER_H
#include "./base.h"

struct timer_request_t
{
    u64_t *task_id;
    u64_t wake_ticks;
    volatile struct timer_request_t *next; // for forward list.
};

#endif