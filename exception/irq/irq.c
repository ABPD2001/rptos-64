#include "./irq.h"

__attribute__((section(".irq_handlers")));

void uart_receiver_overrun()
{
    volatile muart_statistics_t *statistics = __global_muart_statistics__;
    statistics->receiver_overruns++; // increment receiver overrun in statistics.

    volatile u8_t *aux_mu_io = AUX_MU_IO_REG;
    *aux_mu_io; // discard a byte.
}

void uart_tx_empty()
{
    volatile struct muart_metadata_t *muart_metadata = __global_muart_metadata__;
    volatile struct muart_statistics_t *muart_statistics = __global_muart_statistics__;
    volatile u32_t *lsr_reg = AUX_MU_LSR_REG;
    volatile u8_t *aux_mu_io = AUX_MU_IO_REG;

    if (!muart_metadata->write_buffer || !muart_metadata->write_length)
    {
        muart_statistics->wt_rejections++; // increment rejection count.
        return;                            // return.
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