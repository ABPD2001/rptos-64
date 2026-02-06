#include "./muart.h"

u64_t muart_write(u8_t *buffer, u64_t length)
{
    volatile struct muart_metadata_t *muart = (MUART_METADATA_BASE);

    muart->write_buffer = buffer;
    muart->write_length = length;

    return 0;
}

u64_t muart_read(u8_t *buffer, u64_t maximum_length)
{
    volatile struct muart_metadata_t *muart = (MUART_METADATA_BASE);

    muart->read_buffer = buffer;
    muart->read_maximum_length = maximum_length;
}

u64_t muart_write_char(u8_t ch)
{
    volatile struct muart_metadata_t *muart = (MUART_METADATA_BASE);
    volatile u8_t *mu_io = AUX_MU_IO_REG;

    while (!(muart_availablity() & 0x1))
    {
    }
    *mu_io = ch; // write character to TX FIFO.

    return 0;
}

u64_t muart_read_char(u8_t *ch)
{
    volatile struct muart_metadata_t *muart = (MUART_METADATA_BASE);
    volatile u8_t *mu_io = AUX_MU_IO_REG;

    while (!(muart_availablity() & 0x2))
    {
    }
    *ch = *mu_io;

    return 0;
}

u8_t muart_availablity()
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

u64_t muart_settings(u16_t baudrate, u8_t data_bits, u8_t enablation)
{
    volatile struct muart_metadata_t *muart = (MUART_METADATA_BASE);
    volatile u32_t *baudrate_reg = AUX_MU_BAUD_REG;
    volatile u32_t *cntl = AUX_MU_CNTL_REG;
    volatile u32_t *en = AUX_ENABLES_REG;
    volatile u32_t *ier = AUX_IER_REG;

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