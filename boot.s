
.section init
.equiv EL1_STACK,
reset_handler:
    ldr sp,=EL1_STACK
    

.ltorg

.section vectors
.equiv EL1_CUR_SYNC_HANDLER_TABLE,
.equiv EL1_CUR_IRQ_HANDLER_TABLE,
.equiv EL1_CUR_FIQ_HANDLER_TABLE

.equiv SYSTEM_TIMER_BASE,#0x7E003000 @ hardware timer (not ARM Timer).
.equiv SYSTEM_TIMER_CS,#0x00
.equiv SYSTEM_TIMER_CLO,#0x04
.equiv SYSTEM_TIMER_CHI,#0x08
.equiv SYSTEM_TIMER_C0,#0x0C

.equiv AUX_BASE,#0x7E215000
.equiv AUX_IRQ,#0x00
.equiv AUX_MU_IIR_REG,#0x48

vector_table:
    .balign 128
    eret

    .balign 128
    eret

    .balign 128
    eret

    .balign 128
    eret

    @ CURRENT EXECUTION LEVEL with SPx
    @ synchronous exception
    .balign 128
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
    
    ldp x30,x0,[sp,#-16]
    ldp x1,x1,[sp,#-16] @ save ELR with padding

    mov x1,x0
    mov x2,x0
    
    bfc x1,xzr,#0,#26 @ seperate class of sync
    lsr x1,#26 
    bfc x2,xzr,#25,#6 @ serperate instruction of sync

    bl determine_id_sync

    mov x19,=EL1_CUR_SYNC_HANDLER_TABLE
    mul x19,#4 @ calculate callback relative address of table.
    mov pc,x19 # call the callback (c handler)

    b EL1_CUR_SYNC_RETURN
    @ IRQ/vIRQ exception (reentrant)
    .balign 128
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
    
    ldp x30,x0,[sp,#-16]
    ldp x1,x1,[sp,#-16] @ save ELR with padding

    @ read GIC-400 for IRQ id.

    ldr x1,=EL1_CUR_IRQ_HANDLER_TABLE
    mul x0,#4 @ calculate callback relative address of table.
    add x1,x1,x0

    msr x3,DAIF_EL1
    and x3,0b1011 @ enable IRQs
    mrs x3,DAIF_EL1 @ apply

    bl x1 # call the callback (c handler)

    b EL1_CUR_SYNC_RETURN
    @ FIQ/vFIQ exception (reentrant)
    .balign 128
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
    
    ldp x30,x0,[sp,#-16]
    ldp x1,x1,[sp,#-16] @ save ELR with padding

    @ read GIC-400 for FIQ id.

    ldr x1,=EL1_CUR_FIQ_HANDLER_TABLE
    mul x0,#4 @ calculate callback relative address of table.
    add x1,x1,x0

    msr x3,DAIF_EL1
    and x3,0b0111 @ enable IRQs
    mrs x3,DAIF_EL1 @ apply

    bl x1 # call the callback (c handler)

    b EL1_CUR_SYNC_RETURN
    @ SError/vSError exception (reentrant)
    .balign 128

.ltorg

.section vector_table_handlers
EL1_CUR_SYNC_RETURN:
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

determine_id_sync:
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