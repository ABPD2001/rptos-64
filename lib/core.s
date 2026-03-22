.section .text
.balign 4

.global multi_core_enable,restore_context,enable_irq,enable_fiq,disable_irq,disable_fiq,enable_daif,disable_daif,system_panic,void_event_loop
.equiv SYSTEM_TIMER_BASE,0x7E003000
.equiv SYSTEM_TIMER_CLO,0x04
.equiv SYSTEM_TIMER_CHI,0x08


multi_core_enable:
    stp x29,x30,[sp,#-16]!
    mov x29,sp

    ldr x19,=__init_start__
    str x19,[xzr,#0xd8] @ set core 1 entry point (for firmware).
    str x19,[xzr,#0xe0] @ set core 2 entry point (for firmware).
    str x19,[xzr,#0xe8] @ set core 3 entry point (for firmware).

    dsb sy @ data synchronuzation barrier (outer shareable). 
    
    sev @ send event to all cores.

    mov sp,x29
    ldp x29,x30,[sp],#16
    ret @ return.

restore_core_context:
    ldr x0,[x0] @ point to it self (actual pcb).
    str xzr,[x0,#8]! @ clear vaild property. 

    @ restore from struct.

    ldp x1,x2,[x0,#16]! @ TTBR0, TTBR1
    ldp x3,x4,[x0,#16]! @ TCR, SCTLR
    ldp x5,x6,[x0,#16]! @ FAR, SPSR
    ldp x7,x8,[x0,#16]! @ CNTP_CTL, CNTFRQ
    ldp x9,x10,[x0,#16]! @ CNTP_TVAL, CNTPCT

    @ restore into registers.

    msr TTBR0_EL1,x1
    msr TTBR1_EL1,x2
    msr TCR_EL1,x3
    msr SCTLR_EL1,x4
    msr FAR_EL1,x5
    msr SPSR_EL1,x6
    msr CNTP_CTL_EL1,x7
    msr CNTFRQ_EL1,x8
    msr CNTP_TVAL_EL1,x9
    msr CNTPCT_EL1,x10
    isb sy @ wait until everything synchronizes.

    ret @ return.

save_core_context:
    mrs x0,MPIDR_EL1 @ read MPIDR_EL1.
    and x0,x0,#0xFF @ mask first byte.

    ldr x1,=__cccb_bank_base__ @ read context table base address.
    mov x2,#96
    mul x0,x0,x2 @ calculate relative address to context table.
    add x0,x0,x1 @ calculate absolute address to context table.

    @ read context.
    mrs x1,TTBR0_EL1
    mrs x2,TTBR1_EL1
    mrs x3,TCR_EL1
    mrs x4,SCTLR_EL1
    mrs x5,FAR_EL1
    mrs x6,SPSR_EL1
    mrs x7,CNTP_CTL_EL1
    mrs x8,CNTPFRQ_EL1
    mrs x9,CNTP_TVAL_EL1
    mrs x10,CNTPCT_EL1

    @ set valid property.
    mov x11,#1
    ldr x11,[x0,#8]!

    @ store context.
    ldp x1,x2,[x0,#16]!
    ldp x3,x4,[x0,#16]!
    ldp x5,x6,[x0,#16]!
    ldp x7,x8,[x0,#16]!
    ldp x9,x10,[x0,#16]!

    ret @ return.
    
restore_context:
    @ in this function, we deceive processor core to we are in a exception, and we return to task with "eret".
    @ we use x0 as register pointer and x1 as temporary register.
    ldr x0,[x0] @ point to it self.

    ldp x1,x30,[x0,#16]!
    ldp x28,x29,[x0,#16]!
    ldp x26,x27,[x0,#16]!
    ldp x24,x25,[x0,#16]!
    ldp x22,x23,[x0,#16]!
    ldp x20,x21,[x0,#16]!
    ldp x18,x19,[x0,#16]!
    ldp x16,x17,[x0,#16]!
    ldp x14,x15,[x0,#16]!
    ldp x12,x13,[x0,#16]!
    ldp x10,x11,[x0,#16]!
    ldp x8,x9,[x0,#16]!
    ldp x6,x7,[x0,#16]!
    ldp x4,x5,[x0,#16]!
    ldp x2,x3,[x0,#16]!
    
    ldr x1,[x0,#8]!
    mov sp,x1 @ restore stack pointer.

    msr SPSR_EL1,x1 @ apply spsr.
    add x0,x0,#80 @ skip 10 properties.
    
    ldr x1,[x0,#8]!
    msr ELR_EL1,x1 @ apply pc (into exception link register).
    ldr x1,[x0,#8]!
    msr TTBR0_EL1,x1 @ apply ttbr.
    ldr x1,=0x300007B @ set mmu and other settings...
    msr SCTLR_EL0,x1 @ set SCTLR for el0.

    ldp x0,x1,[x0,#-16]! @ all context is restored.

    eret @ return to task.

disable_daif:
    stp x29,x30,[sp,#-16]!
    mov x29,sp

    msr daifset,#0xF

    ldp x29,x30,[sp],#16
    mov sp,x29
    ret

enable_daif:
    stp x29,x30,[sp,#-16]!
    mov x29,sp

    msr daifclr,#0xF

    ldp x29,x30,[sp],#16
    mov sp,x29
    ret

enable_irq:
    stp x29,x30,[sp,#-16]!
    mov x29,sp

    msr daifclr,#0x2

    ldp x29,x30,[sp],#16
    mov sp,x29
    ret

disable_irq:
    stp x29,x30,[sp,#-16]!
    mov x29,sp

    msr daifset,#0x2

    ldp x29,x30,[sp],#16
    mov sp,x29
    ret

enable_fiq:
    stp x29,x30,[sp,#-16]!
    mov x29,sp

    msr daifclr,#0x1

    ldp x29,x30,[sp],#16
    mov sp,x29
    ret

enable_fiq:
    stp x29,x30,[sp,#-16]!
    mov x29,sp

    msr daifclr,#0x1

    ldp x29,x30,[sp],#16
    mov sp,x29
    ret

system_panic_ksp_loop:
    cmp x0,#128 @ compare to 128.
    b.ge x30 @ back if it was equal/greater

    ldrb x1,[sp,x0] @ read a byte from stack
    strb x1,[x11,#1]! @ store a byte info panic log.

    add x0,x0,#1 @ increment. 
    b system_panic_ksp_loop @ circle back.

system_panic_void_loop:
    wfe
    b system_panic_void_loop

system_panic:
    @ Read important registers.
    mrs x0,ESR_EL1
    mrs x1,ELR_EL1
    mov x2,sp
    mrs x3,SPSR_EL1
    mrs x4,FAR_EL1
    mrs x5,MPIDR_EL1
    mrs x6,SCTLR_EL1
    mrs x7,daif
    ldr x8,=GICC_BASE @ get GIC Interface base address (mmio).
    ldr x9,[x8,#GICC_IAR]
    ldr x8,[x8,#GICC_HPPIR]

    ldr x10,=SYSTEM_TIMER_BASE @ get timer base address (mmio).

    ldr w11,[x10,#SYSTEM_TIMER_CHI] @ read upper 32-bit of timer.
    lsl w11,#32 @ shift to left.
    ldr w11,[x10,#SYSTEM_TIMER_CLO] @ read lower 32-bit of timer.

    mov x10,x11
    ldr x11,=__system_panic_log__

    @ store important registers.

    stp x0,x1,[x11,#16]!
    stp x29,x30,[x11,#16]!
    stp x2,x3,[x11,#16]!
    stp x4,x5,[x11,#16]!
    stp x6,x7,[x11,#16]!
    stp x8,x9,[x11,#16]!
    str x10,[x11,#8]!

    mov x0,#0 @ set x0 to zero for label.
    bl system_panic_ksp_loop @ start loop.

    @ loop and wait for watchdog.
    b void_event_loop

void_event_loop:
    wfe
    b void_event_loop

psci_cpu_off:
    ldr w0,=0x84000002 @ set function id.
    smc #0 @ secure monitor-call.
    ret @ return.

psci_cpu_on:
    @ save paramaters at other registers.
    mov x3,x0
    mov x4,x1
    mov x5,x2
    
    ldr w0,=0xC4000003 @ set function id.
    mrs x2,MPIDR_EL1
    
    and x1,x2,#0xFFFF00 @ mask AFF1, AFF2
    and x2,x2,#0xFF00000000 @ mask AFF3
    orr x1,x1,x2 @ merge.
    and x3,x3,#0xFF @ mask only first byte of parameter.
    orr x1,x1,x3 @ merge.
    
    mov x2,x4
    mov x3,x5

    smc #0 @ secure monitor-call.

    ret @ return.

psci_system_reset:
    ldr w0,=0x84000008 @ set function id.
    smc #0 @ secure monitor-call.
    ret @ return.

psci_system_off:
    ldr w0,=0x84000009 @ set function id.
    smc #0 @ secure monitor-call.
    ret @ return.

psci_cpu_suspend:
    mov x4,x2
    mov x3,x1
    mov x2,x0
    
    ldr w0,=0xC4000001 @ set functio id.
    
    mov x1,#0 @ just in case.
    and x2,x2,#0xFFFF @ mask only first half-word.
    orr x1,x1,x2 @ merge.
    
    cmp x3,#0 @ compare state type.
    cset x2,EQ @ set bool value.
    lsl x2,#16 @ shift to left.

    orr x1,x1,x2 @ merge.

    and x4,x4,#0x3 @ only first two bit is required.
    lsl x4,#24 @ shift to left.

    orr x1,x1,x4 @ merge.

    smc #0 @ secure monitor-call.
    
    ret @ return.
    
cpu_standby_wfe:
    wfe 
    ret @ return.

cpu_standby_wfi:
    wfi
    ret @ return.

.ltorg
