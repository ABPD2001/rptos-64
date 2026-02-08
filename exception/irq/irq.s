.section irq_table:
.org 0x0, b test
.ltorg

.section .irq_handlers

core_generic_timer: @ generic timer of core gonna used for task schaduling...
    mrs x1,MPIDR_EL1 @ read cores info.
    and x1,#0xFF @ mask only id.

    ldr x0,=__core_info_table__
    add x0,#32 @ skip stacks table.

    mul x1,x1,#8 @ calculate relative address of cores task table.
    add x1,x1,x0 @ calculate PCB address of this core.

    add sp,sp,#16 @ skip ELR_EL1 + padding, becuase there is no return to task.
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
    mov x0,x1 @ pass pcb address to first paramter.

    bl task_schaduler   
    bl task_dispatcher

system_timer: @ handler by core 0 only.
    @ handler for incrementing global system timer ticks and schaduling timer services.
    ldr x0,=__global_timer_ticks__ @ load.
    add x0,x0,#1 @ increment.
    str x0,=__global_timer_ticks__ @ store.
    @ send each core a irq (sgi).
    b wakeup_service
    ret @ done.