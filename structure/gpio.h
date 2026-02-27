#ifndef TYPES_GPIO_H
#define TYPES_GPIO_H
#include "./base.h"

struct gpio_ownership_t // 24 Bytes.
{
    u64_t task_id;
    u64_t table;
    u64_t nth; // (with padding)
};

#endif