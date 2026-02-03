#include "./lib/fwlist.h"
#include "./lib/types/task.h"
#include "./lib/core.h"

pcb_t global_pcb_bank[64];

fwlist_header_t *c0_created_queue_pcb;
fwlist_header_t *c1_created_queue_pcb;
fwlist_header_t *c2_created_queue_pcb;
fwlist_header_t *c3_created_queue_pcb;

fwlist_header_t *c0_ready_queue_pcb;
fwlist_header_t *c1_ready_queue_pcb;
fwlist_header_t *c2_ready_queue_pcb;
fwlist_header_t *c3_ready_queue_pcb;

fwlist_header_t *c0_waiting_queue_pcb;
fwlist_header_t *c1_waiting_queue_pcb;
fwlist_header_t *c2_waiting_queue_pcb;
fwlist_header_t *c3_waiting_queue_pcb;

fwlist_header_t *c0_terminated_queue_pcb;
fwlist_header_t *c1_terminated_queue_pcb;
fwlist_header_t *c2_terminated_queue_pcb;
fwlist_header_t *c3_terminated_queue_pcb;

void task_dispatcher();
void task_schaduler();
u8_t core_id();

void kernel()
{
    if (!core_id())
    {
        multi_core_enable();                           // wake up other cores.
        u32_t *counts = (__core_stack_table__ + 0x1C); // set pointer to counts.
        while (1)                                      // wait until all cores are ready.
            if (*counts == 4)
                break;
    }
}

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

void task_dispatcher()
{
}

void task_schaduler()
{
}