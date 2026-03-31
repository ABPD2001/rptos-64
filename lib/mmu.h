#ifndef LIB_MMU_H
#define LIB_MMU_H
#include "../structure/base.h"
#include "../structure/mmu.h"

// MMU

extern void enable_mmu();  // enable mmu.
extern void disable_mmu(); // disable mmu.

extern void set_ttbr0(u64_t base_address, u16_t asid, u8_t skl, u8_t cnp); // set TTBR0_EL1.
extern void set_ttbr1(u64_t base_address, u16_t asid, u8_t skl, u8_t cnp); // set TTBR1_EL1.

extern void set_ttbr0_dry(u64_t base_address, u8_t skl, u8_t cnp); // set TTBR0_EL1 (without synchronization).
extern void set_ttbr1_dry(u64_t base_address, u8_t skl, u8_t cnp); // set TTBR1_EL1 (without synchronization).

extern void mmu_settings(struct mmu_settings_t *mmu_settings); // settings of TCR_EL1.

extern void mmu_configuration(u64_t base_address0, u64_t base_address1, u16_t asids, u8_t enable, u8_t cnps, u8_t skls); // Basic configuration of mmu.
extern void mmu_initialize();                                                                                            // initialize mmu with default settings.

// TLB (MMU)

extern void invalidate_tlb();
extern void invalidate_tlb_asid(u16_t asid);
extern void invalidate_tlb_va(u64_t virtual_address);
extern void invalidate_tlb_va_allasids(u64_t virtual_address);

// translation

extern u64_t translate_address(u64_t address, u64_t ttb0r, u64_t ttb1r);
#endif