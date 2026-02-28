#ifndef TYPES_DUMP_H
#define TYPES_DUMP_H
#include "./base.h"

struct task_dump_t // 56 Bytes.
{
    u64_t task_id;
    u64_t preipherals;
    u64_t preipherals_count;
    u64_t fault_code;
    u64_t fault_dump;
    u64_t flags;
    u64_t stime;
};

#endif