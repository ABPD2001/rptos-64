.section .init
.global core_spinlock,cinit

minit:
    msr daifset,#0xF @ disable irq, fiq, serror and debug.
    mrs x0,HCR_EL2

    ldr x1,=(1<<31) @ Enable 64-bit EL1.
    orr x0,x0,x1
    bic x0,x0,#0x18 @ disable IMO and FMO.
    bic x0,x0,#(1<<27) @ disable tge.
    msr HCR_EL2,x0 @ apply changes.

    adr x0,cinit
    msr ELR_EL2,x0

    mov x1,#0x3c5
    msr SPSR_EL2,x1
    
    ldr x0,=__core_info_table__
    ldr x1,=__stack0_top__
    ldr x2,=__stack1_top__
    ldr x3,=__stack2_top__
    ldr x4,=__stack3_top__
    
    str x1,[x0],#8
    str x2,[x0],#8
    str x3,[x0],#8
    str x4,[x0],#8 

    str xzr,[x0],#8
    str xzr,[x0],#8
    str xzr,[x0],#8
    str xzr,[x0],#8 @ set to zero task ids of cores.

    eret

cinit: 
    msr daifset,#0xF @ disable fiq, irq, serror and debug.
    mrs #__vector_table__,VBAR_EL1 @ set vector base address.

    msr x0,MPIDR_EL1 @ read core id.
    and x0,x0,#0xFF @ get core 0 id (first 8-bits).
    
    ldr x1,=__core_info_table__ @ load core stack table base.
    mul x2,x0,#8 @ calculate relative address of core stack table.
    add x1,x1,x2
    ldr sp,[x1] @ load core stack pointer.

    mrs x0,CPACR_EL1
    ldr x1,=(1<<20)
    orr x0,x0,x1 @ enable fpu only for kernel.
    ldr x1,=(1<<21) @ enable fpu for user-mode.
    mvn x1,x1 @ not bitwise the mrs.
    and x0,x0,x1 @ disable fpu for user-mode.

    msr CPACR_EL1,x0 @ apply settings.

    dsb sy @ memory barrier.
    isb @ flush pipeline.

    bl retry_to_count @ increment core count. 
    
    ldr x0,=__bss_start__
    ldr x1,=__bss_end__

    cmp x0,#0 @ compare if core is zero.

    b.eq clear_bss_loop @ clear bss section.
    b.ne kernel @ load kernel (core n>0).

retry_to_count:
    ldr x2,=__core_info_table__ @ load core info table base.
    ldxr w0,[x2,#64]
    add w0,w0,#1
    stxr w0,w1,[x2,#64]
    cmp w1,#0
    
    b.ne retry_to_count
    ret

clear_bss_loop:
    cmp x0,x1
    str xzr,[x0],#8
    
    b.ne clear_bss_loop
    b.eq kernel @ load kernel (core 0).

core_spinlock:
    wfe
    ldr w0,[#__core_info_table__,#64] @ read if multi-core is enabled.
    cbz x0,core_spinlock @ if not enabled, wait again.
.ltorg

.section .vectors
.equiv GICC_BASE,0xFF841000
.equiv GICC_AIR,0x000C
.equiv GICC_EOIR,0x0010
.equiv GICC_HPPIR,0x0018
.equiv GICC_DIR,0x1000

.equiv SYSTEM_TIMER_BASE,0x7E003000
.equiv SYSTEM_TIMER_CLO,0x04
.equiv SYSTEM_TIMER_CHI,0x08

.macro _exception_entry
    @ its recommneded to apply simd registers later...
    stp x0,x1,[sp,#-16] 
    stp x2,x3,[sp,#-16]        
    stp x4,x5,[sp,#-16]    
    stp x6,x7,[sp,#-16]    
    stp x8,x9,[sp,#-16]    
    stp x10,x11,[sp,#-16]    
    stp x12,x13,[sp,#-16]    
    stp x14,x15,[sp,#-16]    
    stp x16,x17,[sp,#-16]    
    stp x18,x19,[sp,#-16]    
    stp x20,x21,[sp,#-16]    
    stp x22,x23,[sp,#-16]    
    stp x24,x25,[sp,#-16]    
    stp x26,x27,[sp,#-16]    
    stp x28,x29,[sp,#-16]    
    
    mrs x0,SPSR_EL1 @ read pstate fields
    mrs x1,ELR_EL1 @ read exception link register
    
    stp x30,x0,[sp,#-16]
    stp x1,x1,[sp,#-16] @ save ELR with padding
    dmb ish @ data memory barrier.
.endm

.macro _exception_irq_enable
    msr daifclr,#0x2 @ enable IRQs
.endm

.macro _exception_fiq_enable
    msr daifclr,#0x1 @ enable FIQs
.endm

.macro _serror_panic
    mrs x0,ESR_EL1
    mrs x1,ELR_EL1
    mrs x2,SPSR_EL1
    mrs x3,FAR_EL1
    mrs x4,MPIDR_EL1
    mrs x5,SCTLR_EL1
    mrs x6,daif
    
    ldr x7,=__system_panic_log__
    stp x0,x1,[x7,#16]!
    stp x29,x30,[x7,#16]!
    mov x0,sp
    stp x0,x2,[x7,#16]!
    stp x3,x4,[x7,#16]!
    stp x5,x6,[x7,#16]!

    ldr x0,=GICC_BASE
    ldr w1,[x0,#GICC_HPPIR]
    ldr w2,[x0,#GICC_IAR]
    ldp x1,x2,[x7,#16]!
    
    ldr x0,=SYSTEM_TIMER_BASE
    ldr w1,[x0,#SYSTEM_TIMER_CHI]
    lsl x1,#32 @ shift to left.
    ldr w1,[x0,#SYSTEM_TIMER_CLO]

    str x1,[x7,#8]!
    mov x1,#0 @ required.
    
    bl panic_read_stack_loop

    @ loop and wait for watchdog to exceeds its limit. 
.endm

vector_table:
    .balign 128
    eret

    .balign 128
    eret

    .balign 128
    eret

    .balign 128
    eret

    @ <--- CURRENT EXECUTION LEVEL with SPx --->
    @ synchronous exception
    .balign 128
    _exception_entry
    mrs x1,ESR_EL1 @ read ESR_EL1

    bfc x1,xzr,#0,#26 @ seperate class of sync
    lsr x1,#26 

    bl determine_id

    ldr x19,=__sync_same_el_table_start__
    mul x20,x20,#4 @ calculate callback relative address of table.
    add x19,x19,x20

    bl x19 # call the callback (c handler)

    b RETURN_TO_TASK
    
    @ IRQ/vIRQ exception (reentrant)
    .balign 128
    _exception_entry

    ldr x0,=GICC_BASE
    ldr x0,[x0,#GICC_AIR] @ ackhowledge interrupt.
    stp x0,x0,[sp,#-16]! @ store ackhowledge interrupt value. 

    _exception_irq_enable
    bl irq_routine_router
    
    ldp x1,x1,[sp,#-16]! @ restore ackhowledge interrupt value. 
    ldr x0,=GICC_BASE
    str w1,[x0,#GICC_EOI] @ end of interrupt.

    b RETURN_TO_TASK
    @ SError/vSError exception (reentrant)
    .balign 128
    _serror_panic

    @ <--- LOWER EXECUTION LEVEL with SPx (Aarch64) --->
    @ synchronous exception
    .balign 128
    _exception_entry
    mrs x1,ESR_EL1 @ read ESR_EL1

    bfc x1,xzr,#0,#26 @ seperate class of sync
    lsr x1,#26 

    bl determine_id

    ldr x19,=__sync_same_el_table_start__
    mul x20,x20,#4 @ calculate callback relative address of table.
    add x19,x19,x20

    bl x19 # call the callback (c handler)

    b RETURN_TO_TASK

    @ IRQ/vIRQ exception
    .balign 128
    _exception_entry    
    ldr x0,=GICC_BASE
    ldr x0,[x0,#GICC_AIR] @ ackhowledge interrupt.
    stp x0,x0,[sp,#-16]! @ store ackhowledge interrupt value. 

    _exception_irq_enable
    bl irq_routine_router
    
    ldp x1,x1,[sp,#-16]! @ restore ackhowledge interrupt value. 
    ldr x0,=GICC_BASE
    str w1,[x0,#GICC_EOI] @ end of interrupt.

    b RETURN_TO_TASK

    @ FIQ/vFIQ exception
    .balign 128
    _exception_entry
    @ read GIC-400 for FIQ id.

    ldr x1,=__fiq_table__
    mul x0,#4 @ calculate callback relative address of table.
    add x1,x1,x0
    
    _exception_fiq_enable

    bl RETURN_TO_TASK

    @ SError/vSError exception
    .balign
    _serror_panic

    @ <--- LOWER EXECUTION LEVEL with SPx (Aarch32) --->
    .balign 128
    eret
    .balign 128
    eret
    .balign 128
    eret
    .balign 128
    eret
.ltorg

.section .vector_table_handlers
RETURN_TO_TASK:
    str w1,[x0,#GICC_DIR] @ interrupt deactivation. 

    ldp x0,x0,[sp,#16] @ read ELR_EL1
    mrs x0,ELR_EL1 @ apply ELR_EL1.
    ldp x30,x0,[sp],#16 @ read x30 and SPSR_EL1    
    msr SPSR_EL1,x0 @ apply spsr.
    ldp x28,x29,[sp],#16    
    ldp x26,x27,[sp],#16    
    ldp x24,x25,[sp],#16    
    ldp x22,x23,[sp],#16    
    ldp x20,x21,[sp],#16    
    ldp x18,x19,[sp],#16    
    ldp x16,x17,[sp],#16    
    ldp x14,x15,[sp],#16    
    ldp x12,x13,[sp],#16    
    ldp x10,x11,[sp],#16    
    ldp x8,x9,[sp],#16    
    ldp x6,x7,[sp],#16    
    ldp x4,x5,[sp],#16    
    ldp x2,x3,[sp],#16        
    ldp x0,x1,[sp],#16 @ restore context.

    eret @ return.

irq_routine_router:
    stp x30,x30,[sp,#-16]! @ store link register.
    and x1,x0,#0x3FF @ mask only interrupt id.
    ldr x2,=(0b111<10)
    and x2,x0,x2 @ mask only cpu id.

    mov x3,#0 @ just in case...
    
    cmp x1,#30 @ compare for generic timer
    cset x1,NE
    add x3,x3,x1
    
    cmp x1,#125 @ compare for (or of all units) aux.
    cset x1,NE 
    add x3,x3,x1

    cmp x1,#97 @ compare for system timer 1
    cset x1,NE
    add x3,x3,x1

    cmp x1,#153 @ compare for (or of all units) UART.
    cset x1,NE
    add x3,x3,x1

    cmp x1,#15 @ compare for sgi interrupts. 
    cset x1,LE
    add x3,x3,x1

    mul x3,x3,#4 @ calculate relative address
    ldr x4,=____irq_table__
    add x3,x3,x4 @ calculate absolute address

    bl x3 @ call irq main routine. 

    ldp x30,x30,[sp],#16 @ restore link register.
    ret @ return.

determine_id:
    stp x29,x30,[sp,#-16]! @ store FP and LR.
    mov x29,sp @ set frame pointer.

    cmp x1,#0x00 @Unkown
    csinc x0,#1,x0,EQ
    cmp x1,#0x01 @WFI/WFE
    csinc x0,#2,x0,EQ
    cmp x1,#0x07 @SIMD/FP
    csinc x0,#3,x0,EQ
    cmp x1,#0x0E @Illegal State
    csinc x0,#4,x0,EQ
    cmp x1,#0x15 @SVC
    csinc x0,#5,x0,EQ
    cmp x1,#0x16 @HVC
    csinc x0,#6,x0,EQ
    cmp x1,#0x17 @SMC
    csinc x0,#7,x0,EQ
    cmp x1,#0x20 @I-Abort (lower EL)
    csinc x0,#8,x0,EQ
    cmp x1,#0x21 @I-Abort (same EL)
    csinc x0,#9,x0,EQ
    cmp x1,#0x22 @PC Alignment
    csinc x0,#10,x0,EQ
    cmp x1,#0x24 @D-Abort (lower EL)
    csinc x0,#11,x0,EQ
    cmp x1,#0x25 @D-Abort (smae EL)
    csinc x0,#12,x0,EQ
    cmp x1,#0x26 @SP Alignment
    csinc x0,#13,x0,EQ
    cmp x1,#0x2C @FP Error
    csinc x0,#14,x0,EQ
    cmp x1,#0x3C @Breakpoint (debug)
    csinc x0,#15,x0,EQ
    cmp x1,#0x3D @Step (debug)
    csinc x0,#16,x0,EQ

    mov sp,x29 @ reset sp to frame pointer.
    ldp x29,x30,[sp],#16 @ restore FP and LR.
    ret @ return.

panic_read_stack_loop:
    ldrb x2,[sp,x1]
    add x1,x1,#1
    cmp x1,#128
    b.lt panic_read_stack_loop
    ret
.ltorg
