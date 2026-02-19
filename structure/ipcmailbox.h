#ifndef TYPES_IPC_MAILBOX
#define TYPES_IPC_MAILBOX
#include "./base.h"
#include "./semaphore.h"

typedef struct ipcmailbox_base_t
{
    u64_t task_owner;
    u64_t id;
    u64_t start_region;
    u64_t end_region;
    mutex_t access_mutex; // a mutex, where task wants to write.
    u16_t maximum_length;
    u16_t type; // 0-7: mailbox type, 8-15: mailbox message content size. (1/2/4/8) bytes.
};

typedef struct ipcmailbox_oneline_t : ipcmailbox_base_t // a one line where only master can write/read and slave is Read-Only.
{
    u64_t task_master_id; // only this task can write/read, even managing mailbox.
    u64_t task_slave_id;  // Read-Only.
};

typedef struct ipcmailbox_dual_t : ipcmailbox_base_t // a dual line where both of tasks can write/read, but one of them has more access than the other (managing the mailbox like delete or changing properties).
{
    u64_t task_primary_id; // this task has more access than other.
    u64_t task_secondary_id;
};

typedef struct ipcmailbox_universal_t : ipcmailbox_base_t // a universal access, where tasks generally can read, and kernel only can write to mailbox and controll it, (there is a option to allow tasks to write without limitation).
{
    u64_t *blacklist_tasks_id;
    u64_t accessibility; // 0: write access, 1: read access, 2: management, 3-63: reserved.
};

typedef struct ipcmailbox_message_base_t
{
    u64_t author_task_id;
};

typedef struct ipcmailbox_message_large : ipcmailbox_message_base_t
{
    u64_t content;
};

typedef struct ipcmailbox_message_medium : ipcmailbox_message_base_t
{
    u32_t content;
};

typedef struct ipcmailbox_message_small : ipcmailbox_message_base_t
{
    u16_t content;
};

typedef struct ipcmailbox_message_tiny : ipcmailbox_message_base_t
{
    u8_t content;
};

#endif