__attribute__((section(".svc_handlers")));
#include "./svc.h"

void free_flag_preiph(u8_t flag)
{

    volatile struct pcb_t *ctask = core_tasks[core_id()];

    for (u64_t i = 0; i < 16; i++)
    {
        if ((ctask->preipherals >> i * 4) & 0xF == flag & 0xF)
        {
            ctask->preipherals &= ~(ctask->preipherals >> i * 4); // clear flag.

            u64_t mask = 0;

            for (u64_t j = 0; j < i; j++)
            {
                mask |= 1 << j;
            }

            u64_t pt1 = ctask->preipherals & mask;                   // mask first part.
            u64_t pt2 = (ctask->preipherals & ~mask) << (i - 1) * 4; // mask seconds part.

            ctask->preipherals |= pt1; // set part 1.
            ctask->preipherals |= pt2; // set part 2.
        }
    }
}

u64_t svc_muart_write(u8_t *buffer, u64_t length)
{
    const u8_t cid = core_id();
    volatile struct pcb_t *ctask = core_tasks[cid];
    volatile struct muart_metadata_t *muart = __global_muart_metadata__;

    // translate virtual address (task) to physical address (kernel).
    buffer = translate_address(buffer, ctask->ttbr, NULL);

    if (muart->owner_task != ctask->id)
        return 1; // not allocated to allowed by task.
    muart->write_buffer = buffer;
    muart->write_length = length;

    return 0;
}

u64_t svc_muart_read(u8_t *buffer, u64_t maximum_length)
{
    const u8_t cid = core_id();
    volatile struct pcb_t *ctask = core_tasks[cid];
    volatile struct muart_metadata_t *muart = __global_muart_metadata__;

    // translate virtual address (task) to physical address (kernel).
    buffer = translate_address(buffer, ctask->ttbr, NULL);

    if (muart->owner_task != ctask->id)
        return 1; // not allocated to allowed by task.

    muart->read_buffer = buffer;
    muart->read_maximum_length = maximum_length;
}

u64_t svc_muart_write_char(u8_t ch)
{
    const u8_t cid = core_id();
    volatile struct pcb_t *ctask = core_tasks[cid];
    volatile struct muart_metadata_t *muart = __global_muart_metadata__;
    volatile u8_t *mu_io = AUX_MU_IO_REG;

    if (muart->owner_task != ctask->id && !ctask->perimision_level) // if wasnt owner and was a user.
        return 1;                                                   // not allocated to allowed by task.

    while (!(svc_muart_availablity() & 0x1))
    {
    }
    *mu_io = ch; // write character to TX FIFO.

    return 0;
}

u64_t svc_muart_read_char(u8_t *ch)
{

    const u8_t cid = core_id();
    volatile struct pcb_t *ctask = core_tasks[cid];
    volatile struct muart_metadata_t *muart = __global_muart_metadata__;
    volatile u8_t *mu_io = AUX_MU_IO_REG;

    // translate virtual address (task) to physical address (kernel).
    ch = translate_address(ch, ctask->ttbr, NULL);

    if (muart->owner_task != ctask->id && !ctask->perimision_level) // if wasnt owner and was a user.
        return 1;                                                   // not allocated to allowed by task.

    while (!(svc_muart_availablity() & 0x2))
    {
    }
    *ch = *mu_io;

    return 0;
}

u8_t svc_muart_availablity()
{
    u8_t output = 0;
    volatile u32_t *aux_en = AUX_ENABLES_REG;
    volatile struct muart_metadata_t *muart_metadata = __global_muart_metadata__;

    if (!*aux_en & 1)
        return 0; // mini UART not enabled.

    volatile u32_t *aux_ier = AUX_IER_REG;
    volatile u32_t *aux_mu_lsr = AUX_MU_LSR_REG;

    if ((*aux_ier & 0x1) && (*aux_ier & 0x2))
        output |= 0x1;
    if ((*aux_mu_lsr & 1) && (*aux_mu_lsr & 128))
        output |= 0x2;
    if (muart_metadata->owner_task)
        output &= ~(0x4); // if owner task exist, clear free flag.
    else
        output |= (0x4);

    return output;
}

u64_t svc_get_task_id()
{
    const u8_t cid = core_id();
    const volatile u64_t *ctask = core_tasks[cid];

    return *ctask;
}

u64_t svc_muart_settings(u16_t baudrate, u8_t data_bits, u8_t enablation)
{
    const u8_t cid = core_id();
    volatile struct pcb_t *ctask = core_tasks[cid];
    volatile struct muart_metadata_t *muart = __global_muart_metadata__;
    volatile u32_t *baudrate_reg = AUX_MU_BAUD_REG;
    volatile u32_t *cntl = AUX_MU_CNTL_REG;
    volatile u32_t *en = AUX_ENABLES_REG;
    volatile u32_t *ier = AUX_IER_REG;

    if (muart->owner_task != ctask->id && ctask->perimision_level)
        return 1; // not allocated to allowed by task.

    if (data_bits)
        *cntl = *cntl | (0x1);
    else
        *cntl = *cntl & (0xFFFFFFFE);
    *baudrate_reg = baudrate;

    if (enablation & 0x01)
        *en = *en | 0x1;
    else
        *en = *en & 0xFFFFFFFE;
    if (enablation & 0x2)
        *cntl = *cntl | 0x2;
    else
        *cntl = *cntl | 0xFFFFFFFD;
    if (enablation & 0x4)
        *cntl = *cntl | 0x1;
    else
        *cntl = *cntl & 0xFFFFFFFE;
    if (enablation & 0x8)
        *ier = *ier | 0x1;
    else
        *ier = *ier & 0xFFFFFFFE;
    if (enablation & 0xF)
        *ier = *ier | 0x2;
    else
        *ier = *ier & 0xFFFFFFFD;

    muart->settings->baudrate = baudrate;
    muart->settings->enablation = enablation;
    muart->settings->data_bits = data_bits; // update new settings of uart.

    return 0;
}

u64_t svc_tsleep_ms(u32_t us)
{
    const u8_t cid = core_id();

    volatile struct pcb_t **current_running_task = core_tasks[cid];

    for (u64_t i = 0; i < 64; i++)
    {
        if (!global_timer_requests_bank[i].next && !global_timer_requests_bank[i].task_id && !global_timer_requests_bank[i].wake_ticks)
        {

            global_timer_requests_bank[i].wake_ticks = us + read_stimer_us();    // set wake ticks.
            global_timer_requests_bank[i].task_id = (*current_running_task)->id; // set task id.
            tfw_push_back(global_timer_requests_queue, &global_timer_requests_bank[i]);

            return 0;
        }
    }
    return 1;
}

u64_t svc_termination_request(u64_t fault_code, u64_t fault_dump)
{
    const u8_t cid = core_id();
    volatile struct pcb_t *ctask = core_tasks[cid];

    ctask->fault_code = fault_code;
    ctask->fault_dump = fault_dump;
    ctask->status = 3; // set status to terminated.

    set_gtimer(1); // allow generic timer to work for a ms on a loop.
    while (1)
    {
    } // just loop.
}

u64_t svc_gpalloc(u64_t pin)
{
    const u8_t cid = core_id();
    volatile struct pcb_t *ctask = core_tasks[cid];

    for (u64_t i = 0; i < 64; i++)
    {
        if (!global_gpio_bank[i].task_id)
        {
            global_gpio_bank[i].task_id = ctask->id;
            global_gpio_bank[i].pin_number = pin;
            ctask->preipherals |= (PREIPH_GPIO_FLAG << ctask->preipherals_count * 4);

            return 0; // done.
        }
    }
    return 1; // out of space.
}

u64_t svc_gpfree(u64_t pin)
{
    const u8_t cid = core_id();
    volatile struct pcb_t *ctask = core_tasks[cid];

    for (u64_t i = 0; i < 64; i++)
    {
        if (global_gpio_bank[i].task_id == ctask->id)
        {
            if (pin == global_gpio_bank[i].pin_number)
            {
                volatile struct pcb_t *ctask = core_tasks[core_id()];

                global_gpio_bank[i].task_id = 0;    // clear task id.
                global_gpio_bank[i].pin_number = 0; // clear pin number.

                free_flag_preiph(PREIPH_GPIO_FLAG);

                return 0; // done.
            }
            return 1; // invalid information.
        }
    }
    return 2; // no ownership with this id.
}

u64_t svc_gpfunction(u64_t pin, u8_t function)
{
    const u8_t cid = core_id();
    volatile struct pcb_t *ctask = core_tasks[cid];

    volatile u64_t *table = GPIO_FSEL0;
    u64_t nth = 0;

    table += ((u64_t)pin / 10) * 4;
    nth = pin % 10;

    for (u64_t i = 0; i < 64; i++)
    {
        if (ctask->id == global_gpio_bank[i].task_id || ctask->perimision_level)
        {
            if (pin == global_gpio_bank[i].pin_number || ctask->perimision_level)
            {
                gpfunction(table, nth, function); // set function.
                return 0;                         // done.
            }
            return 1; // invalid information.
        }
    }
    return 2; // no ownership with this id.
}

u64_t svc_gpset(u64_t pin)
{
    const u8_t cid = core_id();
    volatile struct pcb_t *ctask = core_tasks[cid];

    volatile u64_t *table = GPIO_OUT_SET0;
    u64_t nth = 0;

    table += ((u64_t)pin / 32) * 4;
    nth = pin % 32;

    for (u64_t i = 0; i < 64; i++)
    {
        if (ctask->id == global_gpio_bank[i].task_id || ctask->perimision_level)
        {
            if (pin == global_gpio_bank[i].pin_number || ctask->perimision_level)
            {
                gpset(table, nth); // set pin.
                return 0;          // done.
            }
            return 1; // invalid information.
        }
    }
    return 2; // no ownership with this id.
}

u64_t svc_gpclear(u64_t pin)
{
    const u8_t cid = core_id();
    volatile struct pcb_t *ctask = core_tasks[cid];
    volatile u64_t *table = GPIO_OUT_CLR0;
    u64_t nth = 0;

    table += ((u64_t)pin / 32) * 4;
    nth = pin % 32;

    for (u64_t i = 0; i < 64; i++)
    {
        if (ctask->id == global_gpio_bank[i].task_id || ctask->perimision_level)
        {
            if (pin == global_gpio_bank[i].pin_number || ctask->perimision_level)
            {
                gpclear(table, nth); // clear pin.
                return 0;            // done.
            }
            return 1; // invalid information.
        }
    }
    return 2; // no ownership with this id.
}
u64_t svc_gpvalue(u64_t pin, u8_t value)
{
    const u8_t cid = core_id();
    volatile struct pcb_t *ctask = core_tasks[cid];
    volatile u64_t *table = value ? GPIO_OUT_SET0 : GPIO_OUT_CLR0;
    u64_t nth = 0;

    table += ((u64_t)pin / 32) * 4;
    nth = pin % 32;

    for (u64_t i = 0; i < 64; i++)
    {
        if (ctask->id == global_gpio_bank[i].task_id || ctask->perimision_level)
        {
            if (pin == global_gpio_bank[i].pin_number || ctask->perimision_level)
            {
                if (value)
                    gpset(table, nth); // set pin.
                else
                    gpclear(table, nth); // clear pin.

                return 0; // done.
            }
            return 1; // invalid information.
        }
    }
    return 2; // no ownership with this id.
}

u64_t svc_create_ipcmailbox(u64_t accessblity, u64_t whitelist_tasks_pt1_id, u64_t whitelist_tasks_pt2_id, u64_t *blacklist_tasks_id, u8_t type, u32_t maximum_length)
{
    const u8_t cid = core_id();
    volatile struct pcb_t *ctask = core_tasks[cid];
    volatile struct ipcmailbox_t *mailbox = alloc_ipcmailbox();

    if (!mailbox)
        return 0; // failed to allocate mailbox.

    // -- wait for mutex to open befroe gain! --
    while (!gain_mutex(mailbox->access_mutex))
    {
        spinwait_mutex(mailbox->access_mutex);
    }
    mailbox->metadata |= 0x4; // set status flag to 'filling'.
    mailbox->metadata |= (type & 0x3);
    mailbox->accessibility = accessblity;
    mailbox->whitelist_tasks_pt1_id = whitelist_tasks_pt1_id;
    mailbox->whitelist_tasks_pt2_id = whitelist_tasks_pt2_id;
    mailbox->maximum_length = maximum_length;
    mailbox->task_owner = ctask->id;
    mailbox->metadata &= ~(0xC);          // clear status flag.
    mailbox->metadata |= 0x8;             // set status flag to 'fill'.
    release_mutex(mailbox->access_mutex); // release mutex.

    return mailbox; // done.
}

u64_t svc_write_ipcmailbox(u64_t mailbox_id, u64_t content_pt1, u64_t content_pt2, u64_t done, u64_t receiver_task_id)
{
    const u8_t cid = core_id();
    volatile struct pcb_t *ctask = core_tasks[cid];
    volatile struct ipcmailbox_t *mailbox = NULL;

    for (u64_t i = 0; i < 64; i++)
    {
        if (global_ipcmailbox_bank[i].id == mailbox_id)
            mailbox = global_ipcmailbox_bank + i; // set pointer.
    }

    if (ctask->perimision_level != 2)
    {
        if (receiver_task_id > 127)
            return 1; // forbidden receiver task id.
        if ((mailbox->blacklist_tasks_pt1_id || mailbox->blacklist_tasks_pt2_id) && (mailbox->whitelist_tasks_pt1_id || mailbox->whitelist_tasks_pt2_id))
            return 2; // invalid input.
        if (ctask->id < 63 && mailbox->blacklist_tasks_pt1_id && (mailbox->blacklist_tasks_pt1_id & (1 << ctask->id)))
            return 3; // task included in blacklist.
        else if (ctask->id > 63 && mailbox->blacklist_tasks_pt2_id && (mailbox->blacklist_tasks_pt2_id & (1 << ctask->id - 63)))
            return 3; // task included in blacklist.
        else if (ctask->id < 63 && mailbox->whitelist_tasks_pt1_id && !(mailbox->whitelist_tasks_pt1_id & (1 << ctask->id)))
            return 4; // task does not included in whitelist.
        else if (ctask->id > 63 && mailbox->whitelist_tasks_pt2_id && !(mailbox->whitelist_tasks_pt2_id & (1 << ctask->id - 63)))
            return 4; // task does not included in whitelist.
        else if (!(mailbox->accessibility & 0x1))
            return 5; // invalid access.
    }

    return write_ipcmailbox(mailbox, content_pt1, content_pt2, ctask->id, receiver_task_id);
}

u64_t svc_read_ipcmailbox(u64_t mailbox_id, struct ipcmailbox_message_t *message, u64_t receiver_task_id)
{
    const u8_t cid = core_id();
    volatile struct pcb_t *ctask = core_tasks[cid];
    volatile struct ipcmailbox_t *mailbox = NULL;

    for (u64_t i = 0; i < 64; i++)
    {
        if (global_ipcmailbox_bank[i].id == mailbox_id)
            mailbox = global_ipcmailbox_bank + i; // set pointer.
    }

    if (ctask->perimision_level != 2)
    {
        if (receiver_task_id > 127)
            return 1; // forbidden recevier task id.
        if ((mailbox->blacklist_tasks_pt1_id || mailbox->blacklist_tasks_pt2_id) && (mailbox->whitelist_tasks_pt1_id || mailbox->whitelist_tasks_pt2_id))
            return 2; // invalid input.
        if (ctask->id < 63 && mailbox->blacklist_tasks_pt1_id && (mailbox->blacklist_tasks_pt1_id & (1 << ctask->id)))
            return 3; // task included in blacklist.
        if (ctask->id > 63 && mailbox->blacklist_tasks_pt2_id && (mailbox->blacklist_tasks_pt2_id & (1 << ctask->id - 63)))
            return 3; // task included in blacklist.
        if (ctask->id < 63 && mailbox->whitelist_tasks_pt1_id && !(mailbox->whitelist_tasks_pt1_id & (1 << ctask->id)))
            return 4; // task does not included in whitelist.
        if (ctask->id > 63 && mailbox->whitelist_tasks_pt2_id && !(mailbox->whitelist_tasks_pt2_id & (1 << ctask->id - 63)))
            return 4; // task does not included in whitelist.
        if (!(mailbox->accessibility & 0x2))
            return 5; // invalid access.
    }

    *message = read_ipcmailbox(mailbox, receiver_task_id);
    return 0;
}

u64_t svc_edit_ipcmailbox(u64_t mailbox_id, struct ipcmailbox_settings_t *settings)
{
    const u8_t cid = core_id();
    volatile struct pcb_t *ctask = core_tasks[cid];
    volatile struct ipcmailbox_t *mailbox = NULL;

    for (u64_t i = 0; i < 64; i++)
    {
        if (global_ipcmailbox_bank[i].id == mailbox_id)
            mailbox = global_ipcmailbox_bank + i; // set pointer.
    }

    if (!(mailbox->accessibility & 0x4 || mailbox->task_owner == ctask->id) && ctask->perimision_level != 2)
        return 1;                        // forbidden.
    edit_ipcmailbox(mailbox, *settings); // edit.

    return 0; // done.
}

u64_t svc_mutex_gain(u64_t *mutex)
{
    volatile struct pcb_t *ctask = core_tasks[core_id()];
    // translate virtual address (task) to physical address (kernel).
    mutex = translate_address(mutex, ctask->ttbr, NULL);

    if (ctask->preipherals_count >= 16)
        return 2;
    const u64_t res = gain_mutex(mutex);

    if (!res)
    {
        ctask->preipherals |= (PREIPH_IPCMAILBOX_FLAG) << ctask->preipherals_count * 4; // insert flag of "software lock gain".
        ctask->preipherals_count++;                                                     // increment preipherals count.
    }

    return res;
}
u64_t svc_mutex_release(u64_t *mutex)
{
    volatile struct pcb_t *ctask = core_tasks[core_id()];
    // translate virtual address (task) to physical address (kernel).
    mutex = translate_address(mutex, ctask->ttbr, NULL);

    return release_mutex(mutex); // release mutex.
}

u64_t svc_semaphore_gain(u64_t *semaphore)
{
    volatile struct pcb_t *ctask = core_tasks[core_id()];
    // translate virtual address (task) to physical address (kernel).
    semaphore = translate_address(semaphore, ctask->ttbr, NULL);

    const u64_t res = gain_semaphore(semaphore);
    if (!res)
    {
        volatile struct pcb_t *ctask = core_tasks[core_id()];

        if (ctask->preipherals_count >= 16)
            return 2;

        ctask->preipherals |= (PREIPH_IPCMAILBOX_FLAG) << ctask->preipherals_count * 4; // insert flag of "software lock gain".
        ctask->preipherals_count++;                                                     // increment preipherals count.

        return res;
    }
}

u64_t svc_semaphore_release(u64_t *semaphore)
{
    volatile struct pcb_t *ctask = core_tasks[core_id()];
    // translate virtual address (task) to physical address (kernel).
    semaphore = translate_address(semaphore, ctask->ttbr, NULL);

    return release_semaphore(semaphore); // release semaphore.
}

s64_t svc_system_shutdown()
{
    const u8_t cid = core_id();
    volatile struct pcb_t *ctask = core_tasks[cid];
    volatile u64_t *cores_signal = __core_info_table__ + 64;

    if (ctask->perimision_level != 2)
        return 1; // as forbidden access.

    core_terminate();                            // terminate current core.
    gic400_sgi(8, GIC_SGI_MODE_BROADCASR, NULL); // send a sgi to cores with id of '8'.

    while (1)
    {
        u64_t signaled_cores_count = 0;
        for (u64_t i = 0; i < 4; i++)
        {
            if ((*cores_signal & (0xFF << i * 8)) == 2)
                signaled_cores_count++; // increment if a core was signaled correctly.
        }
        if (signaled_cores_count == 4)
            break; // check even itself signalled.
    }

    return psci_system_off(); // shutdown.
}

s64_t svc_system_reboot()
{
    const u8_t cid = core_id();
    volatile struct pcb_t *ctask = core_tasks[cid];
    volatile u64_t *cores_signal = __core_info_table__ + 64;

    if (ctask->perimision_level != 2)
        return 1; // as forbidden access.

    core_terminate();                            // terminate current core.
    gic400_sgi(8, GIC_SGI_MODE_BROADCASR, NULL); // send a sgi to cores with id of '8'.

    while (1)
    {
        u64_t signaled_cores_count = 0;
        for (u64_t i = 0; i < 4; i++)
        {
            if ((*cores_signal & (0xFF << i * 8)) == 3)
                signaled_cores_count++; // increment if a core was signaled correctly.
        }
        if (signaled_cores_count == 4)
            break; // check even itself signalled.
    }

    return psci_system_reset(); // reboot.
}

u64_t svc_wait(u64_t instruction, u8_t type)
{
    const u8_t cid = core_id();
    volatile struct pcb_t *ctask = core_tasks[cid];

    ctask->wait_reason = type;                              // set reason.
    ctask->wait_instruction = instruction;                  // set instruction.
    ctask->status = 2;                                      // set status to waiting.
    ctask->priority = built_in_max(0, ctask->priority - 1); // decrease priority (higher priority) as reward.

    // restore another task.
    task_schaduler();
    task_dispatcher();
}

u64_t svc_spawn_task(u64_t instruction_space, u8_t core_dependent, u8_t core_migration)
{
    volatile struct pcb_t *allocated_task = NULL;
    const u8_t cid = core_id();
    const u64_t tid = core_tasks[cid];

    for (u64_t i = 0; i < 128; i++)
    {
        if (!global_pcb_bank[i].valid)
        {
            allocated_task = global_pcb_bank + i; // set pointer.
            break;
        }
        if (i == 127)
            return 128; // out of bank space (maximum task id+1).
    }
    for (u64_t i = 0; i < 128; i++)
    {
        if (global_pcb_bank[i].id == tid)
            allocated_task->parent = global_pcb_bank + i; // set parent.
    }

    allocated_task->priority = 3;           // set middle at first priority.
    allocated_task->pc = instruction_space; // set pc.
    allocated_task->flags = 0b001;          // set ready flag.
    if (core_dependent)
        allocated_task->flags |= 1 << 5;
    if (core_migration)
        allocated_task->flags |= 1 << 3;

    // create a default memory & virtualization settings and space.

    volatile struct kmem_page_t *virtual_pages = alloc_kframe(2 * KB);       // allocate 8 KB at first.
    volatile u64_t *dword = virtual_pages->start_address;                    // create a pointer for virtualization space.
    volatile struct memframe_t *init_frame = alloc_page(allocated_task->id); // allocate initial first 4KB memory page.

    if (!virtual_pages)
        return 129; // failed to reserve virtualization space. (maximum task id+2)
    if (!init_frame)
        return 130; // failed to reserve memory space. (maximum task id+3)

    // define level descriptors.
    dword[0] = ((u64_t)(dword + 1) & 0xfffffffff) << 11;  // set l0 table descriptor.
    dword[1] = ((u64_t)(dword + 2) & 0xfffffffff) << 11;  // set l1 table descriptor.
    dword[2] = ((u64_t)(dword + 3) & 0xfffffffff) << 11;  // set l2 table descriptor.
    dword[3] = ((u64_t)(init_frame) & 0xfffffffff) << 11; // set l3 page descriptor.

    // set pages and virtualization for allocated task.
    allocated_task->pages.head = init_frame;
    allocated_task->pages.tail = init_frame;
    allocated_task->ttbr = dword; // set Transaltion Table Base Register for allocated task.

    allocated_task->valid = 1; // set valid flag for allocated task.

    return allocated_task->id; // done.
}