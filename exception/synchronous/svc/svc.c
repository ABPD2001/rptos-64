#include "./svc.h"

u64_t svc_mini_uart_write(u8_t *buffer, u64_t length)
{
    const u8_t cid = core_id();
    const u64_t *ctask = (CORES_RUNNING_TASK_BASE + cid * 8);
    volatile struct muart_metadata_t *muart = (MUART_METADATA_BASE);

    if (muart->owner_task != *ctask)
        return 1; // not allocated to allowed by task.
    muart->write_buffer = buffer;
    muart->write_length = length;

    return 0;
}

u64_t svc_mini_uart_read(u8_t *buffer, u64_t maximum_length)
{
    const u8_t cid = core_id();
    const volatile u64_t *ctask = (CORES_RUNNING_TASK_BASE + cid * 8);
    volatile struct muart_metadata_t *muart = (MUART_METADATA_BASE);

    if (muart->owner_task != *ctask)
        return 1; // not allocated to allowed by task.

    muart->read_buffer = buffer;
    muart->read_maximum_length = maximum_length;
}

u64_t svc_mini_uart_write_char(u8_t ch)
{
    const u8_t cid = core_id();
    const volatile u64_t *ctask = (CORES_RUNNING_TASK_BASE + cid * 8);
    volatile struct muart_metadata_t *muart = (MUART_METADATA_BASE);
    volatile u8_t *mu_io = AUX_MU_IO_REG;

    if (muart->owner_task != *ctask)
        return 1; // not allocated to allowed by task.

    while (!(svc_mini_uart_availablity() & 0x1))
    {
    }
    *mu_io = ch; // write character to TX FIFO.

    return 0;
}

u64_t svc_mini_uart_read_char(u8_t *ch)
{
    const u8_t cid = core_id();
    const volatile u64_t *ctask = (CORES_RUNNING_TASK_BASE + cid * 8);
    volatile struct muart_metadata_t *muart = (MUART_METADATA_BASE);
    volatile u8_t *mu_io = AUX_MU_IO_REG;

    if (muart->owner_task != *ctask)
        return 1; // not allocated to allowed by task.

    while (!(svc_mini_uart_availablity() & 0x2))
    {
    }
    *ch = *mu_io;

    return 0;
}

u8_t svc_mini_uart_availablity()
{
    u8_t output = 0;
    volatile u32_t *aux_en = AUX_ENABLES_REG;

    if (!*aux_en & 1)
        return 0; // mini UART not enabled.

    volatile u32_t *aux_ier = AUX_IER_REG;
    volatile u32_t *aux_mu_lsr = AUX_MU_LSR_REG;

    if ((*aux_ier & 0x1) && (*aux_ier & 0x2))
        output |= 0x1;
    if ((*aux_mu_lsr & 1) && (*aux_mu_lsr & 128))
        output |= 0x2;

    return output;
}

u64_t svc_get_task_id()
{
    const u8_t cid = core_id();
    const volatile u64_t *ctask = (CORES_RUNNING_TASK_BASE + cid * 8);

    return *ctask;
}