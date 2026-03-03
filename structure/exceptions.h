#ifndef TYPES_EXCEPTIONS_H
#define TYPES_EXCEPTIONS_H
#include "./base.h"

struct system_exceptions_statistics_t
{
    u64_t softwaresteps;
    u64_t breakpoints;
    u64_t kernel_wfi_wfe_counts;
    u64_t kernel_stack_alignment_faults;
};

#endif