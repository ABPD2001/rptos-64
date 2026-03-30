.section svc_table
.org 0x00, b svc_muart_write
.org 0x04, b svc_muart_read
.org 0x08, b svc_muart_write_char
.org 0x0C, b svc_muart_read_char
.org 0x10, b svc_muart_settings
.org 0x14, b svc_muart_availablity
.org 0x18, b svc_muart_free
.org 0x1C, b svc_muart_alloc
.org 0x20, b svc_get_task_id
.org 0x24, b svc_tsleep_ms
.org 0x28, b svc_termination_request
.org 0x2C, b svc_gpalloc
.org 0x30, b svc_gpfree
.org 0x34, b svc_gpset
.org 0x38, b svc_gpclear
.org 0x3C, b svc_gpvalue
.org 0x40, b svc_create_ipcmailbox
.org 0x44, b svc_write_ipcmailbox
.org 0x48, b svc_read_ipcmailbox
.org 0x4C, b svc_mutex_gain
.org 0x50, b svc_mutex_release
.org 0x54, b svc_semaphore_gain
.org 0x58, b svc_semaphore_release
.org 0x5C, b svc_core_id
.org 0x60, b svc_cluster_id
.org 0x64, b svc_system_shutdown
.org 0x68, b svc_system_reboot
.org 0x6C, b svc_wait
.org 0x70, b svc_gpfunction
.org 0x74, b svc_edit_ipcmailbox
.org 0x78, b svc_spawn_task // no. 30
.ltorg

.section .svc_handlers
.global svc_muart_alloc,svc_muart_free
.equiv PREIPH_MUART_FLAG,0b0000

svc_muart_alloc_free_ret:
    ldp x29,x30, [sp],#16 @ restore frame pointer and return address.
    mov sp,x29 @ set stack pointer to frame pointer.
    mov x0,#1 @ set x0 to 1, which means is allocated already by other tasks.
    ret @ return.

svc_muart_alloc_free_ctxswitch_void_loop:
    b svc_muart_alloc_free_ctxswitch_void_loop

svc_muart_alloc_free_ctxswitch:
    ldr x3,=__core_info_table__
    
    mrs x4,MPIDR_EL1 @ read core id.
    and x4,x4,#0xFF @ mask core id.

    mov x5,#0 @ clear register (just in case).
    add x5,x5,#32 @ skip stack table
    mov x6,#8
    mul x5,x4,x6 @ calculate relative address of task table.

    add x3,x3,x5 @ point into table.
    ldr x3,[x3] @ point to pcb (by pointing to it self).
    
    mov x4,#4 @ evaluate register.
    str x4,[x3,#256] @ set status of pcb to 'waiting'.

    ldr x4,[x3,#264] @ read pcb priority.
    add x4,x4,#2 @ increment by 2 (because, at start of schaduler, its gonna be decremented by one.)    
    
    cmp x4,#7 @ compare priority with maximum value.
    mov x5,#7
    csel x4,x4,x5,LE @ set x4 to (x4 if it wasnt greater than 7, else set to x5 which is 7/maximum value).

    str x4,[#264] @ store pcb priority.
    mov x4,x0 @ store x0 register value.
    mov x5,x29 @ store link register.
    mov x0,#1 @ set argument to 1ms.

    bl set_gtimer @ call the set_gtimer.
    
    mov x0,x4 @ restore x0 register value.
    mov x29,x5 @ restore link register.
    
    ret @ return.

set_mutex:
    mov x1,#1 @ set x1 to 1.
    stxr w1,x1,[x0,#72] @ exclusive write.
    ret

svc_muart_alloc_free_gainmutex:
    stp x30,x30,[sp,#-16]!
    
    ldaxr x1,[x0,#72] @ read acquired exclusive.
    cmp x1,#1 @ compare mutex value.
    adr x30. @ set link register.
    add x30,x30,#8 @ skip two instructions (add & branch).
    b.eq set_mutex @ try to gain mutex if was free.
    cset x2,EQ @ set if mutex was free.
    and x1,x1,x2 @ bitwise and (mutex free/exclusive write stat) & mutex free.

    cmp x1,#0 @ check if mutex is gain.
    
    adr x30,. @ set link register.
    add x30,x30,#8 @ skip two instructions (add & branch).
    b.ne svc_muart_alloc_ctxswitch @ if it was not 0, which means some other task is trying to allocate muart, do a context switch (voluntarily, which means, priority increment).
    cmp x1,#0 @ check if mutex is gain (just in case, because a context switch happens).
    adr x30,. @ set link register.
    add x30,x30,#8 @ skip two instructions (add & branch).
    b.ne svc_muart_alloc_free_gainmutex
    
    ldp x30,x30,[sp],#16
    ret @ return.

svc_muart_alloc:
    stp x29,x30, [sp,#-16] @ save frame pointer and return address.
    mov x29,sp @ set frame pointer.

    ldr x0,=__global_muart_metadata__
    ldr x1,[x0,#8] @ load the pointer of owner task of muart in muart_metadata.

    cmp x1,#0 @ compare x1 to 0.
    b.ne svc_muart_alloc_free_ret @ return if it was allocated already by other tasks.

    mov x1,#0 @ set to 0 for gaining mutex.
    mov x2,#1 @ set to 1 for gaining mutex.
    
    bl svc_muart_alloc_free_gainmutex @ start gaining mutex loop.

    ldr x1,=__core_info_table__

    msr x2,MPIDR_EL1 @ read core id.
    and x2,x2,#0xFF @ get core 0 id (first 8-bits).
    
    add x2,x2,#32 @ skip stack table.
    mov x3,#8
    mul x2,x2,x3 @ calculate relative address of core task table.
    
    ldr x1,[x1,x2] @ load current running task id of this core.
    ldr x1,[x1] @ point to current running task pcb of this core.
    ldr x1,[x1,#248] @ get id of current running task.

    str x1,[x0,#8] @ set owner task id of muart in muart_metadata to current running task id.
    ldr xzr,[x0,#72] @ release mutex by setting it to 0. (direct memory access, no need of CAS because we are sure that we are the one who is holding the mutex right now).

    ldp x29,x30,[sp],#16 @ restore frame pointer and return address.
    mov sp,x29 @ set stack pointer to frame pointer.
    ret @ return.

svc_muart_free_loop:
    add x1,x1,#8
    ldr xzr,[x0,x1]
    cmp x1,#64
    b.eq svc_muart_free_loop_ret

svc_muart_free_loop_ret:
    ldr xzr,[x0,#72] @ release mutex.
    ldp x29,x30,[sp],#16 @ restore frame pointer and stack pointer.
    mov sp,x29 @ set stack pointer to frame pointer.
    ret @ return.

svc_muart_free:
    stp x29,x30,[sp,#-16] @ store frame pointer and return address.
    mov x29,sp @ set stack pointer to frame pointer.

    ldr x0,=__global_muart_metadata__
    ldr x1,[x0,#8] @ read owner_task of metadata.

    cmp x1,#0 @ compare x1 (owner_task) with 0. (if it was equal, is allocated alerady).
    b.ne svc_muart_alloc_free_ret @ return if it was allocated by other task.

    mov x1,#0 @ set to 0 for gaining mutex.
    mov x2,#1 @ set to 1 for gaining mutex.
    
    bl svc_muart_alloc_free_gainmutex @ start gaining mutex loop.
    mov x0,#PREIPH_MUART_FLAG
    bl free_flag_preiph @ clear mini uart preipheral flag.

    mov x1,#0 @ clear x0, (for loop).
    bl svc_muart_free_loop @ do a loop for clearing metadata.

svc_core_id:
    stp x29,x30,[sp,#-16]!
    mov x29,sp

    mrs x0,MPIDR_EL1
    and x0,x0,#0xFF @ mask only first byte.

    mov sp,x29
    ldp x29,x30,[sp],#16
    ret @ return.

svc_cluster_id:
    stp x29,x30,[sp,#-16]!
    mov x29,sp

    mrs x0,MPIDR_EL1
    and x0,x0,#0xFF00 @ mask only second byte.
    lsr x0,#8 @ shift to right.

    mov sp,x29
    ldp x29,x30,[sp],#16
    ret @ return.

.ltorg
