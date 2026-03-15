.section irq_table
.balign 4
.org 0x00,core_generic_timer
.org 0x04,aux_main_routine
.org 0x08,system_timer1
.org 0x0C,uart_main_routine @ uart
.org 0x10,sgi_main_routine 
.ltorg

.section .irq_handlers
.balign 4
.equiv GICD_BASE,0xFF841000
.equiv GICD_SGIR,0xF00

.equiv AUX_BASE,0x7e215000
.equiv AUX_LSR_REG,0x54
.equiv AUX_IRQ,0x00
.equiv AUX_IIR_REG,0x48
.equiv AUX_MU_IO,0x40

core_generic_timer: @ generic timer of core gonna used for task schaduling...
    mrs x1,MPIDR_EL1 @ read cores info.
    and x1,#0xFF @ mask only id.

    ldr x0,=__core_info_table__
    add x0,#32 @ skip stacks table.

    mov x2,#8
    mul x1,x1,x2 @ calculate relative address of cores task table.
    add x1,x1,x0 @ calculate PCB address of this core.

    add sp,sp,#16 @ skip ELR_EL1 + padding, becuase there is no return to task.
    
    ldp x0,x0,[sp],#16 @ read ELR_EL1.
    str x0,[x1,#344] @ store task pc.
    
    mrs x0,TTBR0_EL1 @ read ttbr0.
    str x0,[x1,#348] @ store ttbr of task.

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
    
    mrs x0,SP_EL0 @ read stack pointer of task.
    str x0,[x1],#8 @ store sp on pcb.
    
    mov x0,x1 @ pass pcb address to first paramter.

    bl task_schaduler   
    bl task_dispatcher

system_timer1: @ handler by core 0 only.
    @ handler for incrementing global system timer ticks and schaduling timer services.
    ldr x0,=__global_timer_ticks__ @ load.
    add x0,x0,#1 @ increment.
    str x0,=__global_timer_ticks__ @ store.
    @ send each a broadcast to all cores. (sgi).

    ldr x0,=GICD_BASE @ load GICD base address.
    ldr w1,=0b000000010000000010000000000010000 @ set value.
    str w1,[x0,#GICD_SGIR] @ store (signal on mmio).

    b wakeup_service @ start wakeup service.
    ret @ done.

aux_main_routine_end:
    ldr x1,[x0,#AUX_IIR_REG] @ read interrupt register of aux.
    bic x1,x1,(0b11<<1) @ clear interrupts.
    str x1,[x0,#AUX_IIR_REG] @ apply.

    msr daifset,#0x2 @ disable IRQs.

    ldr x0,=__generic_base_irq_statistics__

    mrs x1,MPIDR_EL1 @ read core id.
    and x1,x1,#0xFF @ mask core id.

    mov x2,#2
    mul x1,x1,x2 @ calculate realtive address of base.
    add x0,x0,x1 @ calculate absolute address of base.

    ldr x1,[x0] @ read global statistic.
    add x1,x1,#1 @ increment global statistic.
    str x1,[x0] @ store global statistic.

    msr daifclr,#0x2 @ enable IRQs.

    ret @ return to main handler.

aux_main_routine:
    stp x30,x30,[sp,#-16]! @ store link register to main handler.
    
    ldr x0,=AUX_BASE
    ldr x1,[x0,#AUX_IRQ] @ read general interrupt status of aux.
    
    and x1,x1,#0x1 @ check if interrupt bit is set.
    cbz x1,aux_main_routine_end @ else end to routine.

    ldr x1,[x0,#AUX_IIR_REG] @ read interrupt status register of mini uart.
    and x2,x1,#0x1 @ check if interrupt bit is set.
    cbnz x1,aux_main_routine_end  @ if it wasnt zero (not logic) end to routine.
    
    and x2,x1,#0x6 @ mask interrupt id of mini uart.
    cbz x1,aux_main_routine_end @ end to routine if wasnt any interrupt id.

    cmp x2,#0x4 @ check if it is rx id.
    adr x30,. @ set link register.
    add x30,x30,#8 @ skip two instructions (add & branch).
    b.eq muart_valid_byte @ call routine.

    cmp x2,#0x2 @ check if is tx id.
    adr x30,. @ set link register.
    add x30,x30,#8 @ skip two instructions (add & branch).
    b.eq muart_tx_empty @ call routine.

    ldr x1,[x0,#AUX_IIR_REG] @ read interrupt status.
    bic x1,x1,#(0b11<<1) @ clear interrupt masks.
    str x1,[x0,#AUX_IIR_REG] @ apply to interrupt status.

    ldp x30,x30,[sp,#-16]! @ restore link register to main handler.
    ret @ return to main handler.

sgi_main_routine_end:
    msr daifset,#0x2 @ disable IRQs.

    ldr x0,=__generic_base_irq_statistics__

    mrs x1,MPIDR_EL1 @ read core id.
    and x1,x1,#0xFF @ mask core id.

    mov x2,#2
    mul x1,x1,x2 @ calculate realtive address of base.
    add x0,x0,x1 @ calculate absolute address of base.

    ldr x1,[x0,#8] @ read oop_sgis_count
    add x1,x1,#1 @ increment oop_sgis_count.
    str x1,[x0,#8] @ apply oop_sgis_count.

    msr daifclr,#0x2 @ disable IRQs.

    ret @ return to main handler.


sgi_main_routine:
    stp x30,x30,[sp,#-16]! @ store link register to main handler.
    
    adr x30,. @ set link register.
    add x30,x30,#8 @ skip five instructions (add & cmp & branch & cmp & branch).
    cmp x0,#7
    b.eq wakeup_service @ wake-up service.

    cmp x0,#8
    b.eq core_terminate @ system termination.

    b.ne sgi_main_routine_end

    ldp x30,x30,[sp,#-16]! @ restore link register to main handler.
    ret @ return to main handler.

uart_main_routine:
    ret @ dummy.
.ltorg
