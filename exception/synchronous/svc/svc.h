#ifndef SVC_H
#define SVC_H
#include "../../../lib/types/base.h"
#include "../../../lib/types/uart.h"

#define AUX_BASE 0x7e215000
#define AUX_ENABLES_REG (AUX_BASE + 0x04)
#define AUX_IER_REG (AUX_BASE + 0x44)
#define AUX_MU_IO_REG (AUX_BASE + 0x40)
#define AUX_MU_LSR_REG (AUX_BASE + 0x54)

u64_t svc_mini_uart_write(u8_t *buffer, u64_t length);
u64_t svc_mini_uart_read(u8_t *buffer, u64_t maximum_length);
#endif