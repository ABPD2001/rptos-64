#ifndef SVC_H
#define SVC_H
#include "../../../structure/base.h"
#include "../../../structure/extern.h"
#include "../../../structure/gpio.h"
#include "../../../structure/ipcmailbox.h"
#include "../../../structure/mmu.h"
#include "../../../structure/muart.h"
#include "../../../structure/timer.h"

#include "../../../drivers/muart.h"
#include "../../../drivers/stimer.h"
#include "../../../drivers/gic400.h"
#include "../../../drivers/gpio.h"

#include "../../../lib/core.h"
#include "../../../lib/ipcmailbox.h"
#include "../../../lib/fwlist.h"
#include "../../../lib/memory.h"
#include "../../../lib/kmem.h"
#include "../../../lib/schaduler.h"
#include "../../../lib/softwarelock.h"

// Serial

u64_t svc_muart_write(u8_t *buffer, u64_t length);
u64_t svc_muart_read(u8_t *buffer, u64_t maximum_length);
u64_t svc_muart_write_char(u8_t ch);
u64_t svc_muart_read_char(u8_t *ch);
u8_t svc_muart_availablity();
extern u64_t svc_muart_alloc();
extern u64_t svc_muart_free();

// Task

u64_t svc_get_task_id();
u64_t svc_termination_request(u64_t fault_code, u64_t fault_dump);
u64_t svc_tsleep_ms(u32_t us);
u64_t svc_wait(u64_t instruction, u8_t type);
u64_t svc_spawn_task(u64_t instruction_space, u8_t core_dependent, u8_t core_migration);

// GPIO

u64_t svc_gpalloc(u64_t table, u8_t nth);
u64_t svc_gpfree(u64_t task_id, u64_t table, u8_t nth);
u64_t svc_gpset(u64_t table, u8_t nth);
u64_t svc_gpclear(u64_t table, u8_t nth);
u64_t svc_gpvalue(u64_t table, u8_t nth, u8_t value);
u64_t svc_gpfunction(u64_t table, u8_t nth, u8_t function);

// Inter-Process-Communication Mailboxes

u64_t svc_create_ipcmailbox(u64_t accessblity, u64_t whitelist_tasks_pt1_id, u64_t whitelist_tasks_pt2_id, u64_t *blacklist_tasks_id, u8_t type, u32_t maximum_length);
u64_t svc_write_ipcmailbox(volatile struct ipcmailbox_t *mailbox, u64_t content_pt1, u64_t content_pt2, u64_t done, u64_t receiver_task_id);
u64_t svc_read_ipcmailbox(volatile struct ipcmailbox_t *mailbox, struct ipcmailbox_message_t *message, u64_t receiver_task_id);
u64_t svc_edit_ipcmailbox(volatile struct ipcmailbox_t *mailbox, struct ipcmailbox_settings_t *settings);

// Software locks.

u64_t svc_mutex_gain(u64_t *mutex);
u64_t svc_mutex_release(u64_t *mutex);
u64_t svc_semaphore_gain(u64_t *semaphore);
u64_t svc_semaphore_release(u64_t *semaphore);

// Multi-core

extern u8_t svc_core_id();
extern u8_t svc_cluster_id();

// System

s64_t svc_system_shutdown();
s64_t svc_system_reboot();
#endif