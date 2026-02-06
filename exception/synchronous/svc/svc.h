#ifndef SVC_H
#define SVC_H
#include "../../../lib/types/base.h"
#include "../../../lib/types/uart.h"

u64_t svc_mini_uart_write(u8_t *buffer, u64_t length);
u64_t svc_mini_uart_read(u8_t *buffer, u64_t maximum_length);
u64_t svc_mini_uart_write_char(u8_t ch);
u64_t svc_mini_uart_read_char(u8_t *ch);
u8_t svc_mini_uart_availablity();

u64_t svc_get_task_id();
#endif