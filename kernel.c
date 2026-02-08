#include "./lib/fwlist.h"
#include "./lib/types/task.h"
#include "./lib/types/uart.h"
#include "./lib/core.h"

volatile struct pcb_t *global_pcb_bank = NULL; // limit of 64 tasks.
volatile u64_t *global_system_ticks = NULL;
volatile muart_settings_t *global_mini_uart_settings = NULL;
volatile muart_metadata_t *global_mini_uart_metadata = NULL;

volatile struct muart_metadata_t *muart_metadata;

volatile struct fwlist_header_t created_queues[4];
volatile struct fwlist_header_t ready_queues[4];
volatile struct fwlist_header_t waiting_queues[4];
volatile struct fwlist_header_t terminated_queues[4];
volatile struct fwlist_header_t sleeping_queues[4];

void task_dispatcher();
void task_schaduler();

void kernel()
{
    // first, we set address of global pointers (.bss variables)
    global_system_ticks = __global_timer_ticks__;
    global_pcb_bank = __pcb_bank_base__; // reminder: it has limit of 64 tasks.
    global_mini_uart_settings = __global_muart_settings__;
    global_mini_uart_metadata = __global_muart_metadata__;

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
    fwlist_header_t *created_queue = (__pcb_queue_base__ + 16 * cid);
    fwlist_header_t *ready_queue = (__pcb_queue_base__ + 4 * 16 * cid);
    fwlist_header_t *waiting_queue = (__pcb_queue_base__ + 8 * 16 * cid);
    fwlist_header_t *terminated_queue = (__pcb_queue_base__ + 12 * 16 * cid);
    fwlist_header_t *sleep_queue = (__pcb_queue_base__ + 16 * 16 * cid);

    // processing current task.
    current_running_task->status = 1; // change status to ready.
    current_running_task->priroty--;  // punish task, because didnt done its job in range of quantum time.

    pcb_t *temp_task = created_queue->head;
    for (u64_t i = 0; i < 64; i++)
    {
        u64_t next_ptr = temp_task->next;
        if (temp_task->next == NULL)
            break;
        if (temp_task->status == 1)
        {
            fw_rm(created_queue, i);              // remove from created queue.
            fw_push_back(ready_queue, temp_task); // push back into ready queue.
        }
        else if (!temp_task->status && temp_task->flags & 0x4)
        {
            temp_task->status = 1; // set status to ready.

            fw_rm(created_queue, i);              // remove from created queue.
            fw_push_back(ready_queue, temp_task); // push back into ready queue.
        }
        temp_task = next_ptr; // seek to next pcb in queue.
    }

    temp_task = ready_queue->head;
    for (u64_t i = 0; i < 64; i++)
    {
        u64_t next_ptr = temp_task->next;
        if (temp_task->next == NULL)
            break;
        if (temp_task->status == 2)
            fw_push_back(ready_queue, temp_task);
        if (temp_task->status == 5)
        {
            fw_rm(ready_queue, i);                //  remove from ready queue.
            fw_push_back(sleep_queue, temp_task); // push back into sleep queue.
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
            temp_task->priority = (temp_task->stack_end - temp_task->stack_start) * (100 * 1024 * 1024 / 255); // maximum stack size is 100MB!
            fw_rm(waiting_queue, i);                                                                           // remove from waiting queue.
            fw_push_back(terminated_queue, temp_task);                                                         // push back into terminated queue.
        }
        temp_task = next_ptr; // seek to next pcb in queue.
    }
}

void task_dispatcher()
{
}

void wakeup_service()
{
}