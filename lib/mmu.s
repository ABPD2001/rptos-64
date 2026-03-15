.section .text
.balign 4

.global enable_mmu,disable_mmu,set_ttbr0,set_ttbr1,mmu_configuration,mmu_settings,invalidate_tlb,invalidate_tlb_asid,invalidate_tlb_va,invalidate_tlb_va_allasids

enable_mmu:
    mrs x0,SCTLR_EL1 @ read system control register.
    orr x0,#0x1 @ set first bit. (mmu enable)
    msr SCTLR_EL1,x0 @ apply.
    isb sy @ wait until apply.

    ret @ return.

disable_mmu:
    mrs x0,SCTLR_EL1 @ read system control register.
    bic x0,#0x1 @ clear first bit. (mmu enable)
    msr SCTLR_EL1,x0 @ apply.
    isb sy @ wait until apply.

    ret @ return.

set_ttbr0:
    msr TTBR0_EL1,x0 @ set Translation Table Base Register 0.
    isb sy @ wait until apply.
    ret @ return.

set_ttbr1:
    msr TTBR1_EL1,x0 @ set Translation Table Base Register 1.
    isb sy @ wait until apply.
    ret @ return.

mmu_configuration:
    cmp x2,#0 @ compare.
    cset x2,EQ @ set bool value.

    mrs x3,SCTLR_EL1 @ read system control register value.
    orr x3,x3,x2 @ set mmu enabltion.

    msr TTBR0_EL1,x0 @ set Translation Table Base Register 0.
    msr TTBR1_EL1,x1 @ set Translation Table Base Register 1.
    msr SCTLR_EL1,x3 @ apply mmu enablation.
    isb ish @ wait until inner shareable synchronizes. 

    ret @ return.

mmu_settings:
    ldr x1,[x0],#1 @ read "translation granuel size of user mode".
    ldr x2,[x0],#1 @ read "translation granuel size of kernel mode".
    ldr x3,[x0],#1 @ read "intermidate physical address size".
    ldr x4,[x0],#1 @ read "inner cachablity of user".
    ldr x5,[x0],#1 @ read "inner cachablity of kernel".
    ldr x6,[x0],#1 @ read "outer cachablity of user".
    ldr x7,[x0],#1 @ read "outer cachablity of kernel".
    ldr x8,[x0],#1 @ read "shareablity of user".
    ldr x9,[x0],#1 @ read "shareablity of kernel".
    ldr x10,[x0],#1 @ read "TTBNR_EL1, N = 0/1 of user".
    ldr x11,[x0],#1 @ read "TTBNR_EL1, N = 0/1 of kernel".
    dmb ish @ data memory barrier (inner shareable).

    lsl x1,#29 @ shift to left.
    lsl x2,#13 @ shift to left.
    lsl x3,#31 @ shift to left.
    lsl x4,#7 @ shift to left.
    lsl x5,#23 @ shift to left.
    lsl x6,#9 @ shift to left.
    lsl x7,#25 @ shift to left.
    lsl x8,#11 @ shift to left.
    lsl x9,#27 @ shift to left.

    and x10,x10,#0x1F @ mask only first 5 bits.    
    and x11,x11,#0x1F @ mask only first 5 bits.    
    lsl x11,#15 @ shift to left.

    @ merge masks.
    orr x1,x1,x2
    orr x1,x1,x3
    orr x1,x1,x4
    orr x1,x1,x5
    orr x1,x1,x6

    orr x1,x1,x7
    orr x1,x1,x8
    orr x1,x1,x9
    orr x1,x1,x10
    orr x1,x1,x11

    @ apply to special register.
    msr TCR_EL1,x1
    isb sy @ wait until everything synchronizes.

    ret @ return.

invalidate_tlb:
    tlbi alle1 @ invalidate all EL1.
    tlbi alle0 @ invalidate all EL0.
    ret @ return.

invalidate_tlb_asid:
    tlbi aside1,x0 @ invalidate by ASID EL1.
    ret @ return.

invalidate_tlb_va:
    tlbi vae0,x0 @ invalidate by VA EL0.
    ret @ return.

invalidate_tlb_va_allasids:
    tlbi vaae1,x0 @ invalidate by VA (all asids) EL1.
    ret @ return.

translate_address:
    msr TTBR0_EL1,x3 @ save.
    msr TTBR1_EL1,x4 @ save.
    
    mrs x1,TTBR0_EL1 @ apply.
    mrs x2,TTBR1_EL1 @ apply.
    isb sy @ wait until synchronizes...

    ldr x1,[x0] @ use virtual address.
    mrrs x0,x1,PAR_EL1 @ read physical address register (x0 is lower 64-bits and x1 is upper 64-bits).
    
    and x1,x1,0xFFFFFFFFFFF000 @ mask PA bits.
    and x0,x0,#0xFFF @ mask first 12-bits (offset).
    orr x0,x0,x1 @ merge OA and offset to create real PA.

    mrs x3,TTBR0_EL1 @ restore.
    mrs x4,TTBR1_EL1 @ restore.
    isb sy @ wait until synchronizes...
    ret @ return.