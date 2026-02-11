#ifndef SVC_H
#define SVC_H
#include "../../../lib/types/base.h"
#include "../../../lib/types/muart.h"
#include "../../../lib/core.h"
#include "../../../lib/fwlist.h"
#include "../../../lib/types/timer.h"
#include "../../../lib/preipherals/stimer.h"

extern volatile tfwlist_header_t *timer_requestes_queues;
extern volatile timer_request_t *global_timer_requests_bank;
extern volatile u64_t **core_tasks;

u64_t svc_mini_uart_write(u8_t *buffer, u64_t length);
u64_t svc_mini_uart_read(u8_t *buffer, u64_t maximum_length);
u64_t svc_mini_uart_write_char(u8_t ch);
u64_t svc_mini_uart_read_char(u8_t *ch);
u8_t svc_mini_uart_availablity();

u64_t svc_get_task_id();
u64_t svc_tsleep_ms(u32_t us);
#endif