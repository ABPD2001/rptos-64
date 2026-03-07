#ifndef TYPES_CORE_H
#define TYPES_CORE_H
#include "./base.h"

struct cccb_t // Core Context Control Block. (96 Bytes)
{
    u64_t valid;
    // mmu

    u64_t ttbr0;
    u64_t ttbr1;
    u64_t tcr;

    // system & exception

    u64_t sctlr;
    u64_t far;
    u64_t spsr;

    // Genric timer register.

    u64_t cntp_ctl;
    u64_t cntfrq;
    u64_t cntp_tval;
    u64_t cntpct;
};
#endif