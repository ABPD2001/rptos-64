#ifndef TYPES_PANIC_H
#define TYPES_PANIC_H
#include "./base.h"

typedef struct system_panic_log_t // 88 Bytes.
{
    u64_t syndrome;  // ESR_EL1
    u64_t elr;       // ELR_EL1
    u64_t fp;        // X29
    u64_t lr;        // X30
    u64_t spsr;      // SPSR_EL1
    u64_t far;       // FAR_EL1
    u64_t mpidr;     // MPIDR_EL1
    u64_t sctlr;     // SCTLR_EL1
    u64_t stimer;    // value of system timer.
    u8_t stack[128]; // last 128 bytes of stack
};
#endif