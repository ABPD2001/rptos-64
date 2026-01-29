.equiv EL1_STACK,
.equiv EL1_CUR_SYNC_HANDLER_TABLE,

.section init
reset_handler:
    ldr sp,=EL1_STACK
    

.ltorg

.section vectors
vector_table:
    .balign 128
    eret

    .balign 128
    eret

    .balign 128
    eret

    .balign 128
    eret

    @ current el with spx

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
    
    msr x3,DAIF_EL1
    and x3,0b1101 @ enable IRQs
    mrs x3,DAIF_EL1 @ apply

    mov x19,=EL1_CUR_SYNC_HANDLER_TABLE
    mul x19,#4 @ calculate isr relative address of table.
    mov pc,x19 # call the isr (c handler)

    b EL1_CUR_SYNC_RETURN
    
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


.ltorg