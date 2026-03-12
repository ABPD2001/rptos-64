#ifndef STDLIB_GPIO_H
#define STDLIB_GPIO_H
#include "../base.h"

extern u64_t gpioalloc(u64_t pin);
extern u64_t gpiofree(u64_t pin);
extern u64_t gpioon(u64_t pin);
extern u64_t gpiooff(u64_t pin);
extern u64_t gpioval(u64_t pin);
extern u64_t gpiofunction(u64_t pin, u8_t function);
#endif