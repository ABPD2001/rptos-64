#ifndef SVC_H
#define SVC_H
#include "../../../structure/base.h"
#include "../../../structure/muart.h"
#include "../../../lib/core.h"
#include "../../../lib/fwlist.h"
#include "../../../structure/timer.h"
#include "../../../lib/preipherals/stimer.h"
#include "../../structure/gpio.h"
#include "../../drivers/gpio.h"

extern volatile tfwlist_header_t *timer_requestes_queues;
extern volatile timer_request_t *global_timer_requests_bank;
extern volatile u64_t **core_tasks;
extern volatile gpio_ownership_t *global_gpio_bank;

u64_t svc_mini_uart_write(u8_t *buffer, u64_t length);
u64_t svc_mini_uart_read(u8_t *buffer, u64_t maximum_length);
u64_t svc_mini_uart_write_char(u8_t ch);
u64_t svc_mini_uart_read_char(u8_t *ch);
u8_t svc_mini_uart_availablity();

u64_t svc_get_task_id();
u64_t svc_termination_request();
u64_t svc_tsleep_ms(u32_t us);

u64_t svc_gpalloc(u64_t table, u8_t nth);
u64_t svc_gpfree(u64_t task_id, u64_t table, u8_t nth);
u64_t svc_gpset(u64_t table, u8_t nth);
u64_t svc_gpclear(u64_t table, u8_t nth);
u64_t svc_gpvalue(u64_t table, u8_t nth, u8_t value);
#endif