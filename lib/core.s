.section .text
.balign 4

.global multi_core_enable,restore_context,enable_irq,enable_fiq,disable_irq,disable_fiq,enable_daif,disable_daif

multi_core_enable:
    stp x29,x30,[sp,#-16]!

    mov x19,#0x80000
    str x19,[xzr,#0xd8]
    str x19,[xzr,#0xe0]
    str x19,[xzr,#0xe8]

    dsb sy @ memory barrier. 
    
    sev @ send event to other cores.

    ldp x29,x30,[sp],#16
    ret

restore_context:
    @ in this function, we deceive processor core to we are in a exception, and we return to task with "eret".
    @ we use x0 as register pointer and x1 as temporary register.
    ldr x0,[x0] @ point to it self.

    ldp x1,x30,[x0,#-16]!
    ldp x28,x29,[x0,#-16]!
    ldp x26,x27,[x0,#-16]!
    ldp x24,x25,[x0,#-16]!
    ldp x22,x23,[x0,#-16]!
    ldp x20,x21,[x0,#-16]!
    ldp x18,x19,[x0,#-16]!
    ldp x16,x17,[x0,#-16]!
    ldp x14,x15,[x0,#-16]!
    ldp x12,x13,[x0,#-16]!
    ldp x10,x11,[x0,#-16]!
    ldp x8,x9,[x0,#-16]!
    ldp x6,x7,[x0,#-16]!
    ldp x4,x5,[x0,#-16]!
    ldp x2,x3,[x0,#-16]!
    
    msr SPSR_EL1,x1 @ apply spsr.
    ldr x1,[x0,#-112]
    msr ELR_EL1,x1 @ apply ELR_EL1 (link register).
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