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

void __free_gpio(u64_t task_id)
{
    for (u64_t i = 0; i < 64; i++)
    {
        if (global_gpio_bank->task_id == task_id)
        {
            global_gpio_bank->task_id = 0;
            global_gpio_bank->nth = 0;
            global_gpio_bank->table = 0;
            break;
        } // clear ownership
        global_gpio_bank++; // go to next gpio ownership.
    }
}

void __free_lock(u64_t task_id)
{
    for (u64_t i = 0; i < 128; i++)
    {
        if (global_software_locks_bank->owner_task == task_id)
        {
            // clear whole lock structure.
            global_software_locks_bank->gained_task = 0;
            global_software_locks_bank->owner_task = 0;
            global_software_locks_bank->lock = NULL;
            global_software_locks_bank->type = 0;
            break;
        }
        else if (global_software_locks_bank->gained_task == task_id)
        {
            // clear gained access of this lock structure by this task.
            global_software_locks_bank->gained_task = 0;
            if (!global_software_locks_bank->type)
                release_mutex(global_software_locks_bank->lock);
            else if (global_software_locks_bank->type)
                release_semaphore(global_software_locks_bank->lock);
            break;
        }
        global_software_locks_bank++; // go next software lock structure.
    }
}

void __free_ipcmailbox(u64_t task_id)
{
    volatile struct ipcmailbox_t *nav_header = global_ipcmailbox_bank;

    for (u64_t i = 0; i < 64; i++)
    {
        if (nav_header->task_owner == task_id)
        {
            nav_header->access_mutex = 1;          // release mutex.
            nav_header->task_owner = 0;            // clear ownership.
            nav_header->blacklist_tasks_id = NULL; // clear blacklist_tasks_id.
            nav_header->whitelist_tasks_id = NULL; // clear whitelist_tasks_id.
            nav_header->accessibility = 0;         // clear accessibility.
            nav_header->maximum_length = 0;        // clear maximum length.
            nav_header->metadata = 0;              // clear metadata.
            break;
        }

        nav_header++; // go to next ipc mailbox header.
    }
}

void __free_timer_request(u64_t task_id)
{
    volatile struct tfwlist_header_t *nav_header = timer_requestes_queues[core_id()];
    volatile struct timer_request_t *nav_req = nav_header->head;

    for (u64_t i = 0; i < 64; i++)
    {
        if (nav_req->task_id == task_id)
        {
            nav_req->task_id = 0;    // clear ownership.
            nav_req->wake_ticks = 0; // clear wake_ticks.
            fw_rm(nav_header, i);    // remove item from queue.
            break;
        }
        if (!nav_req->task_id)
            break;
        nav_req = nav_req->next; // go to next request.
    }
}

void __free_void(u64_t task_id)
{
    volatile struct schaduler_statistics_t *shcaduler_statistics = __generic_base_schaduler_statistics__;
    shcaduler_statistics->void_terminate_counts++; // incremenet void terminate counts.
}

void __free_muart(u64_t task_id)
{
    volatile struct muart_metadata_t *muart_metadata = __global_muart_metadata__;
    release_mutex(muart_metadata->access_mutex); // we are sure that we have mutex already by this task.
    muart_metadata->owner_task = 0;
    muart_metadata->read_buffer = NULL;
    muart_metadata->read_length = NULL;
    muart_metadata->timeout = 0;
    muart_metadata->write_buffer = NULL;
    muart_metadata->write_length = 0;
    muart_metadata->written_length = 0;
    muart_metadata->delimiter = NULL;
}

void terminate_context(volatile struct pcb_t *task)
{
    const u64_t task_id = task->id;
    const u64_t preipherals = task->preipherals;
    const u64_t fault_code = task->fault_code;
    const u64_t fault_dump = task->fault_dump;
    const u64_t preipherals_count = task->preipherals_count;
    const u64_t flags = task->flags;

    for (u64_t i = 0; i < 43; i++)
    { // clearing struct of pcb.
        volatile u64_t *property = (u64_t *)task + i;
        *property = 0; // clear property.
    }
    task->id = task_id; // id is always same.

    __built_in_free_preipheral_t *free_preipheral_functions[10] = {&__free_muart, &__free_void, &__free_void, &__free_void, &__free_void, &__free_void, &__free_timer_request, &__free_gpio, &__free_lock, &__free_ipcmailbox}; // this must be filled.

    // free preipherals here.
    for (u64_t i = 0; i < task->preipherals_count; i++)
    {
        u8_t preipheral_id = (task->preipherals >> task->preipherals_count * 4) & (0xF); // calculate preipheral id.
        free_preipheral_functions[preipheral_id](task_id);                               // call release routine.
    }

    for (u64_t i = 0; i < 32; i++)
    {
        if (!global_tasks_dump_bank[i]->task_id)
        {
            // save dump to structure where is empty.
            global_tasks_dump_bank[i]->task_id = task_id;
            global_tasks_dump_bank[i]->preipherals = preipherals;
            global_tasks_dump_bank[i]->preipherals_count = preipherals_count;
            global_tasks_dump_bank[i]->fault_code = fault_code;
            global_tasks_dump_bank[i]->fault_dump = fault_dump;
            global_tasks_dump_bank[i]->flags = flags;
            global_tasks_dump_bank[i]->stime = read_stimer_us();
            break;
        }
        if (i == 31)
        {
            for (u64_t i = 0; i < 32; i++)
            {
                u64_t least_time = global_tasks_dump_bank[i]->stime;
                s64_t idx = -1;

                if (least_time > global_tasks_dump_bank[i]->stime)
                {
                    least_time = global_tasks_dump_bank[i]->stime; // set least time.
                    idx = i;                                       // set idx.
                }
                if (i == 31)
                {
                    // save dump to structure where it is oldest.
                    global_tasks_dump_bank[idx]->task_id = task_id;
                    global_tasks_dump_bank[idx]->preipherals = preipherals;
                    global_tasks_dump_bank[idx]->preipherals_count = preipherals_count;
                    global_tasks_dump_bank[idx]->fault_code = fault_code;
                    global_tasks_dump_bank[idx]->fault_dump = fault_dump;
                    global_tasks_dump_bank[idx]->flags = flags;
                    global_tasks_dump_bank[idx]->stime = read_stimer_us();
                    break;
                }
            }
        }
    }
}