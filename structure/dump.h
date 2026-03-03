#ifndef TYPES_DUMP_H
#define TYPES_DUMP_H
#include "./base.h"

struct task_dump_t // 56 Bytes.
{
    u64_t task_id;           // id of dumped task.
    u64_t preipherals;       // allocated preipherals by task.
    u64_t preipherals_count; // count of allocated preipherals by task.
    u64_t fault_code;        // fault code.
    u64_t fault_dump;        // fault dump (description).
    u64_t flags;             // flags of task.
    u64_t stime;             // value of timer (time of dump).
};

#endif