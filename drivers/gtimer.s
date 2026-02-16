.section .text
.balign 4

.global set_gtimer,read_guptime,read_gticks

set_gtimer:
    stp x29,x30,[sp,#-16]!

    mrs x1,CNTFRQ_EL1 @ read generic-timer frequency.
    udiv x1,x1,#1000 @ frequency/1000 
    mul x1,x1,x0 @ (frequency/1000)*ms = genereic-timer counts.
    mrs x1,CNTP_TVAL_EL1 @ set generic-timer counts.
    
    ldp x29,x30,[sp],#16
    ret

read_guptime:
    stp x29,x30,[sp,#-16]!

    mrs x0,CNTPCT_EL1 @ read uptime counts.
    mrs x1,CNTFRQ_EL1 @ read generic-timer frequency.
    mul x0,x0,#1000 @ counts*1000 -> counts (seconds)
    udiv x0,x0,x1 @ counts/frequency = milli seconds.

    ldp x29,x30,[sp],#16
    ret

read_gticks:
    stp x29,x30,[sp,#-16]!
    mrs x0,CNTPCT_EL1 @ read ticks counts.
    ldp x29,x30,[sp],#16
    ret

.ltorg