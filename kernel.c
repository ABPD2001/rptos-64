#include "./lib/fwlist.h"
#include "./lib/types/task.h"
#include "./lib/core.h"

volatile struct pcb_t global_pcb_bank[64];

volatile struct fwlist_header_t *c0_created_queue_pcb;
volatile struct fwlist_header_t *c1_created_queue_pcb;
volatile struct fwlist_header_t *c2_created_queue_pcb;
volatile struct fwlist_header_t *c3_created_queue_pcb;

volatile struct fwlist_header_t *c0_ready_queue_pcb;
volatile struct fwlist_header_t *c1_ready_queue_pcb;
volatile struct fwlist_header_t *c2_ready_queue_pcb;
volatile struct fwlist_header_t *c3_ready_queue_pcb;

volatile struct fwlist_header_t *c0_waiting_queue_pcb;
volatile struct fwlist_header_t *c1_waiting_queue_pcb;
volatile struct fwlist_header_t *c2_waiting_queue_pcb;
volatile struct fwlist_header_t *c3_waiting_queue_pcb;

volatile struct fwlist_header_t *c0_terminated_queue_pcb;
volatile struct fwlist_header_t *c1_terminated_queue_pcb;
volatile struct fwlist_header_t *c2_terminated_queue_pcb;
volatile struct fwlist_header_t *c3_terminated_queue_pcb;

volatile struct fwlist_header_t *c0_sleeping_queue_pcb;
volatile struct fwlist_header_t *c1_sleeping_queue_pcb;
volatile struct fwlist_header_t *c2_sleeping_queue_pcb;
volatile struct fwlist_header_t *c3_sleeping_queue_pcb;

volatile struct muart_metadata_t *muart_metadata;

void task_dispatcher();
void task_schaduler();

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

void task_dispatcher()
{
}

void task_schaduler()
{
}