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
    mrs x3,TTBR0_EL1
    and x0,x0,#0x1FFFFFFFFFF @ mask base address 0.
    and x1,x1,#0x3 @ first two bits.
    
    cmp x2,#1
    cset x2,GE @ boolean value of cnp.

    lsr x0,#4 @ shift to left (BADDR).
    lsr x1,#1 @ shift to left (SKL).

    orr x0,x0,x1 @ merge BADDR and SKL.
    orr x0,x0,x2 @ merge CNP and others.

    msr TTBR0_EL1,x0 @ apply.
    isb sy @ wait until everything synchronizes...
    ret @ return.

set_ttbr1:
    mrs x3,TTBR1_EL1
    and x0,x0,#0x1FFFFFFFFFF @ mask base address 0.
    and x1,x1,#0x3 @ first two bits.
    
    cmp x2,#1
    cset x2,GE @ boolean value of cnp.

    lsr x0,#4 @ shift to left (BADDR).
    lsr x1,#1 @ shift to left (SKL).

    orr x0,x0,x1 @ merge BADDR and SKL.
    orr x0,x0,x2 @ merge CNP and others.

    msr TTBR1_EL1,x0 @ apply.
    isb sy @ wait until everything synchronizes...
    ret @ return.

set_ttbr0_dry:
    mrs x3,TTBR0_EL1
    and x0,x0,#0x1FFFFFFFFFF @ mask base address 0.
    and x1,x1,#0x3 @ first two bits.
    
    cmp x2,#1
    cset x2,GE @ boolean value of cnp.

    lsr x0,#4 @ shift to left (BADDR).
    lsr x1,#1 @ shift to left (SKL).

    orr x0,x0,x1 @ merge BADDR and SKL.
    orr x0,x0,x2 @ merge CNP and others.

    msr TTBR0_EL1,x0 @ apply.
    ret @ return.

set_ttbr1_dry:
    mrs x3,TTBR1_EL1
    and x0,x0,#0x1FFFFFFFFFF @ mask base address 0.
    and x1,x1,#0x3 @ first two bits.
    
    cmp x2,#1
    cset x2,GE @ boolean value of cnp.

    lsr x0,#4 @ shift to left (BADDR).
    lsr x1,#1 @ shift to left (SKL).

    orr x0,x0,x1 @ merge BADDR and SKL.
    orr x0,x0,x2 @ merge CNP and others.

    msr TTBR1_EL1,x0 @ apply.
    ret @ return.

mmu_configuration:
    and x6,x0,#0x1FFFFFFFFFF @ mask base address 0.
    and x7,x1,#0x1FFFFFFFFFF @ mask base address 1.
    
    and x8,x4,#0xF @ mask CnP.
    cmp x8,#1
    cset x8,GE

    and x9,x5,#0x3 @ first two bits (SKL0).
    and x10,x2,#0xFF @ first byte (ASID 0).

    lsr x6,#4 @ shift to left (BADDR 0).
    lsr x9,#1 @ shift to left (SKL0).
    lsr x10,#47 @ shift to left (ASID 0).

    orr x6,x6,x8 @ merge CnP 0 and BADDR 0.
    orr x6,x6,x9 @ merge SKL0 and others of 0.
    orr x6,x6,x10 @ merge ASID 0 and others of 0.

    msr TTBR0_EL1,x6 @ set TTBR0_EL1

    and x8,x4,#0xF0 @ mask CnP 1.
    cmp x8,#1
    cset x8,GE

    and x9,x5,#0xC @ second two bits (SKL1).
    and x10,x2,#0xFF00 @ second byte (ASID 1).

    lsr x7,#4 @ shift to left (BADDR 1).
    lsr x9,#1 @ shift to left (SKL1).
    lsr x10,#47 @ shift to left (ASID 1).
    
    orr x7,x7,x8 @ merge CnP 1 and BADDR 1.
    orr x7,x7,x9 @ merge SKL1 and others of 1.
    orr x7,x7,x10 @ merge ASID 1 and others of 1.

    msr TTBR1_EL1,x7 @ set TTBR1_EL1.

    cmp x3,#1
    cset x0,GE

    mrs x1,SCTLR_EL1
    orr x1,x1,x0 @ set mmu enablation.
    msr SCTLR_EL1,x1 @ apply.

    isb sy @ wait until eveything synchronizes...
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

mmu_initialize:
    ret

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
