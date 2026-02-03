#include "./core.h"

u8_t core_id()
{
    u64_t id;
    asm volatile(
        "mrs %0,mpidr_el1"
        : "=r"(id)
        :
        :);

    return id & (0xFF);
};