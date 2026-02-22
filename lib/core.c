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

void __free_gpios(u64_t task_id)
{
    for (u64_t i = 0; i < 64; i++)
    {
        if (global_gpio_bank->task_id == task_id)
        {
            global_gpio_bank->task_id = 0;
            global_gpio_bank->nth = 0;
            global_gpio_bank->table = 0;
        } // clear ownership
        global_gpio_bank++; // go to next gpio ownership.
    }
}

void __free_locks(u64_t task_id)
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
        }
        else if (global_software_locks_bank->gained_task == task_id)
        {
            // clear gained access of this lock structure by this task.
            global_software_locks_bank->gained_task = 0;
            if (!global_software_locks_bank->type)
                release_mutex(global_software_locks_bank->lock);
            else if (global_software_locks_bank->type)
                release_semaphore(global_software_locks_bank->lock);
        }
        global_software_locks_bank++; // go next software lock structure.
    }
}

void __free_ipcmailboxes(u64_t task_id) {}   //
void __free_timer_requests(u64_t task_id) {} //

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

    __built_in_free_preipheral_t *free_preipheral_functions[4] = {&__free_muart}; // this must be filled.

    // free preipherals here.
    for (u64_t i = 0; i < task->preipherals_count; i++)
    {
        u8_t preipheral_id = (task->preipherals >> task->preipherals_count * 4) & (0xF); // calculate preipheral id.
        free_preipheral_functions[preipheral_id](task_id);                               // call release routine.
    }
    // save dump of task somewhere.
}