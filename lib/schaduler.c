#include "./schaduler.h"

u64_t __built_in_task_waiting(volatile struct pcb_t *task)
{
    switch (task->wait_reason)
    {
    case 0:
        if (global_mini_uart_metadata->owner_task != task->id)
        {
            task->status = 3;      // set status to terminated.
            task->fault_code = 10; // forbidden access to mini-uart via waiting.
            return false;
        }

        return global_mini_uart_metadata->write_length == global_mini_uart_metadata->written_length; // if it was done.
    case 1:
        if (global_mini_uart_metadata->owner_task != task->id)
        {
            task->status = 3;      // set status to terminated.
            task->fault_code = 10; // forbidden access to mini-uart via waiting.
            return false;
        }

        return global_mini_uart_metadata->read_maximum_length == global_mini_uart_metadata->read_length; // if it was done.
    case 2:
        return global_mini_uart_metadata->access_mutex && !global_mini_uart_metadata->owner_task; // if was free and wasnt allocated.
    case 3:
        return global_mini_uart_metadata->access_mutex && global_mini_uart_metadata->owner_task; // if was free and allocated.
    case 4:
        return false; // skip, becuase timer request checking happens in other block.
    case 5:
        volatile u64_t *truevalue = task->wait_instruction;
        return *truevalue > 0; // if it was true.
    case 6:
        volatile u64_t *falsevalue = task->wait_instruction;
        return *falsevalue == 0; // if it was false.
    case 7:
        u16_t mailbox_id = task->wait_instruction & 0xFFFF;
        u32_t receiver_task_id = (task->wait_instruction & ~(0xFFFF) >> 16);

        for (u64_t i = 0; i < 1024; i++)
        {
            if (global_ipcmailbox_bank[i].id == mailbox_id)
            {
                if (ipc_not_allowed_in_lists(global_ipcmailbox_bank + i, task->id))
                {
                    task->status = 3;      // set status to terminated.
                    task->fault_code = 12; // set fault code to "invalid access to ipc mailbox (via waiting)."
                    return false;
                }
                return is_ipc_empty(global_ipcmailbox_bank + i, receiver_task_id);
            }
        }

        task->status = 3;      // set task status to terminated.
        task->fault_code = 11; // set fault code to "invalid ipc mailbox id (via waiting)".

        break;
    case 8:
        if (!task->event_number)
            return false; // if wasnt any event, skip.

        if (task->events_handler)
        {
            task->pc = task->events_handler; // set program counter of task to handler where task defined itself, reason of not running directly here: because we are in EL1 mode and letting user to run anything here its a security hole.
            return true;
        }
        return false;
    default:
        return false;
    }
}

void task_schaduler()
{
    const u8_t cid = core_id();
    u8_t cpri = *pri_map & (0xFF << cid * 8);

    volatile struct pcb_t *ctask = core_tasks[cid];
    volatile struct fwlist_header_t *ready_queues[8] = {pri0_ready_queue, pri1_ready_queue, pri2_ready_queue, pri3_ready_queue, pri4_ready_queue, pri5_ready_queue, pri6_ready_queue, pri7_ready_queue};
    volatile struct pcb_t *tmptask = NULL;

    // gain queue lock.
    while (!gain_mutex(queues_lock))
    {
        spinwait_mutex(queues_lock);
    }

    tmptask = ready_queues[tmptask->priority]->head; // set task to header of ready queue.
    for (u64_t i = 0; i < 128; i++)
    {
        if (tmptask->id == ctask->id)
        {
            const u64_t ctask_pri = ctask->priority;
            ctask->status = 1;                                      // set current running task status to ready.
            ctask->priority = built_in_max(ctask->priority + 1, 7); // increment priority (lower priority) as punishment.

            fw_rm(ready_queues[ctask_pri], i);                  // remove item.
            ctask->next = NULL;                                 // just in case...
            fw_push_back(ready_queues[ctask->priority], ctask); // push back into new prioriy ready queue.
        }
    }

    tmptask = created_queue->head; // set task to head of created queue.

    u8_t schadule_ticks = *sch_ticks & (0xFF << cid * 8); // mask schadule ticks.
    schadule_ticks++;                                     // increment schadule ticks.

    *sch_ticks &= ~(0xFF << cid * 8); // clear schaudling ticks for current core.
    *sch_ticks |= (0xFF << cid * 8);  // set schaudling ticks for current core.

    if (sch_ticks > 5)
    {                                     // for each 5 ticks
        *sch_ticks &= ~(0xFF << cid * 8); // clear ticks.
        cpri = built_in_min(cpri + 1, 8); // increment priority level with limit.
        if (cpri >= 8)
            cpri = 0; // reset priority level.

        *pri_map &= (0xFF << cid * 8); // clear priority map.
        *pri_map |= (cpri << cid * 8); // set new priority map value.
    }

    while (tmptask->next != NULL)
    {
        if (tmptask->flags & 0x4)
            tmptask->status = 1; // set status to ready.
        tmptask = tmptask->next; // go to next pcb.
    }

    tmptask = ready_queues[cpri]->head; // set task to header of current ready queue.
    for (u64_t i = 0; i < 128 && tmptask->next != NULL; i++)
    {
        u64_t next = tmptask->next; // save next pointer.

        if (!tmptask->status)
        {
            fw_rm(created_queue, i);                  // remove item from queue.
            tmptask->next = NULL;                     // just in case...
            fw_push_back(ready_queues[cid], tmptask); // push back into ready queue.
        }

        if (tmptask->status == 2) // if it was running.
        {
            fw_rm(ready_queues[cid], i); // remove item from queue.
            tmptask->next = NULL;        // clear next pointer.
        }

        tmptask = next; // go to next item.
    }

    tmptask = terminated_queue->head; // set task to header of terminated tasks.
    for (u64_t i = 0; i < 3 && tmptask->next != NULL; i++)
    {
        u64_t next = tmptask->next;

        terminate_context(tmptask); // terminate current task.
        fw_rm(terminated_queue, i); // remove from terminate queue.
        tmptask->next = NULL;       // just in case.
        tmptask->valid = 0;         // invalidate.

        tmptask = next; // go to next item.
    }

    tmptask = waiting_queue->head; // set task to header of waiting queue.

    for (u64_t i = 0; i < 3 && tmptask->next != NULL; i++)
    {
        u64_t next = tmptask->next;

        if (__built_in_task_waiting(tmptask))
        {
            fw_rm(waiting_queue, i);                                // remove from waiting queue.
            tmptask->next = NULL;                                   // just in case...
            fw_push_back(ready_queues[tmptask->priority], tmptask); // push back into ready queue.
        }

        tmptask = next; // go to next item.
    }

    release_mutex(queues_lock); // release queue lock.
}

void task_dispatcher()
{
    const u8_t cid = core_id();
    const u8_t cpri = *pri_map & (0xFF << cid * 8);

    volatile struct fwlist_header_t *ready_queues[8] = {pri0_ready_queue, pri1_ready_queue, pri2_ready_queue, pri3_ready_queue, pri4_ready_queue, pri5_ready_queue, pri6_ready_queue, pri7_ready_queue};
    volatile struct pcb_t *tmptask = ready_queues[cpri]->head;

    // gain queue lock.
    while (!gain_mutex(queues_lock))
    {
        spinwait_mutex(queues_lock);
    }

    for (u64_t i = 0; i < 128 && tmptask->next != NULL; i++)
    {
        u64_t next = tmptask->next;

        if (tmptask->flags & (1 << 4))
        {
            u64_t next = tmptask->next;
            if (tmptask->flags & 0x3 != cid && tmptask->flags & (1 << 3)) // if migration was enabled and task wasnt allocated by this core.
            {

                // sgi id 9 means migration routine (for from core).
                gic400_sgi(9, GIC_SGI_MODE_DIRECT, 1 << (tmptask->flags & 0x3)); // send a sgi only to from core of target task to migrate.

                tmptask->flags &= (0b11 << 5);             // clear previous core.
                tmptask->flags = (tmptask->flags & (0x3)); // set previous core.

                tmptask->flags &= ~(0x3);      // clear core id.
                tmptask->flags |= (cid & 0x3); // set core id.

                tmptask->status = 2;       // set status to running.
                core_tasks[cid] = tmptask; // set pointer for current running task.

                u64_t new_asid = allocate_asid(tmptask->id); // allocate new asid from asid poll.
                tmptask->ttbr &= ~(0xffff << 47);            // clear asid.
                tmptask->ttbr |= (new_asid << 47);           // set new asid.

                fw_rm(ready_queues[cpri], i); // remove from ready queue.
                tmptask->next = NULL;         // just in case...

                release_mutex(queues_lock);                                          // release queue lock.
                set_gtimer(SCHADULING_QUANTUM_TIME * ((cpri + 1) >> core_id() * 8)); // set generic-timer.
                restore_context(tmptask);                                            // restore task context.
            }
            else if (tmptask->flags & 0x3 == cid)
            {
                tmptask->status = 2;       // set status to running.
                core_tasks[cid] = tmptask; // set pointer for current running task.

                fw_rm(ready_queues[cpri], i); // remove from ready queue.
                tmptask->next = NULL;         // just in case...

                release_mutex(queues_lock);                                          // release queue lock.
                set_gtimer(SCHADULING_QUANTUM_TIME * ((cpri + 1) >> core_id() * 8)); // set generic-timer.
                restore_context(tmptask);                                            // restore task context.
            }
        }
        else
        {
            tmptask->flags &= ~(0x3);      // clear core id.
            tmptask->flags |= (cid & 0x3); // set core id.
            tmptask->flags &= ~(1 << 4);   // clear accessed before flag.

            tmptask->status = 2;       // set status to running.
            core_tasks[cid] = tmptask; // set pointer for current running task.

            fw_rm(ready_queues[cpri], i); // remove from ready queue.
            tmptask->next = NULL;         // just in case...

            release_mutex(queues_lock);
            set_gtimer(SCHADULING_QUANTUM_TIME * ((cpri + 1) >> core_id() * 8)); // set generic-timer.
            restore_context(tmptask);                                            // restore task context.
        }
        tmptask = next; // go to next pcb.
    }
    release_mutex(queues_lock); // release queue lock (just in case).
}

void wakeup_service()
{
    volatile struct fwlist_header_t *ready_queues[8] = {pri0_ready_queue, pri1_ready_queue, pri2_ready_queue, pri3_ready_queue, pri4_ready_queue, pri5_ready_queue, pri6_ready_queue, pri7_ready_queue};
    volatile struct pcb_t *temp_task = waiting_queue->head;

    // try to gain lock.
    while (!gain_mutex(queues_lock))
    {
        spinwait_mutex(queues_lock);
    }

    if (temp_task != NULL)
        for (u64_t i = 0; i < 128; i++)
        {
            if (temp_task->wait_reason != 4)
                return; // if wasnt because of waiting for timer request.

            volatile struct timer_request_t *req = tfw_find(global_timer_requests_queue, temp_task->id);
            volatile struct pcb_t *next_task = temp_task->next; // store next pcb address.

            if (req > 1) // if was correct and valid.
            {
                if (*global_system_ticks >= req->wake_ticks) // if it was reached its timeout.
                {
                    fw_rm(global_timer_requests_queue, tfw_idx(global_timer_requests_queue, temp_task->id)); // remove from timer requests queue.
                    fw_rm(waiting_queue, i);                                                                 // remove from sleep queue.
                    fw_push_back(ready_queues[temp_task->priority], temp_task);                              // insert into its priority ready queue (in other words, wake up the task).
                }
            }
            if (temp_task->next == NULL)
                break;
            temp_task = next_task; // seek to next pcb in queue.
        }
    release_mutex(queues_lock); // release queue lock.
}

void memoryvisor()
{
    // gain lock for memory frames.
    while (!gain_mutex(memory_paging_settings->access_mutex))
    {
        spinwait_mutex(memory_paging_settings->access_mutex);
    }

    u64_t free_frames = 0;

    for (u64_t i = 0; i < memory_paging_settings->pages_count; i++)
    {
        if (free_frames * 4 >= memory_paging_settings->eviction_threshold * 128 + 256)
            break; // if free frames was greater or equal to eviction threshold
        if (memory_frames[i].owner_task_id = 0)
            free_frames++; // if it was free.
    }
    if (free_frames * 4 < memory_paging_settings->eviction_threshold * 128 + 256)
    { // if free frames was lower than evication threshold (triggred).
        volatile struct pcb_t *victim_task = NULL;

        for (u64_t i = 0; i < 128; i++)
        {
            if (!victim_task)
                victim_task = global_pcb_bank + i; // set initial victim.
            else if (victim_task->priority < global_pcb_bank[i].priority && !victim_task->perimision_level && (victim_task->status == 1 | victim_task->status == 2))
                victim_task = global_pcb_bank + i; // if lower priority and user permision level and it was ready or running found, set victim pointer.
        }

        victim_task->status = 3;      // set status to terminated.
        victim_task->fault_code = 17; // set fault code to eviction.
    }

    release_mutex(memory_paging_settings->access_mutex); // release memory frames lock.
}