#include "./lib/fwlist.h"
#include "./lib/types/task.h"
#include "./lib/types/uart.h"
#include "./lib/core.h"
#include "./lib/math.h"

volatile pcb_t *global_pcb_bank = NULL; // limit of 64 tasks.
volatile u64_t *global_system_ticks = NULL;
volatile muart_settings_t *global_mini_uart_settings = NULL;
volatile muart_metadata_t *global_mini_uart_metadata = NULL;

volatile muart_metadata_t *muart_metadata = NULL;

volatile fwlist_header_t *created_queues = NULL;
volatile fwlist_header_t *pri0_ready_queues = NULL;
volatile fwlist_header_t *pri1_ready_queues = NULL;
volatile fwlist_header_t *pri2_ready_queues = NULL;
volatile fwlist_header_t *pri3_ready_queues = NULL;
volatile fwlist_header_t *pri4_ready_queues = NULL;
volatile fwlist_header_t *pri5_ready_queues = NULL;
volatile fwlist_header_t *pri6_ready_queues = NULL;
volatile fwlist_header_t *pri7_ready_queues = NULL;
volatile fwlist_header_t *waiting_queues = NULL;
volatile fwlist_header_t *terminated_queues = NULL;
volatile fwlist_header_t *sleeping_queues = NULL;

volatile u32_t *pri_map = NULL;
volatile u32_t *sch_ticks = NULL;

void task_dispatcher();
void task_schaduler();

void kernel()
{
    // first, we set address of global pointers (.bss variables)
    global_system_ticks = __global_timer_ticks__;
    global_pcb_bank = __pcb_bank_base__; // reminder: it has limit of 64 tasks.
    global_mini_uart_settings = __global_muart_settings__;
    global_mini_uart_metadata = __global_muart_metadata__;

    // initialize pcb queues.

    created_queues = __pcb_queue_base__;
    pri0_ready_queues = __pcb_queue_base__ + 4 * 16;
    pri1_ready_queues = __pcb_queue_base__ + 8 * 16;
    pri2_ready_queues = __pcb_queue_base__ + 12 * 16;
    pri3_ready_queues = __pcb_queue_base__ + 16 * 16;
    pri4_ready_queues = __pcb_queue_base__ + 20 * 16;
    pri5_ready_queues = __pcb_queue_base__ + 24 * 16;
    pri6_ready_queues = __pcb_queue_base__ + 28 * 16;
    pri7_ready_queues = __pcb_queue_base__ + 32 * 16;
    waiting_queues = __pcb_queue_base__ + 36 * 16;
    terminated_queues = __pcb_queue_base__ + 40 * 16;
    sleeping_queues = __pcb_queue_base__ + 44 * 16;

    pri_map = __pcb_queue_base__ + 704;
    sch_ticks = __pcb_queue_base__ + 708;

    // then, if core id was zero, enabling multi-core mode and waiting until all cores acknowledged core zero.
    if (!core_id())
    {
        // initialize mini-uart here...
        multi_core_enable();                        // wake up other cores.
        u32_t *counts = (__core_info_table__ + 64); // set pointer to counts.
        while (1)                                   // wait until all cores are ready.
            if (*counts == 4)
                break;
    }
}

void task_schaduler(pcb_t *current_running_task)
{
    const u8_t cid = core_id(); // read the core id (for calculation of queues).

    // calculating queues for each state.
    fwlist_header_t *created_queue = &created_queues[cid];

    // multi-level queue
    fwlist_header_t *pri0_ready_queue = &pri0_ready_queue[cid];
    fwlist_header_t *pri1_ready_queue = &pri1_ready_queues[cid];
    fwlist_header_t *pri2_ready_queue = &pri2_ready_queue[cid];
    fwlist_header_t *pri3_ready_queue = &pri3_ready_queues[cid];
    fwlist_header_t *pri4_ready_queue = &pri4_ready_queues[cid];
    fwlist_header_t *pri5_ready_queue = &pri5_ready_queues[cid];
    fwlist_header_t *pri6_ready_queue = &pri6_ready_queues[cid];
    fwlist_header_t *pri7_ready_queue = &pri7_ready_queues[cid];

    fwlist_header_t *ready_multi_queues[8] = {pri0_ready_queue, pri1_ready_queue, pri2_ready_queue, pri3_ready_queue, pri4_ready_queue, pri5_ready_queue, pri6_ready_queue, pri7_ready_queue};

    fwlist_header_t *waiting_queue = &waiting_queues[cid];
    fwlist_header_t *terminated_queue = &terminated_queues[cid];
    fwlist_header_t *sleep_queue = &sleeping_queues[cid];

    // processing current task.
    current_running_task->status = 1;                                                     // change status to ready.
    current_running_task->priority = built_in_max(0, current_running_task->priority - 1); // punish task, because didnt done its job in range of quantum time.

    u8_t local_sch_ticks = (*sch_ticks & (~0xFF << cid) >> cid * 8);
    local_sch_ticks++;

    for (u64_t i = 0; i < 8; i++)
    {
        if (ready_multi_queues[i]->head != NULL)
        {
            u32_t cpy_pri_map = *pri_map;
            cpy_pri_map &= ~(0xFF << cid); // first, clear core priority map.
            cpy_pri_map |= i << cid;       // then, set priority map.
            *pri_map = cpy_pri_map;        // at last, store it on global pointer.

            break;
        }
    }
    if (local_sch_ticks >= 6)
    {
        if (!*pri_map)
            local_sch_ticks = 0;
        else
            local_sch_ticks--;
        *pri_map = built_in_max(0, *pri_map - 1);
    }

    pcb_t *temp_task = created_queue->head;
    for (u64_t i = 0; i < 64; i++)
    {
        pcb_t *next_ptr = temp_task->next; // store next pcb address.

        if (temp_task->next == NULL)
            break;
        if (temp_task->status == 1)
        {
            fw_rm(created_queue, i);                                          // remove from created queue.
            fw_push_back(ready_multi_queues[temp_task->priority], temp_task); // push into multi-level based ready queue.
        }
        else if (!temp_task->status && temp_task->flags & 0x4)
        {
            temp_task->status = 1; // set status to ready.
            fw_rm(created_queue, i);
            fw_push_back(ready_multi_queues[temp_task->priority], temp_task);
        }

        temp_task = next_ptr; // seek to next pcb.
    }

    temp_task = ready_multi_queues[*pri_map]->head;
    for (u64_t i = 0; i < 64; i++)
    {
        u64_t next_ptr = temp_task->next;
        if (temp_task->next == NULL)
            break;
        if (temp_task->status == 2)
            fw_push_back(ready_multi_queues[temp_task->priority], temp_task);
        if (temp_task->status == 5)
        {
            fw_rm(ready_multi_queues[temp_task->priority], i); //  remove from ready queue.
            fw_push_back(sleep_queue, temp_task);              // push back into sleep queue.
        }
        temp_task = next_ptr; // seek to next pcb in queue.
    }

    temp_task = waiting_queue->head;
    for (u64_t i = 0; i < 64; i++)
    {
        u64_t next_ptr = temp_task->next;
        if (temp_task->next == NULL)
            break;
        if (temp_task->status == 3)
        {
            fw_rm(waiting_queue, i);                   // remove from waiting queue.
            fw_push_back(terminated_queue, temp_task); // push back into terminated queue.
        }
        temp_task = next_ptr; // seek to next pcb in queue.
    }
}

void task_dispatcher()
{
    const u8_t cid = core_id(); // get core id.
}

void wakeup_service()
{
}