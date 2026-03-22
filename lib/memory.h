#ifndef LIB_MEMORY_H
#define LIB_MEMORY_H
#include "../structure/base.h"
#include "../structure/extern.h"
#include "../structure/mmu.h"

volatile struct memframe_t *alloc_page(u64_t task);
void free_page(volatile struct memframe_t *page);
#endif