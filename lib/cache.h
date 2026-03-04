#ifndef LIB_CACHE_H
#define LIB_CACHE_H

extern void enable_l1dcache(); // enable L1 Data-Cache.
extern void enable_l1icache(); // enable L1 Instruction-Cache.

extern void disable_l1dcache(); // disable L1 Data-Cache.
extern void disable_l1icache(); // disable L1 Instruction-Cache.

extern void enable_l1cache();  // enable L1 Cache entirely.
extern void disable_l1cache(); // disable L1 Cache entirely.

#endif