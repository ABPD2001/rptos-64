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

void terminate_context(struct pcb_t *task)
{
    u64_t task_id = task->id;
    u64_t preipherals = task->preipherals;
    u64_t fault_code = task->fault_code;
    u64_t fault_dump = task->fault_dump;

    for (u64_t i = 0; i < 43; i++)
    { // clearing struct of pcb.
        volatile u64_t *property = (u64_t *)task + i;
        *property = 0; // clear property.
    }
    task->id = task_id; // id is always same.

    // free preipherals here.
    // save dump of task somewhere.
}