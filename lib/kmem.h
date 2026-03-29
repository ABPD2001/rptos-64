#ifndef LIB_KMEM_H
#define LIB_KMEM_H
#include "../structure/base.h"
#include "../structure/extern.h"
#include "../structure/mmu.h"
#include "../lib/softwarelock.h"

volatile struct kmem_page_t *alloc_organized_frame(u8_t size); // allocates 2KB,8KB,32KB,128KB,512KB sizes.
u64_t available_organized_frame(u8_t size);
u64_t free_organized_frame(u32_t id); // free by id.

volatile struct kmem_page_t *alloc_frame(u32_t size);
u64_t free_frame(u32_t id); // free by id.

void clear_mem_incr(u64_t start_address, u64_t size);
void clear_mem_decr(u64_t start_address, u64_t size);

void clear_frame(u64_t start_address, u8_t size);

#endif