.section .text

.global set_stimer, read_stimer_us
.equiv SYSTEM_TIMER_BASE,#0x7E003000
.equiv SYSTEM_TIMER_CS,#0x00
.equiv SYSTEM_TIMER_CLO,#0x04
.equiv SYSTEM_TIMER_CHI,#0x08
.equiv SYSTEM_TIMER_C1,#0x10

set_stimer:
    stp x29,x30,[sp,#-16]!
    ldr x1,=SYSTEM_TIMER_BASE @ read system timer base.
    str w0,[x1,#SYSTEM_TIMER_C1] @ set c1 to first argument.
    ldp x29,x30,[sp],#16
    ret @ return

read_stimer_us:
    stp x29,x30,[sp,#-16]!
    ldr x0,=SYSTEM_TIMER_BASE @ read system timer base.
    ldr w1,[x0,#SYSTEM_TIMER_CLO]
    ldr w0,[x0,#SYSTEM_TIMER_CHI]
    
    rsl x0,#32 @ right shift with amount of 32, x0 (CHI).
    orr x0,x0,x1 @ set 0-31 bits of x0 to CLO.

    ldp x29,x30,[sp],#16
    ret @ return