.section .sync_same_el_table
.org 0x00, b same_el_unkown_handler // panic.
.org 0x04, b same_el_wfi_wfe_handler // not in action, but even if happen, just increments statistics.
.org 0x08, b same_el_simd_fp_err_handler // check fp/simd is powered, if wasnt, power up, else panic. 
.org 0x0C, b same_el_illegal_state_handler // panic.
.org 0x10, b lower_el_svc_handler // same as lower EL.
.org 0x14, b same_el_i_abort_lower_handler 
.org 0x18, b same_el_i_abort_same_handler 
.org 0x20, b same_el_pc_alignment_handler // panic.
.org 0x24, b same_el_d_abort_lower_handler 
.org 0x28, b same_el_d_abort_same_handler 
.org 0x2C, b same_el_sp_alignment_handler // round stack pointer to nearest aligned address.
.org 0x30, b same_el_fp_error_handler // check float pointing unit is powered, if wasnt, power up, else panic.
.org 0x34, b same_el_breakpoint_handler // fill up a log.
.org 0x38, b same_el_step_handler // ignore (statistics increment).
.ltorg

.section .sync_lower_el_table
.org 0x00, b lower_el_unkown_handler // task termination.
.org 0x04, b lower_el_wfi_wfe_handler // task termination (user not allowed to do without system call).
.org 0x08, b lower_el_simd_fp_err_handler // task termination.
.org 0x0C, b lower_el_illegal_state_handler // system panic.
.org 0x10, b lower_el_svc_handler // system call.
.org 0x14, b lower_el_i_abort_lower_handler //
.org 0x18, b lower_el_i_abort_same_handler //
.org 0x20, b lower_el_pc_alignment_handler // task termination.
.org 0x24, b lower_el_d_abort_lower_handler //
.org 0x28, b lower_el_d_abort_same_handler //
.org 0x2C, b lower_el_sp_alignment_handler // task termination.
.org 0x30, b lower_el_fp_error_handler // task termination.
.org 0x34, b lower_el_breakpoint_handler // ignore (just increment statistics).
.org 0x38, b lower_el_step_handler // ignore (just increment statistics).
.ltorg

.section sync_lower_el_handlers
.global lower_el_svc_handler

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
.ltorg
