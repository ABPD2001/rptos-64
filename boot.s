.section init
.equiv EL1_CORE_STACK_TABLE,

init:
    mrs #__vector_table__,VBAR_EL1 @ set vector base address.

    msr x0,MPIDR_EL1 @ read core id.
    and x0,x0,#0xFF @ get core 0 id (first 8-bits).
    
    ldr x1,__core_stack_table__ @ load core stack table base.
    mul x2,x0,#8 @ calculate relative address of core stack table.
    add x1,x1,x2
    ldr sp,[x1] @ load core stack pointer.

    cbnz x0,core_spinlock @ if core id wasnt zero, jump to (lock cores except core 0).
    
    b kernel @ load kernel (core 0).

core_spinlock:
    wfe
    ldr x0,[#__core_stack_table__,#0x1B] @ read if multi-core is enabled.
    cbz x0,core_spinlock @ if not enabled, wait again.
.ltorg

.section cores_info
.org 0x00, .double  @ core 0 stack info
.org 0x08, .double  @ core 1 stack info
.org 0x10, .double  @ core 2 stack info
.org 0x18, .double  @ core 3 stack info
.org 0x1B, .word @ multi-core enable.
.ltorg

.section vectors
.equiv EL1_CUR_HANDLER_TABLE,
.equiv EL1_CUR_IRQ_HANDLER_TABLE,
.equiv EL1_CUR_FIQ_HANDLER_TABLE

.equiv SYSTEM_TIMER_BASE,#0x7E003000 @ hardware timer (not ARM Timer).
.equiv SYSTEM_TIMER_CS,#0x00
.equiv SYSTEM_TIMER_CLO,#0x04
.equiv SYSTEM_TIMER_CHI,#0x08
.equiv SYSTEM_TIMER_C0,#0x0C

@ .equiv AUX_BASE,#0x7E215000 @ AUX Base address (for SPI1/SPI2/MINI-UART).
@ .equiv AUX_IRQ,#0x00
@ .equiv AUX_MU_IIR_REG,#0x48

.macro _exception_entry
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
    
    msr x0,SPSR_EL1 @ read pstate fields
    msr x1,ELR_EL1 @ read exception link register
    
    stp x30,x0,[sp,#-16]
    stp x1,x1,[sp,#-16] @ save ELR with padding
.endm

.macro _exception_irq_enable
    msr x3,DAIF_EL1
    and x3,0b1011 @ enable IRQs
    mrs x3,DAIF_EL1 @ apply
.endm

.macro _exception_fiq_enable
    msr x3,DAIF_EL1
    and x3,0b0111 @ enable FIQs
    mrs x3,DAIF_EL1 @ apply
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
    msr ESR_EL1,x0 @ read ESR_EL1

    mov x1,x0
    mov x2,x0
    
    bfc x1,xzr,#0,#26 @ seperate class of sync
    lsr x1,#26 
    bfc x2,xzr,#25,#6 @ serperate instruction of sync

    bl determine_id

    ldr x19,=EL1_LOWER_HANDLER_TABLE
    mul x20,x20,#4 @ calculate callback relative address of table.
    add x19,x19,x20

    mov x0,x1 @ pass ESR_EL1 to handler

    bl x19 # call the callback (c handler)

    b EL1_CUR_RETURN
    
    @ IRQ/vIRQ exception (reentrant)
    .balign 128
    _exception_entry

    @ read GIC-400 for IRQ id.

    ldr x1,=EL1_CUR_IRQ_HANDLER_TABLE
    mul x0,#4 @ calculate callback relative address of table.
    add x1,x1,x0

    _exception_irq_enable

    bl x1 # call the callback (c handler)

    b EL1_CUR_RETURN
    
    @ FIQ/vFIQ exception (reentrant)
    .balign 128
    _exception_entry

    @ read GIC-400 for FIQ id.

    ldr x1,=EL1_CUR_FIQ_HANDLER_TABLE
    mul x0,#4 @ calculate callback relative address of table.
    add x1,x1,x0

    _exception_fiq_enable

    bl x1 # call the callback (c handler)

    b EL1_CUR_RETURN
    
    @ SError/vSError exception (reentrant)
    .balign 128
    _serror_panic

    @ <--- LOWER EXECUTION LEVEL with SPx (Aarch64) --->
    @ synchronous exception
    .balign 128
    _exception_entry
    msr ESR_EL1,x0 @ read ESR_EL1

    mov x1,x0
    mov x2,x0
    
    bfc x1,xzr,#0,#26 @ seperate class of sync
    lsr x1,#26 
    bfc x2,xzr,#25,#6 @ serperate instruction of sync

    bl determine_id

    ldr x19,=EL1_LOWER_HANDLER_TABLE
    mul x20,x20,#4 @ calculate callback relative address of table.
    add x19,x19,x20

    mov x0,x1 @ pass ESR_EL1 to handler

    bl x19 # call the callback (c handler)

    b EL1_LOWER_RETURN
    
    @ IRQ/vIRQ exception
    .balign 128
    _exception_entry
    @ read GIC-400 for IRQ id.

    ldr x1,=EL1_LOWER_IRQ_HANDLER_TABLE
    mul x0,#4 @ calculate callback relative address of table.
    add x1,x1,x0
    
    _exception_irq_enable

    bl x1 # call the callback (c handler)
    b EL1_LOWER_RETURN

    @ FIQ/vFIQ exception
    .balign 128
    _exception_entry
    @ read GIC-400 for FIQ id.

    ldr x1,=EL1_LOWER_FIQ_HANDLER_TABLE
    mul x0,#4 @ calculate callback relative address of table.
    add x1,x1,x0
    
    _exception_fiq_enable

    bl x1 # call the callback (c handler)
    bl EL1_LOWER_RETURN

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
EL1_CUR_RETURN:
    ldp x0,x0,[sp,#16] @ read ELR_EL1
    mrs x0,ELR_EL1 @ apply ELR_EL1.
    ldp x30,x0,[sp],#16 @ read x30 and SPSR_EL1    
    mrs x0,SPSR_EL1 @ apply spsr.
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

EL1_LOWER_RETURN:
    @ x1 has current running pcb of core.
    add sp,sp,#16 @ skip ELR_EL1, becuase there is no return to task.
    ldp x30,x0,[sp],#16

    stp x0,x30,[x1],#16 @ save SPSR_EL1 and x30 into pcb.
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
    
    stp x29,x28,[x1],#16
    stp x27,x26,[x1],#16
    stp x25,x24,[x1],#16
    stp x23,x22,[x1],#16
    stp x21,x20,[x1],#16
    stp x19,x18,[x1],#16
    stp x17,x16,[x1],#16
    stp x15,x14,[x1],#16
    stp x13,x12,[x1],#16
    stp x11,x10,[x1],#16
    stp x9,x8,[x1],#16
    stp x7,x6,[x1],#16
    stp x5,x4,[x1],#16
    stp x3,x2,[x1],#16
    
    ldp x2,x3,[sp],#16 @ load x0 and x1 into temp registers.
    stp x2,x3,[x1],#16 @ store it on pcb (this line completes the whole context save).
    
    bl task_schaduler
    bl task_dispatcher

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