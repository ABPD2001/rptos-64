__attribute__((section(".svc_handlers")));
#include "./svc.h"

u64_t free_flag_preiph(u8_t flag)
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
    const u64_t *ctask = core_tasks[cid];
    volatile struct muart_metadata_t *muart = __global_muart_metadata__;

    if (muart->owner_task != *ctask)
        return 1; // not allocated to allowed by task.
    muart->write_buffer = buffer;
    muart->write_length = length;

    return 0;
}

u64_t svc_muart_read(u8_t *buffer, u64_t maximum_length)
{
    const u8_t cid = core_id();
    const volatile u64_t *ctask = core_tasks[cid];
    volatile struct muart_metadata_t *muart = __global_muart_metadata__;

    if (muart->owner_task != *ctask)
        return 1; // not allocated to allowed by task.

    muart->read_buffer = buffer;
    muart->read_maximum_length = maximum_length;
}

u64_t svc_muart_write_char(u8_t ch)
{
    const u8_t cid = core_id();
    const volatile u64_t *ctask = core_tasks[cid];
    volatile struct muart_metadata_t *muart = __global_muart_metadata__;
    volatile u8_t *mu_io = AUX_MU_IO_REG;

    if (muart->owner_task != *ctask)
        return 1; // not allocated to allowed by task.

    while (!(svc_muart_availablity() & 0x1))
    {
    }
    *mu_io = ch; // write character to TX FIFO.

    return 0;
}

u64_t svc_muart_read_char(u8_t *ch)
{
    const u8_t cid = core_id();
    const volatile u64_t *ctask = core_tasks[cid];
    volatile struct muart_metadata_t *muart = __global_muart_metadata__;
    volatile u8_t *mu_io = AUX_MU_IO_REG;

    if (muart->owner_task != *ctask)
        return 1; // not allocated to allowed by task.

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
    const volatile u64_t *ctask = core_tasks[cid];
    volatile struct muart_metadata_t *muart = __global_muart_metadata__;
    volatile u32_t *baudrate_reg = AUX_MU_BAUD_REG;
    volatile u32_t *cntl = AUX_MU_CNTL_REG;
    volatile u32_t *en = AUX_ENABLES_REG;
    volatile u32_t *ier = AUX_IER_REG;

    if (muart->owner_task != *ctask)
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

    volatile struct tfwlist_header_t *timer_requests_queue = timer_requestes_queues[cid];
    volatile struct pcb_t **current_running_task = core_tasks[cid];

    for (u64_t i = 0; i < 64; i++)
    {
        if (!global_timer_requests_bank[i].next && !global_timer_requests_bank[i].task_id && !global_timer_requests_bank[i].wake_ticks)
        {

            global_timer_requests_bank[i].wake_ticks = us + read_stimer_us();    // set wake ticks.
            global_timer_requests_bank[i].task_id = (*current_running_task)->id; // set task id.
            tfw_push_back(timer_requests_queue, &global_timer_requests_bank[i]);

            return 0;
        }
    }
    return 1;
}

u64_t svc_termination_request()
{
    const u8_t cid = core_id();
    volatile struct pcb_t *ctask = core_tasks[cid];
    ctask->status = 3; // set status to terminated.

    set_gtimer(1); // allow generic timer to work for a ms on a loop.
    while (1)
    {
    } // just loop.
}

u64_t svc_gpalloc(u64_t table, u8_t nth)
{
    const u8_t cid = core_id();
    volatile struct pcb_t *ctask = core_tasks[cid];

    free_flag_preiph(PREIPH_GPIO_FLAG);
    return 1; // out of space.
}

u64_t svc_gpfree(u64_t task_id, u64_t table, u8_t nth)
{
    for (u64_t i = 0; i < 64; i++)
    {
        if (global_gpio_bank[i].task_id == task_id)
        {
            if (table == global_gpio_bank[i].table && nth == global_gpio_bank[i].nth)
            {
                volatile struct pcb_t *ctask = core_tasks[core_id()];

                global_gpio_bank[i].task_id = 0; // clear task id.
                global_gpio_bank[i].table = 0;   // clear table.
                global_gpio_bank[i].nth = 0;     // clear nth.

                for (u64_t i = 0; i < 16; i++)
                {
                    if ((ctask->preipherals >> i * 4) & 0xF == PREIPH_GPIO_FLAG)
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

                return 0; // done.
            }
            return 1; // invalid information.
        }
    }
    return 2; // no ownership with this id.
}

u64_t svc_gpfunction(u64_t table, u8_t nth, u8_t function)
{
    const u8_t cid = core_id();
    volatile struct pcb_t *ctask = core_tasks[cid];

    for (u64_t i = 0; i < 64; i++)
    {
        if (ctask->id == global_gpio_bank[i].task_id)
        {
            if (table == global_gpio_bank[i].table && nth == global_gpio_bank[i].nth)
            {
                gpfunction(table, nth, function); // set function.
                return 0;                         // done.
            }
            return 1; // invalid information.
        }
    }
    return 2; // no ownership with this id.
}

u64_t svc_gpset(u64_t table, u8_t nth)
{
    const u8_t cid = core_id();
    volatile struct pcb_t *ctask = core_tasks[cid];

    for (u64_t i = 0; i < 64; i++)
    {
        if (ctask->id == global_gpio_bank[i].task_id)
        {
            if (table == global_gpio_bank[i].table && nth == global_gpio_bank[i].nth)
            {
                gpset(table, nth); // set pin.
                return 0;          // done.
            }
            return 1; // invalid information.
        }
    }
    return 2; // no ownership with this id.
}

u64_t svc_gpclear(u64_t table, u8_t nth)
{
    const u8_t cid = core_id();
    volatile struct pcb_t *ctask = core_tasks[cid];

    for (u64_t i = 0; i < 64; i++)
    {
        if (ctask->id == global_gpio_bank[i].task_id)
        {
            if (table == global_gpio_bank[i].table && nth == global_gpio_bank[i].nth)
            {
                gpclear(table, nth); // clear pin.
                return 0;            // done.
            }
            return 1; // invalid information.
        }
    }
    return 2; // no ownership with this id.
}
u64_t svc_gpvalue(u64_t table, u8_t nth, u8_t value)
{
    const u8_t cid = core_id();
    volatile struct pcb_t *ctask = core_tasks[cid];

    for (u64_t i = 0; i < 64; i++)
    {
        if (ctask->id == global_gpio_bank[i].task_id)
        {
            if (table == global_gpio_bank[i].table && nth == global_gpio_bank[i].nth)
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

u64_t svc_create_ipcmailbox(u64_t accessblity, u64_t *whitelist_tasks_id, u64_t *blacklist_tasks_id, u8_t type, u32_t maximum_length)
{
    const u8_t cid = core_id();
    volatile struct pcb_t *ctask = core_tasks[cid];
    volatile struct ipcmailbox_t *mailbox = alloc_ipcmailbox();

    if (!mailbox)
        return 1; // failed to allocate mailbox.

    // -- wait for mutex to open befroe gain! --
    gain_mutex(mailbox->access_mutex);
    mailbox->metadata |= 0x4; // set status flag to 'filling'.
    mailbox->metadata |= (type & 0x3);
    mailbox->accessibility = accessblity;
    mailbox->whitelist_tasks_id = whitelist_tasks_id;
    mailbox->blacklist_tasks_id = blacklist_tasks_id;
    mailbox->maximum_length = maximum_length;
    mailbox->task_owner = ctask->id;
    mailbox->metadata &= ~(0xC);          // clear status flag.
    mailbox->metadata |= 0x8;             // set status flag to 'fill'.
    release_mutex(mailbox->access_mutex); // release mutex.

    return 0; // done.
}

u64_t svc_write_ipcmailbox(volatile struct ipcmailbox_t *mailbox, u64_t content_pt1, u64_t content_pt2, u64_t done, u64_t receiver_task_id)
{
    const u8_t cid = core_id();
    volatile struct pcb_t *ctask = core_tasks[cid];

    if (mailbox->blacklist_tasks_id && mailbox->whitelist_tasks_id)
        return 2; // invalid input.
    if (mailbox->blacklist_tasks_id && (mailbox->blacklist_tasks_id & 1 << ctask->id))
        return 3; // task included in blacklist.
    else if (mailbox->whitelist_tasks_id && !(mailbox->whitelist_tasks_id & 1 << ctask->id))
        return 4; // task does not included in whitelist.
    else if (!(mailbox->accessibility & 0x1))
        return 5; // invalid access.

    return write_ipcmailbox(mailbox, content_pt1, content_pt2, ctask->id, receiver_task_id);
}

u64_t svc_read_ipcmailbox(volatile struct ipcmailbox_t *mailbox, struct ipcmailbox_message_t *message, u64_t *content_pt1, u64_t *content_pt2, u64_t receiver_task_id)
{
    const u8_t cid = core_id();
    volatile struct pcb_t *ctask = core_tasks[cid];

    if (mailbox->blacklist_tasks_id && mailbox->whitelist_tasks_id)
        return 2; // invalid input.
    if (mailbox->blacklist_tasks_id && (mailbox->blacklist_tasks_id & 1 << ctask->id))
        return 3; // task included in blacklist.
    else if (mailbox->whitelist_tasks_id && !(mailbox->whitelist_tasks_id & 1 << ctask->id))
        return 4; // task does not included in whitelist.
    else if (!(mailbox->accessibility & 0x2))
        return 5; // invalid access.

    *message = read_ipcmailbox(mailbox, receiver_task_id);
    return 0;
}

u64_t svc_mutex_gain(u64_t *mutex)
{
    const u64_t res = gain_mutex(mutex);
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
u64_t svc_mutex_release(u64_t *mutex)
{
    return release_mutex(mutex); // release mutex.
}

u64_t svc_semaphore_gain(u64_t *semaphore)
{
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
    return release_semaphore(semaphore); // release semaphore.
}