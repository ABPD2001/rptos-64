#ifndef TYPES_DEBUG_H
#define TYPES_DEBUG_H
#include "./base.h"

struct system_breakpoint_t // 152 Bytes.
{
    u64_t pc;     // points to where the exception happens.
    u64_t spsr;   // saved PSTATE fields.
    u64_t time;   // value of timer.
    u8_t sp[128]; // last 128 bytes of kernel stack.
};

#endif