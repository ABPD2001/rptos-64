#ifndef SYNCHRONOUS_H
#define SYNCHRONOUS_H
#include "../../structure/base.h"
#include "../../structure/task.h"
#include "../../lib/core.h"

extern void task_dispatcher();
extern void task_schaduler();

// Lower EL (Aarch64).

void lower_el_wfi_wfe_handler();
void lower_el_unkown_handler();
void lower_el_sp_alignment_handler();
void lower_el_pc_alignment_handler();
void lower_el_simd_fp_err_handler();
void lower_el_fp_err_handler();
void lower_el_i_abort_lower_handler();
void lower_el_d_abort_lower_handler();

// Same EL (Aarch64).

void same_el_step_handler();
void same_el_breakpoint_handler();
void same_el_fp_error_handler();
void same_el_sp_alignment_handler();
void same_el_simd_fp_err_handler();
void same_el_wfi_wfe_handler();

#endif