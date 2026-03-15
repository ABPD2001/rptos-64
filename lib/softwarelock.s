.section .text
.global gain_mutex, release_mutex, spinwait_mutex, gain_semaphore, release_semaphore, spinwait_semaphore

gain_mutex:
    stp x29,x30,[sp,#-16]! @ store frame pointer and link register into stack.
    mov x29,sp @ set frame pointer.
    
    mov x1,#0 @ set value for cas instruction.
    mov x2,#1 @ set value for cas instruction.
    cas x1,x2,[x0] @ attempt to gain mutex. (just change to armv8 instead of armv8.2)

    cmp x1,#0 @ check mutex is gain.
    cset x0,NE @ set output to zero if gained, else set to one as error.

    ldp x29,x30,[sp,#-16]! @ restore frame pointer and link register from stack.
    mov sp,x29 @ set stack pointer.
    
    ret @ return.

clear_mutex:
    mov x1,#0
    stxr w1,x1,[x0]
    ret

release_mutex:
    stp x29,x30,[sp,#-16]! @ store frame pointer and link register into stack.
    mov x29,sp @ set frame pointer.
    mov x1,#1 @ set value for cas instruction.
    mov x2,#0 @ set value for cas instruction.
    
    ldaxr x1,[x0] @ read acquire exclusive.
    cmp x1,#0 @ compare if mutex is gain.
    adr x30. @ set link register.
    add x30,x30,#8 @ skip two instructions (add & branch).
    b.eq clear_mutex @ try to release mutex.
    cset x2,EQ @ set if mutex gain.
    and x1,x1,x2 @ bitwise and (mutex gain/exclusive write stats) & mutex gain.
    
    cmp x1,#0 @ check mutex is released.
    cset x0,NE @ set output to zero if release, else set to one as error.
    
    ldp x29,x30,[sp,#-16]! @ restore frame pointer and link register from stack.
    mov sp,x29 @ set stack pointer.
    
    ret @ return.

spinwait_mutex:
    stp x29,x30,[sp,#-16]! @ store frame pointer and link register into stack.
    mov x29,sp @ set frame pointer.
    
    ldr x1,[x0] @ read mutex.
    cmp x0,#1 @ check if mutex is free.
    b.ne spinwait_mutex
    
    ldp x29,x30,[sp,#-16]! @ restore frame pointer and link register from stack.
    mov sp,x29 @ set stack pointer.
    
    ret @ return.

gain_semaphore_ret:
    mov x0,#1 @ set output to one as error.
    ret @ return.

gain_semaphore:
    stp x29,x30,[sp,#-16]! @ store frame pointer and link register into stack.
    mov x29,sp @ set frame pointer.
    
    ldaxr x1,[x0] @ read acquired exclusive semaphore.
    cbz x1,gain_semaphore_ret // return if x1 == 0, which means semphore is fully allocated.

    sub x1,x1,#1 // decrement semaphore.
    stxr w0,x1,[x0] @ store exclusive semaphore.

    ldp x29,x30,[sp,#-16]! @ restore frame pointer and link register from stack.
    mov sp,x29 @ set stack pointer.
    
    ret @ return.

release_semaphore:
    stp x29,x30,[sp,#-16]! @ store frame pointer and link register into stack.
    mov x29,sp @ set frame pointer.
    
    ldaxr x1,[x0] @ read acquired exclusive semaphore.
    add x1,x1,#1 @ increment to one, which means release one unit of semaphore.
    stxr x0,x1,[x0] @ store exclusive semaphore.

    ldp x29,x30,[sp,#-16]! @ restore frame pointer and link register from stack.
    mov sp,x29 @ set stack pointer.
    
    ret @ return.

spinwait_semaphore:
    stp x29,x30,[sp,#-16]! @ store frame pointer and link register into stack.
    mov x29,sp @ set frame pointer.
    
    ldr x1,[x0] @ read semaphore.
    cbz x1,spinwait_semaphore @ cycle back if semaphore isnt free.

    ldp x29,x30,[sp,#-16]! @ restore frame pointer and link register from stack.
    mov sp,x29 @ set stack pointer.
    
    ret @ return.
.ltorg
