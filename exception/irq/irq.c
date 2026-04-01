#include "./irq.h"

__attribute__((section(".irq_handlers")));

void muart_receiver_overrun()
{
    global_mini_uart_statistics->receiver_overruns++; // increment receiver overrun in statistics.

    volatile u8_t *aux_mu_io = AUX_MU_IO_REG;
    *aux_mu_io; // discard a byte.
}

void muart_tx_empty()
{
    volatile struct muart_metadata_t *muart_metadata = __global_muart_metadata__;
    volatile u32_t *lsr_reg = AUX_MU_LSR_REG;
    volatile u8_t *aux_mu_io = AUX_MU_IO_REG;

    if (!muart_metadata->write_buffer || !muart_metadata->write_length)
    {
        global_mini_uart_statistics->wt_rejections++; // increment rejection count.
        return;                                       // return.
    }

    while (muart_metadata->written_length < muart_metadata->write_length && *lsr_reg & 0x10)
    {
        muart_metadata->written_length++;                                          // increment written length.
        *aux_mu_io = muart_metadata->write_buffer[muart_metadata->written_length]; // write a byte.
    }
    if (muart_metadata->written_length == muart_metadata->write_length)
    {
        muart_metadata->write_buffer = NULL;   // clear write buffer in metadata.
        muart_metadata->write_length = NULL;   // clear write length in metadata.
        muart_metadata->written_length = NULL; // clear written length in metadata.
    }
}

void muart_valid_byte()
{
    volatile struct muart_metadata_t *muart_metadata = __global_muart_metadata__;
    volatile u32_t *muart_mu_stat = AUX_BASE + AUX_MU_STAT_REG;
    volatile u8_t *muart_mu_io = AUX_BASE + AUX_MU_IO_REG;

    if (!(muart_metadata->read_buffer && muart_metadata->read_maximum_length))
    {
        global_mini_uart_statistics->rd_rejections++; // increment rd_rejection counts.
        *muart_mu_io;                                 // discard a byte.
    }
    while (muart_metadata->read_length <= muart_metadata->read_maximum_length && (*muart_mu_stat & (0b111 << 16)))
    {
        muart_metadata->read_buffer[muart_metadata->read_length] = *muart_mu_io; // read a byte.
        if (muart_metadata->read_buffer[muart_metadata->read_length] == muart_metadata->delimiter & 0xFF)
        {                                  // end to routine if it was delimiter.
            muart_metadata->read_length++; // increment read length.
            muart_metadata->read_maximum_length = muart_metadata->read_length;
            break;
        }
        muart_metadata->read_length++; // increment read length.
    }
}

void task_migrated()
{
    const u8_t cid = core_id();

    // gain queues lock.
    while (!gain_mutex(queues_lock))
    {
        spinwait_mutex(queues_lock);
    }

    for (u64_t i = 0; i < 128; i++)
    {
        if (global_pcb_bank[i].flags & (0b11 << 5) == cid)
        {
            global_pcb_bank[i].flags &= ~(0b11 << 5);                      // clear previous core.
            u64_t prvasid = (global_pcb_bank[i].flags & 0xffff << 7) >> 7; // previous asid.

            // TLB invalidation
            asm volatile("tlbi aside1,%0\t\nisb ish" // invalidate TLB of task for user and kernel layer (inner-shareable) and wait to synchronizes.
                         : "=r"(prvasid)
                         :
                         :);
        }
    }

    release_mutex(queues_lock); // release queues lock.
}