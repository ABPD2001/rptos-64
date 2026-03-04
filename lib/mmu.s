.section .text
.balign 4

.global enable_mmu,disable_mmu,set_ttbr0,set_ttbr1

enable_mmu:
    mrs x0,SCTLR_EL1 @ read system control register.
    orr x0,#0x1 @ set first bit. (mmu enable)
    msr SCTLR_EL1,x0 @ apply.

    ret @ return.

disable_mmu:
    mrs x0,SCTLR_EL1 @ read system control register.
    bic x0,#0x1 @ clear first bit. (mmu enable)
    msr SCTLR_EL1,x0 @ apply.

    ret @ return.

set_ttbr0:
    msr TTBR0_EL1,x0 @ set Translation Table Base Register 0.
    ret @ return.

set_ttbr1:
    msr TTBR1_EL1,x0 @ set Translation Table Base Register 1.
    isb @ wait until apply.
    ret @ return.

mmu_configuration:
    cmp x2,#0 @ compare.
    cset x2,EQ @ set bool value.

    mrs x3,SCTLR_EL1 @ read system control register value.
    orr x3,x3,x2 @ set mmu enabltion.

    msr TTBR0_EL1,x0 @ set Translation Table Base Register 0.
    msr TTBR1_EL1,x1 @ set Translation Table Base Register 1.
    msr SCTLR_EL1,x3 @ apply mmu enablation.
    isb @ wait until everything synchronizes.

    ret @ return.