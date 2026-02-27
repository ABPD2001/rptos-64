#include "./lib/fwlist.h"
#include "./structure/task.h"
#include "./structure/muart.h"
#include "./lib/core.h"
#include "./lib/math.h"
#include "../drivers/gtimer.h"
#include "../drivers/muart.h"
#include "../structure/gpio.h"
#include "../structure/ipcmailbox.h"
#include "../structure/gic400.h"
#include "../drivers/gic400.h"
#include "../structure/irq.h"
#include "../structure/panic.h"

volatile struct pcb_t *global_pcb_bank = NULL;             // limit of 64 tasks.
volatile struct gpio_ownership_t *global_gpio_bank = NULL; // limit of 64 ownerships.
volatile struct timer_request_t *global_timer_requests_bank = NULL;
volatile struct slcb_t *global_software_locks_bank = NULL; // limit of 128 software locks.

volatile struct ipcmailbox_t *global_ipcmailbox_bank = NULL;                  // limit of 64 headers.
volatile struct ipcmailbox_segment_t *global_ipcmailbox_segments_bank = NULL; // limit of 512 segments.

volatile u64_t *global_system_ticks = NULL;
volatile struct muart_settings_t *global_mini_uart_settings = NULL;
volatile struct muart_metadata_t *global_mini_uart_metadata = NULL;
volatile struct muart_statistics_t *global_mini_uart_statistics = NULL;

volatile struct irq_statistic_t *generic_irq_statistics_base = NULL;

volatile struct gic400_metadata_t *global_gic400_metadata = NULL;

volatile struct system_panic_log_t *system_panic_log = NULL;

volatile struct fwlist_header_t *created_queues = NULL;
volatile struct fwlist_header_t *pri0_ready_queues = NULL;
volatile struct fwlist_header_t *pri1_ready_queues = NULL;
volatile struct fwlist_header_t *pri2_ready_queues = NULL;
volatile struct fwlist_header_t *pri3_ready_queues = NULL;
volatile struct fwlist_header_t *pri4_ready_queues = NULL;
volatile struct fwlist_header_t *pri5_ready_queues = NULL;
volatile struct fwlist_header_t *pri6_ready_queues = NULL;
volatile struct fwlist_header_t *pri7_ready_queues = NULL;
volatile struct fwlist_header_t *waiting_queues = NULL;
volatile struct fwlist_header_t *terminated_queues = NULL;
volatile struct fwlist_header_t *sleeping_queues = NULL;

volatile struct tfwlist_header_t *timer_requestes_queues = NULL;

volatile u64_t **core_tasks = NULL;

volatile u32_t *pri_map = NULL;
volatile u32_t *sch_ticks = NULL;

void task_dispatcher();
void task_schaduler(struct pcb_t *current_running_task);
void wakeup_service();

void kernel()
{
    // first, we set address of global pointers (.bss variables)
    global_system_ticks = __global_timer_ticks__;
    global_pcb_bank = __pcb_bank_base__;                      // reminder: it has limit of 64 tasks.
    global_timer_requests_bank = __timer_request_bank_base__; // reminder: it has limit of 64 requests.
    global_mini_uart_settings = __global_muart_settings__;
    global_mini_uart_metadata = __global_muart_metadata__;
    global_gpio_bank = __gpio_ownerships_bank_base__;                 // reminder: it has limit of 64 ownerships.
    global_software_locks_bank = __global_software_locks_bank_base__; // reminder: it has limit of 128 software locks.
    global_ipcmailbox_bank = __global_ipcmailbox_headers_bank_base__;
    global_ipcmailbox_segments_bank = __global_ipcmailboxes_segments_bank_base__;
    global_gic400_metadata = __global_gic400_metadata__;
    global_mini_uart_statistics = __global_muart_statistics__;
    system_panic_log = __system_panic_log__;
    core_tasks = __core_info_table__ + 16;

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

    timer_requestes_queues = __pcb_queue_base__ + 48 * 16;

    pri_map = __pcb_queue_base__ + 704;
    *pri_map = 0x07070707; // set highest priority at first. (7 is most and 0 is least).
    sch_ticks = __pcb_queue_base__ + 708;
    *sch_ticks = 0; // just in case... (to prevent from unkown behavior).

    turn_on_gtimer(); // turn on generic timer of current core.

    // then, if core id was zero, enabling multi-core mode and waiting until all cores acknowledged core zero.
    if (!core_id())
    {
        initialize_muart();                         // initialize mini-uart.
        gic400_setGrp1_distributor();               // enable group 1 interrupt.
        multi_core_enable();                        // wake up other cores.
        u32_t *counts = (__core_info_table__ + 64); // set pointer to counts.
        while (1)                                   // wait until all cores are ready.
            if (*counts == 4)
                break;

        char buffer[16] = "rptos-64 is up.";
        buffer[15] = '\0';
        muart_write(buffer, 16);
    }

    // at last, configure gic-400.

    gic400_interfacectl(true, true); // enable EOIModeNS and Group 1.
    gic400_priorityirq(125, 0x80);   // AUX
    gic400_priorityirq(89, 0x90);    // UART
    gic400_priorityirq(30, 0xA0);    // generic timer
    gic400_priorityirq(97, 0xB0);    // system timer
    enable_daif();                   // enable IRQ, FIQ, SError, Debug
}

void task_schaduler(struct pcb_t *current_running_task)
{
    const u8_t cid = core_id(); // read the core id (for calculation of queues).

    // calculating queues for each state.
    volatile struct fwlist_header_t *created_queue = &created_queues[cid];

    // multi-level queue
    volatile struct fwlist_header_t *pri0_ready_queue = &pri0_ready_queue[cid];
    volatile struct fwlist_header_t *pri1_ready_queue = &pri1_ready_queues[cid];
    volatile struct fwlist_header_t *pri2_ready_queue = &pri2_ready_queue[cid];
    volatile struct fwlist_header_t *pri3_ready_queue = &pri3_ready_queues[cid];
    volatile struct fwlist_header_t *pri4_ready_queue = &pri4_ready_queues[cid];
    volatile struct fwlist_header_t *pri5_ready_queue = &pri5_ready_queues[cid];
    volatile struct fwlist_header_t *pri6_ready_queue = &pri6_ready_queues[cid];
    volatile struct fwlist_header_t *pri7_ready_queue = &pri7_ready_queues[cid];

    volatile struct fwlist_header_t *ready_multi_queues[8] = {pri0_ready_queue, pri1_ready_queue, pri2_ready_queue, pri3_ready_queue, pri4_ready_queue, pri5_ready_queue, pri6_ready_queue, pri7_ready_queue};

    volatile struct fwlist_header_t *waiting_queue = &waiting_queues[cid];
    volatile struct fwlist_header_t *terminated_queue = &terminated_queues[cid];
    volatile struct fwlist_header_t *sleep_queue = &sleeping_queues[cid];

    // processing current task.
    current_running_task->status = 1;                                                     // change status to ready.
    current_running_task->priority = built_in_max(0, current_running_task->priority - 1); // punish task, because didnt done its job in range of quantum time.

    u8_t local_sch_ticks = (*sch_ticks & (~0xFF << cid * 8 - 1) >> cid * 8 - 1);
    local_sch_ticks++;

    for (u64_t i = 0; i < 8; i++)
    {
        if (ready_multi_queues[i]->head != NULL)
        {
            u32_t cpy_pri_map = *pri_map;
            cpy_pri_map &= ~(0xFF << cid * 8 - 1); // first, clear core priority map.
            cpy_pri_map |= i << cid * 8 - 1;       // then, set priority map.
            *pri_map = cpy_pri_map;                // at last, store it on global pointer.

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

    volatile struct pcb_t *temp_task = created_queue->head;
    if (temp_task != NULL)
        for (u64_t i = 0; i < 64; i++)
        {
            pcb_t *next_ptr = temp_task->next; // store next pcb address.

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

            if (temp_task->next == NULL)
                break;
            temp_task = next_ptr; // seek to next pcb.
        }

    temp_task = ready_multi_queues[*pri_map]->head;
    for (u64_t i = 0; i < 64; i++)
    {
        u64_t next_ptr = temp_task->next;
        if (temp_task->status == 2)
            fw_push_back(ready_multi_queues[temp_task->priority], temp_task);
        if (temp_task->status == 5)
        {
            fw_rm(ready_multi_queues[temp_task->priority], i); //  remove from ready queue.
            fw_push_back(sleep_queue, temp_task);              // push back into sleep queue.
        }
        if (temp_task->next == NULL)
            break;
        temp_task = next_ptr; // seek to next pcb in queue.
    }

    temp_task = waiting_queue->head;
    for (u64_t i = 0; i < 64; i++)
    {
        u64_t next_ptr = temp_task->next;
        if (temp_task->status == 3)
        {
            fw_rm(waiting_queue, i);                   // remove from waiting queue.
            fw_push_back(terminated_queue, temp_task); // push back into terminated queue.
        }
        if (temp_task->next == NULL)
            break;
        temp_task = next_ptr; // seek to next pcb in queue.
    }
}

void task_dispatcher()
{
    const u8_t cid = core_id(); // get core id.

    volatile struct fwlist_header_t *pri0_ready_queue = &pri0_ready_queue[cid];
    volatile struct fwlist_header_t *pri1_ready_queue = &pri1_ready_queues[cid];
    volatile struct fwlist_header_t *pri2_ready_queue = &pri2_ready_queue[cid];
    volatile struct fwlist_header_t *pri3_ready_queue = &pri3_ready_queues[cid];
    volatile struct fwlist_header_t *pri4_ready_queue = &pri4_ready_queues[cid];
    volatile struct fwlist_header_t *pri5_ready_queue = &pri5_ready_queues[cid];
    volatile struct fwlist_header_t *pri6_ready_queue = &pri6_ready_queues[cid];
    volatile struct fwlist_header_t *pri7_ready_queue = &pri7_ready_queues[cid];

    volatile struct fwlist_header_t *ready_multi_queues[8] = {pri0_ready_queue, pri1_ready_queue, pri2_ready_queue, pri3_ready_queue, pri4_ready_queue, pri5_ready_queue, pri6_ready_queue, pri7_ready_queue};

    u64_t local_pri_map = (*pri_map & (0xFF << cid * 8 - 1)) >> cid * 8 - 1;
    volatile struct pcb_t *target_task = ready_multi_queues[local_pri_map]->head;

    fw_rm(ready_multi_queues[local_pri_map], 0);                  // remove first task of queue (because, task after a running cycle, must sits at end of queue).
    target_task->status = 2;                                      // set task status to running.
    fw_push_back(ready_multi_queues[local_pri_map], target_task); // push back task (to sit at end of queue).

    set_timer(100 - (*pri_map * 10)); // quantum time: (100 - (queue_priority*10)) ms.
    restore_context(target_task);     // restore task.
}

void wakeup_service()
{
    const u8_t cid = core_id(); // get core id.

    volatile struct tfwlist_header_t *timer_request_queue = &timer_requestes_queues[cid];

    volatile struct fwlist_header_t *sleeping_queue = &sleeping_queues[cid];
    volatile struct fwlist_header_t *pri0_ready_queue = &pri0_ready_queue[cid];
    volatile struct fwlist_header_t *pri1_ready_queue = &pri1_ready_queues[cid];
    volatile struct fwlist_header_t *pri2_ready_queue = &pri2_ready_queue[cid];
    volatile struct fwlist_header_t *pri3_ready_queue = &pri3_ready_queues[cid];
    volatile struct fwlist_header_t *pri4_ready_queue = &pri4_ready_queues[cid];
    volatile struct fwlist_header_t *pri5_ready_queue = &pri5_ready_queues[cid];
    volatile struct fwlist_header_t *pri6_ready_queue = &pri6_ready_queues[cid];
    volatile struct fwlist_header_t *pri7_ready_queue = &pri7_ready_queues[cid];

    volatile struct fwlist_header_t *ready_multi_queues[8] = {pri0_ready_queue, pri1_ready_queue, pri2_ready_queue, pri3_ready_queue, pri4_ready_queue, pri5_ready_queue, pri6_ready_queue, pri7_ready_queue};

    volatile struct pcb_t *temp_task = sleeping_queue->head;

    if (temp_task != NULL)
        for (u64_t i = 0; i < 64; i++)
        {
            volatile struct timer_request_t *req = tfw_find(timer_request_queue, temp_task->id);
            volatile struct pcb_t *next_task = temp_task->next; // store next pcb address.

            if (req > 1) // if was correct and valid.
            {
                if (*global_system_ticks >= req->wake_ticks) // if it was reached its timeout.
                {
                    fw_rm(timer_request_queue, tfw_idx(timer_request_queue, temp_task->id)); // remove from timer requests queue.
                    fw_rm(sleeping_queue, i);                                                // remove from sleep queue.
                    fw_push_back(ready_multi_queues[temp_task->priority], temp_task);        // insert into its priority ready queue (in other words, wake up the task).
                }
            }
            if (temp_task->next == NULL)
                break;
            temp_task = next_task; // seek to next pcb in queue.
        }
}