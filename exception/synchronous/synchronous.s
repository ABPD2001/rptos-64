.section .sync_same_el_table
.org 0x00, b same_el_unkown_handler 
.org 0x04, b same_el_wfi_wfe_handler 
.org 0x08, b same_el_simd_fp_err_handler 
.org 0x0C, b same_el_illegal_state_handler
.org 0x10, b same_el_svc_handler
.org 0x14, b same_el_smc_handler
.org 0x18, b same_el_i_abort_lower_handler
.org 0x20, b same_el_i_abort_same_handler
.org 0x24, b same_el_pc_alignment_handler
.org 0x28, b same_el_d_abort_lower_handler
.org 0x2C, b same_el_d_abort_same_handler
.org 0x30, b same_el_sp_alignment_handler
.org 0x34, b same_el_fp_error_handler
.org 0x38, b same_el_breakpoint_handler
.org 0x3C, b same_el_step_handler
.ltorg

.section .sync_lower_el_table
.org 0x00, b lower_el_unkown_handler 
.org 0x04, b lower_el_wfi_wfe_handler 
.org 0x08, b lower_el_simd_fp_err_handler 
.org 0x0C, b lower_el_illegal_state_handler
.org 0x10, b lower_el_svc_handler
.org 0x14, b lower_el_smc_handler
.org 0x18, b lower_el_i_abort_lower_handler
.org 0x20, b lower_el_i_abort_same_handler
.org 0x24, b lower_el_pc_alignment_handler
.org 0x28, b lower_el_d_abort_lower_handler
.org 0x2C, b lower_el_d_abort_same_handler
.org 0x30, b lower_el_sp_alignment_handler
.org 0x34, b lower_el_fp_error_handler
.org 0x38, b lower_el_breakpoint_handler
.org 0x3C, b lower_el_step_handler
.ltorg

.section sync_lower_el_handlers
.global lower_el_svc_handler,lower_el_smc_handler,lower_el_pc_alignment_handler,lower_el_sp_alignment_handler
.equiv CORES_RUNNING_TASK_BASE, @ table of current running task of cores.

lower_el_svc_handler:
    stp x29,x30,[sp,#-16]!
    mov x29,sp

    add x19,sp,#288 @ set x19 to start of saved context.
    ldp x0,x1,[x19,#-16]!
    ldp x2,x3,[x19,#-16]!
    ldp x4,x5,[x19,#-16]!
    ldp x6,x7,[x19,#-16]! @ restore arguements of task.

    stp x19,x19,[sp,#-16] @ save start of saved context into stack.

    bl x1 @ call the svc handler from table.

    ldp x19,x19,[sp],#16 @ read start of saved context info stack.
    stp x6,x7,[x19,#-16]!
    stp x4,x5,[x19,#-16]!
    stp x2,x3,[x19,#-16]!
    stp x0,x1,[x19,#-16]! @ save params and result into context.

    mov sp,x29
    ldp x29,x30,[sp],#16
    ret

lower_el_pc_alignment_handler:
    stp x29,x30,[sp,#-16]!
    mov x29,sp

    mrs x0,MPIDR_El1 @ read cores info.
    and x0,x0,#0xFF @ mask core id.

    mul x0,x0,#8
    ldr x1,=svc_table
    mul x0,x0,#4
    add x0,x0,x1

    ldr x0,[x0] @ point to pcb.

    mov x1,#3
    mov x2,#2

    str x1,[x0,#274] @ set process state to terminated.
    str x2,[x0,#300] @ set process fault code to pc alignment fault.
    mov sp,x29
    ldp x29,x30,[sp],#16
    ret

lower_el_sp_alignment_handler:
    stp x29,x30,[sp,#-16]!
    mov x29,sp

    mrs x0,MPIDR_El1 @ read cores info.
    and x0,x0,#0xFF @ mask core id.

    mul x0,x0,#8
    ldr x1,=svc_table
    mul x0,x0,#4
    add x0,x0,x1

    ldr x0,[x0] @ point to pcb.

    mov x1,#3
    mov x2,#1

    str x1,[x0,#274] @ set process status to terminated.
    str x2,[x0,#300] @ set process fault code to stack alignment fault.
    mov sp,x29
    ldp x29,x30,[sp],#16
    ret

lower_el_wfi_wfe_handler:
    stp x29,x30,[sp,#-16]!
    mov x29,sp

    mov x1,#2

    str x1,[x0,#274] @ set process status to waiting.
    
    ldr x0,[sp,#288] @ read only first arguement of task.
    str x0,[sp,#308] @ store sensetive (waking causes) list to pcb.

    mov sp,x29
    ldp x29,x30,[sp],#16
    ret