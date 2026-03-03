#ifndef TYPES_EXCEPTIONS_H
#define TYPES_EXCEPTIONS_H
#include "./base.h"

struct system_exceptions_statistics_t
{
    u64_t softwaresteps;                 // count of software step exceptions.
    u64_t breakpoints;                   // count of breakpoint exceptions.
    u64_t kernel_wfi_wfe_counts;         // count of wfi/wfe exceptions from EL1.
    u64_t kernel_stack_alignment_faults; // count of stack alignment fault exceptions from EL1.
};

#endif