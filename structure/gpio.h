#ifndef TYPES_GPIO_H
#define TYPES_GPIO_H
#include "./base.h"

struct gpio_ownership_t // 16 Bytes.
{
    u64_t task_id;
    u64_t pin_number;
};

#endif