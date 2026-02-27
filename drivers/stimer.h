#ifndef LIB_STIMER_H
#define LIB_STIMER_H
#include "../structure/base.h"

extern void set_stimer(u32_t cmp_us);
extern u64_t read_stimer_us();
// note: c0 and c2 are reserved by Raspberry pi video core firmware.
// instead, we use c1.
#endif