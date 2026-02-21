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
.ltorg

.section .svc_handlers
.global svc_muart_alloc,svc_muart_free

svc_muart_alloc_free_ret:
    ldp x29,x30, [sp],#16 @ restore frame pointer and return address.
    mov sp,x29 @ set stack pointer to frame pointer.
    mov x0,#1 @ set x0 to 1, which means is allocated already by other tasks.
    ret @ return.

svc_muart_alloc_free_ctxswitch:
    @ nothing , just do a context switch to other task, and when the task which is trying to allocate muart gets scheduled again, it will try to allocate muart again and check if it was allocated by other tasks or not.

svc_muart_alloc:
    stp x29,x30, [sp,#-16] @ save frame pointer and return address.
    mov x29,sp @ set frame pointer.

    ldr x0,=__global_muart_metadata__
    ldr x1,[x0,#8] @ load the pointer of owner task of muart in muart_metadata.

    cmp x1,#0 @ compare x1 to 0.
    b.ne svc_muart_alloc_free_ret @ return if it was allocated already by other tasks.

    mov x1,#0 @ set to 0 for gaining mutex.
    mov x2,#1 @ set to 1 for gaining mutex.
    cas x1,x2,[x0,#72] @ gain mutex if it was 0 and set it to 1. (Compare-And-Swap)

    cmp x1,#0 @ check if mutex is gain.
    b.ne svc_muart_alloc_ctxswitch @ if it was not 0, which means some other task is trying to allocate muart, do a context switch (voluntarily, which means, priority increment).

    ldr x1,=__core_info_table__

    msr x2,MPIDR_EL1 @ read core id.
    and x2,x2,#0xFF @ get core 0 id (first 8-bits).
    
    add x2,x2,#32 @ skip stack table.
    mul x2,x2,#8 @ calculate relative address of core task table.
    
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
    cas x1,x2,[x0,#72] @ gain mutex.

    cmp x1,#0 @ check if mutex is gain.
    b.ne svc_muart_alloc_free_ctxswitch @ @ if it was not 0, which means some other task is trying to allocate muart, do a context switch (voluntarily, which means, priority increment).

    mov x1,#0 @ clear x0, (for loop).
    bl svc_muart_free_loop @ do a loop for clearing metadata.