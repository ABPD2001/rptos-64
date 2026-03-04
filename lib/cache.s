.section .text
.balign 4

.global enable_l1dcache,enable_l1icache,disable_l1dcache,disable_l1icache,enable_l1cache,disable_l1cache

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