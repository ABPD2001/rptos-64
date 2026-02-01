.section .sync_table
sync_table:
    .balign 4
    .org 0x00, b unkown_handler 
    .org 0x04, b wfi_wfe_handler 
    .org 0x08, b simd_fp_err_handler 
    .org 0x0C, b illegal_state_handler
    .org 0x10, b svc_hanler
    .org 0x14, b hvc_hanler
    .org 0x18, b smc_hanler
    .org 0x20, b i_abort_lower_hanler
    .org 0x24, b i_abort_same_hanler
    .org 0x28, b pc_alignment_hanler
    .org 0x2C, b d_abort_lower_handler
    .org 0x30, b d_abort_same_handler
    .org 0x34, b sp_alignment_hanler
    .org 0x38, b fp_error_handler
    .org 0x40, b breakpoint_handler
    .org 0x44, b step_handler
.ltorg
