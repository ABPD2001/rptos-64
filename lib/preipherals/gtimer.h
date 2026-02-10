#ifndef LIB_GTIMER_H
#define LIB_GTIMER_H
#include "./types/base.h"

extern void set_gtimer(u32_t time); // by ms (on CNTP_TVAL_EL1)
extern u64_t read_gticks();         // by ticks (on CNTPCT_EL1)
extern u64_t read_guptime();        // by ticks/frequency (on CNTPCT_EL1)
#endif