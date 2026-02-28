#ifndef TYPES_SCHADULER_H
#define TYPES_SCHADULER_H
#include "./base.h"

typedef struct schaduler_statistics_t
{
    u64_t terminate_counts;
    u64_t void_terminate_counts;
    u64_t tasks_count;
};
#endif