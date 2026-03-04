#ifndef LIB_CACHE_H
#define LIB_CACHE_H
#include "../structure/base.h"

extern void enable_l1dcache(); // enable L1 Data-Cache.
extern void enable_l1icache(); // enable L1 Instruction-Cache.

extern void disable_l1dcache(); // disable L1 Data-Cache.
extern void disable_l1icache(); // disable L1 Instruction-Cache.

extern void enable_l1cache();  // enable L1 Cache entirely.
extern void disable_l1cache(); // disable L1 Cache entirely.

// L1 CACHE (DATA)

extern void invalidate_setway_l1dcache(u8_t setway);
extern void clean_invalidate_setway_l1dcache(u8_t setway);
extern void invalidate_va_poc_l1dcache(u64_t virtual_address);
extern void invalidate_va_pou_l1dcache(u64_t virtual_address);
extern void clean_invalidate_va_poc_l1dcache(u64_t virtual_address);
extern void clean_invalidate_va_pou_l1dcache(u64_t virtual_address);

// L1 CACHE (INSTRUCTION)
extern void invalidate_all_pou_inner_shareable();
extern void invalidate_all_pou();
extern void invalidate_va_pou(u64_t virtual_address);
#endif