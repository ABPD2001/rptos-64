#ifndef TYPES_IPC_MAILBOX
#define TYPES_IPC_MAILBOX
#include "./base.h"
#include "./softwarelock.h"

// <--- BASE STRUCTURE --->
struct ipcmailbox_t // 56 Bytes.
{
    u64_t task_owner;
    u64_t id;
    mutex_t access_mutex;         // a mutex, where task wants to write.
    u64_t blacklist_tasks_pt1_id; // this list cant access (if isnt NULL).
    u64_t blacklist_tasks_pt2_id; // this list cant access (if isnt NULL).
    u64_t whitelist_tasks_pt1_id; // this list can access only (if isnt NULL).
    u64_t whitelist_tasks_pt2_id; // this list can access only (if isnt NULL).
    u64_t accessibility;          // 0: write access, 1: read access, 2: management, 3-63: reserved.
    u32_t maximum_length;
    u32_t metadata; // 0-1: 0->universal, 1->oneline, 2->dual, 2-3: (0 -> empty, 1 -> filling, 2 -> fill, 3 -> reading), 4-31: reserved.
};

struct ipcmailbox_settings_t // settings for only function call.
{
    u64_t task_owner;
    u64_t blacklist_tasks_pt1_id; // this list cant access (if isnt NULL).
    u64_t blacklist_tasks_pt2_id; // this list cant access (if isnt NULL).
    u64_t whitelist_tasks_pt1_id; // this list can access only (if isnt NULL).
    u64_t whitelist_tasks_pt2_id; // this list can access only (if isnt NULL).
    u64_t accessibility;          // 0: write access, 1: read access, 2: management, 3-63: reserved.
    u32_t maximum_length;
    u32_t metadata; // 0-1: 0->universal, 1->oneline, 2->dual, 2-3: (0 -> empty, 1 -> filling, 2 -> fill, 3 -> reading), 4-31: reserved.
};

// every ipc is universal-based.

// but in dual:
// we set whitelist for only one task, and set accessiblity to write and read.

// and in one line:
// we set whitelist for only one task, and set accessiblity to read.

// and metadata, is for declare type of mailbox for automatic management of mailbox by kernel, and for declaring general status to kernel.

// <--- CONTENT MANAGEMENT --->

struct ipcmailbox_message_t
{
    u64_t author_task_id;   // 0 for host.
    u64_t receiver_task_id; // 0 for anyone.
    u64_t content_pt1;      // content first 8 Bytes.
    u64_t content_pt2;      // content second 8 Bytes. (if content size is 16 bytes)
    u64_t done;             // set this 1 if is done.
};

struct ipcmailbox_segment_t // 64 Bytes.
{
    mutex_t access_mutex; // a mutex, where task wants to write/read.
    u64_t mailbox_id;
    u32_t mailbox_type;                  // 0: universal, 1: oneline, 2: dual, 3: raw.
    u32_t status;                        // 0: empty, 1: filling, 2: full, 3: reading. this is just for general information about mutex.
    struct ipcmailbox_message_t context; // usable context.
};

#endif