.section .text
.balign 4

multi_core_enable:
    stp x29,x30,[sp,#-16]!

    mov x19,#0x8000
    str x19,[xzr,#0xd8]
    str x19,[xzr,#0xe0]
    str x19,[xzr,#0xe8]

    dsb sy @ memory barrier. 
    
    sev @ send event to other cores.

    ldp x29,x30,[sp],#16
    ret