#ifndef STDLIB_IPC_MAILBOX_H
#define STDLIB_IPC_MAILBOX_H
#include "../../base.h"

extern u64_t mxcreate(u64_t accessblity, u64_t whitelist_tasks_pt1_id, u64_t whitelist_tasks_pt2_id, u64_t *blacklist_tasks_id, u8_t type, u32_t maximum_length);
extern u64_t mxwrite(volatile struct ipcmailbox_t *mailbox, u64_t content_pt1, u64_t content_pt2, u64_t done, u64_t receiver_task_id);
extern u64_t mxread(volatile struct ipcmailbox_t *mailbox, struct ipcmailbox_message_t *message, u64_t receiver_task_id);
extern u64_t mxedit(volatile struct ipcmailbox_t *mailbox, struct ipcmailbox_settings_t *settings);
extern u64_t mxaddress(u64_t id);
#endif