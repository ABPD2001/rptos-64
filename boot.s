.section init
.equiv EL1_CORE_STACK_TABLE,
.global core_spinlock,cinit

minit:
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

.section vectors
.equiv EL1_CUR_HANDLER_TABLE,
.equiv EL1_CUR_IRQ_HANDLER_TABLE,
.equiv EL1_CUR_FIQ_HANDLER_TABLE,

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
    mrs x3,DAIF_EL1
    and x3,0b1011 @ enable IRQs
    msr DAIF_EL1,x3 @ apply
.endm

.macro _exception_fiq_enable
    mrs x3,DAIF_EL1
    and x3,0b0111 @ enable FIQs
    msr DAIF_EL1,x3 @ apply
.endm

.macro _serror_panic
@handle
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
    mrs x0,ESR_EL1 @ read ESR_EL1

    mov x1,x0
    mov x2,x0
    
    bfc x1,xzr,#0,#26 @ seperate class of sync
    lsr x1,#26 
    bfc x2,xzr,#25,#6 @ serperate instruction of sync

    bl determine_id

    ldr x19,=__sync_same_el_table_start__
    mul x20,x20,#4 @ calculate callback relative address of table.
    add x19,x19,x20

    mov x0,x1 @ pass ESR_EL1 to handler

    bl x19 # call the callback (c handler)

    b RETURN_TO_TASK
    
    @ IRQ/vIRQ exception (reentrant)
    .balign 128
    _exception_entry

    @ read GIC-400 for IRQ id.

    ldr x1,=__irq_table__
    mul x0,#4 @ calculate callback relative address of table.
    add x1,x1,x0

    _exception_irq_enable

    bl x1 # call the callback (c handler)

    b RETURN_TO_TASK
    
    @ FIQ/vFIQ exception (reentrant)
    .balign 128
    _exception_entry

    @ read GIC-400 for FIQ id.

    ldr x1,=__fiq_table__
    mul x0,#4 @ calculate callback relative address of table.
    add x1,x1,x0

    _exception_fiq_enable

    bl x1 # call the callback (c handler)

    b RETURN_TO_TASK
    
    @ SError/vSError exception (reentrant)
    .balign 128
    _serror_panic



    @ <--- LOWER EXECUTION LEVEL with SPx (Aarch64) --->
    @ synchronous exception
    .balign 128
    _exception_entry
    mrs x0,ESR_EL1 @ read ESR_EL1

    mov x1,x0
    mov x2,x0
    
    bfc x1,xzr,#0,#26 @ seperate class of sync
    lsr x1,#26 
    bfc x2,xzr,#25,#6 @ serperate instruction of sync

    bl determine_id

    ldr x19,=__sync_same_el_table_start__
    mul x20,x20,#4 @ calculate callback relative address of table.
    add x19,x19,x20

    mov x0,x1 @ pass ESR_EL1 to handler

    bl x19 # call the callback (c handler)

    b EL1_LOWER_RETURN
    
    @ IRQ/vIRQ exception
    .balign 128
    _exception_entry
    @ read GIC-400 for IRQ id.

    ldr x1,=__irq_table__
    mul x0,#4 @ calculate callback relative address of table.
    add x1,x1,x0
    
    _exception_irq_enable

    bl x1 # call the callback (c handler)
    b EL1_LOWER_RETURN

    @ FIQ/vFIQ exception
    .balign 128
    _exception_entry
    @ read GIC-400 for FIQ id.

    ldr x1,=__fiq_table__
    mul x0,#4 @ calculate callback relative address of table.
    add x1,x1,x0
    
    _exception_fiq_enable

    bl x1 # call the callback (c handler)
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

.section vector_table_handlers
RETURN_TO_TASK:
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
.ltorg