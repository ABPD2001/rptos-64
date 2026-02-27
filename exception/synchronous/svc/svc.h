#ifndef SVC_H
#define SVC_H
#include "../../../structure/base.h"
#include "../../structure/gpio.h"
#include "../../structure/ipcmailbox.h"
#include "../../../structure/muart.h"
#include "../../../structure/timer.h"

#include "../../drivers/gpio.h"

#include "../../../lib/core.h"
#include "../../lib/ipcmailbox.h"
#include "../../../lib/fwlist.h"
#include "../../../lib/preipherals/stimer.h"
#include "../../lib/softwarelock.h"

extern volatile tfwlist_header_t *timer_requestes_queues;
extern volatile timer_request_t *global_timer_requests_bank;
extern volatile u64_t **core_tasks;
extern volatile gpio_ownership_t *global_gpio_bank;

u64_t svc_muart_write(u8_t *buffer, u64_t length);
u64_t svc_muart_read(u8_t *buffer, u64_t maximum_length);
u64_t svc_muart_write_char(u8_t ch);
u64_t svc_muart_read_char(u8_t *ch);
u8_t svc_muart_availablity();
extern u64_t svc_muart_alloc();
extern u64_t svc_muart_free();

u64_t svc_get_task_id();
u64_t svc_termination_request();
u64_t svc_tsleep_ms(u32_t us);

u64_t svc_gpalloc(u64_t table, u8_t nth);
u64_t svc_gpfree(u64_t task_id, u64_t table, u8_t nth);
u64_t svc_gpset(u64_t table, u8_t nth);
u64_t svc_gpclear(u64_t table, u8_t nth);
u64_t svc_gpvalue(u64_t table, u8_t nth, u8_t value);

u64_t svc_create_ipcmailbox(u64_t accessblity, u64_t *whitelist_tasks_id, u64_t *blacklist_tasks_id, u8_t type, u32_t maximum_length);
u64_t svc_write_ipcmailbox(struct ipcmailbox_t *mailbox, u64_t content_pt1, u64_t content_pt2, u64_t done, u64_t receiver_task_id); // set receiver_task_id to 0 for anyone access.
u64_t svc_read_ipcmailbox(struct ipcmailbox_t *mailbox, u64_t *content_pt1, u64_t *content_pt2, u64_t receiver_task_id);            // set receiver_task_id to 0 for any readable messages.
#endif