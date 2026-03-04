.section .text
.balign 4

.global enable_l1dcache,enable_l1icache,disable_l1dcache,disable_l1icache,enable_l1cache,disable_l1cache,invalidate_setway_l1dcache,clean_invalidate_setway_l1dcache,invalidate_va_poc_l1dcache,invalidate_va_pou_l1dcache,clean_invalidate_va_poc_l1dcache,clean_invalidate_va_pou_l1dcache,invalidate_all_pou_inner_shareable,invalidate_all_pou,invalidate_va_pou

enable_l1dcache:
    mrs x0,SCTLR_EL1 @ read system control register.
    orr x0,x0,#(1<<2) @ set third bit.
    msr SCTLR_EL1,x0 @ apply.

    ret @ return.

enable_l1icache:
    mrs x0,SCTLR_EL1 @ read system control register.
    orr x0,x0,#(1<<12) @ set 12th bit.
    msr SCTLR_EL1,x0 @ apply.

    ret @ return.


disable_l1dcache:
    mrs x0,SCTLR_EL1 @ read system control register.
    bic x0,x0,#(1<<2) @ clear third bit.
    msr SCTLR_EL1,x0 @ apply.

    ret @ return.

disable_l1icache:
    mrs x0,SCTLR_EL1 @ read system control register.
    bic x0,x0,#(1<<12) @ clear 12th bit.
    msr SCTLR_EL1,x0 @ apply.

    ret @ return.

enable_l1cache:
    mrs x0,SCTLR_EL1 @ read system control register.
    orr x1,x1,#(1<<2) @ set third bit.
    orr x1,x1,#(1<<12) @ set 12th bit.
    orr x0,x0,x1 @ clear bits.
    msr SCTLR_EL1,x0 @ apply.

    ret @ return.

disable_l1cache:
    mrs x0,SCTLR_EL1 @ read system control register.
    orr x1,x1,#(1<<2) @ set third bit.
    orr x1,x1,#(1<<12) @ set 12th bit.
    bic x0,x0,x1 @ clear bits.
    msr SCTLR_EL1,x0 @ apply.

    ret @ return.

invalidate_setway_l1dcache:
    dc isw,x0 @ invalidate by set/way.
    ret @ return.

clean_invalidate_setway_l1dcache:
    dc cisw,x0 @ invalidate by set/way.
    ret @ return.

invalidate_va_poc_l1dcache:
    dc ivac @ invalidate by virtual address (poc).
    ret @ return.

invalidate_va_pou_l1dcache:
    dc ivau @ invalidate by virtual address (pou).
    ret @ return.

clean_invalidate_va_poc_l1dcache:
    dc civac @ clean and invalidate by virtual address (poc).
    ret @ return.

clean_invalidate_va_pou_l1dcache:
    dc civau @ clean and invalidate by virtual address (pou).
    ret @ return.

invalidate_all_pou_inner_shareable:
    ic ialluis @ clean and invalidate all (pou and inner-shareable).
    ret @ return.

invalidate_all_pou:
    ic iallu @ clean and invalidate all (pou).
    ret @ return. 

invalidate_va_pou:
    ic ivau,x0 @ clean and invalidate by virtual address (pou).
    ret @ return.