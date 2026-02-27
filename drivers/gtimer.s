.section .text
.balign 4

.global set_gtimer,read_guptime,read_gticks

turn_on_gtimer:
    stp x29,x30,[sp,#-16]!
    mov x29,sp
    
    mrs x1,CNTP_CTL_EL1     ; read generic-timer control register.
    orr x1,x1,0x1           ; set first bit (enable). 
    msr CNTP_CTL_EL1,x1     ; apply chagnes.
    
    ldp x29,x30,[sp],#16
    mov sp,x29
    ret

set_gtimer:
    stp x29,x30,[sp,#-16]!
    mov x29,sp

    mrs x1,CNTFRQ_EL1       ; read generic-timer frequency.
    mov x2,#1000
    udiv x1,x1,x2           ; frequency/1000 
    mul x1,x1,x0            ; (frequency/1000)*ms = genereic-timer counts.
    msr x1,CNTP_TVAL_EL1    ; set generic-timer counts.
    
    ldp x29,x30,[sp],#16
    mov sp,x29
    ret

read_guptime:
    stp x29,x30,[sp,#-16]!

    mrs x0,CNTPCT_EL1 ; read uptime counts.
    mrs x1,CNTFRQ_EL1 ; read generic-timer frequency.
    mov x2,#1000
    mul x0,x0,x2      ; counts*1000 -> counts (seconds)
    udiv x0,x0,x1     ; counts/frequency = milli seconds.

    ldp x29,x30,[sp],#16
    ret

read_gticks:
    stp x29,x30,[sp,#-16]!
    mrs x0,CNTPCT_EL1 ;  read ticks counts.
    ldp x29,x30,[sp],#16
    ret

.ltorg
