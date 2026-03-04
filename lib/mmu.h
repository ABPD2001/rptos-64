#ifndef LIB_MMU_H
#define LIB_MMU_H
#include "../structure/base.h"
#include "../structure/mmu.h"

extern void enable_mmu();  // enable mmu.
extern void disable_mmu(); // disable mmu.

void set_ttbr0(u64_t base_address); // set TTBR0_EL1.
void set_ttbr1(u64_t base_address); // set TTBR1_EL1.

void mmu_settings(struct mmu_settings_t *mmu_settings);

extern void mmu_configuration(u64_t base_address0, u64_t base_address1, u8_t enable);
#endif