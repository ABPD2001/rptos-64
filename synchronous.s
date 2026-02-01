.section .sync_same_el_table
.org 0x00, b same_el_unkown_handler 
.org 0x04, b same_el_wfi_wfe_handler 
.org 0x08, b same_el_simd_fp_err_handler 
.org 0x0C, b same_el_illegal_state_handler
.org 0x10, b same_el_svc_hanler
.org 0x14, b same_el_hvc_hanler
.org 0x18, b same_el_smc_hanler
.org 0x20, b same_el_i_abort_lower_hanler
.org 0x24, b same_el_i_abort_same_hanler
.org 0x28, b same_el_pc_alignment_hanler
.org 0x2C, b same_el_d_abort_lower_handler
.org 0x30, b same_el_d_abort_same_handler
.org 0x34, b same_el_sp_alignment_hanler
.org 0x38, b same_el_fp_error_handler
.org 0x40, b same_el_breakpoint_handler
.org 0x44, b same_el_step_handler
.ltorg
