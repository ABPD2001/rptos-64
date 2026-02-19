#ifndef LIB_IPC_MAILBOX_H
#define LIB_IPC_MAILBOX_H
#include "../structure/base.h"
#include "../structure/ipcmailbox.h"

// universal IPC-MAILBOX

ipcmailbox_universal_t *alloc_uniipcmailbox();
void free_uniipcmailbox(ipcmailbox_universal_t *mailbox);

u64_t largewrite_uniipcmailbox(ipcmailbox_universal_t *mailbox, u64_t content, u64_t id);  // 0 for id if its by kernel.
u64_t mediumwrite_uniipcmailbox(ipcmailbox_universal_t *mailbox, u32_t content, u64_t id); // 0 for id if its by kernel.
u64_t smallwrite_uniipcmailbox(ipcmailbox_universal_t *mailbox, u16_t content, u64_t id);  // 0 for id if its by kernel.
u64_t tinywrite_uniipcmailbox(ipcmailbox_universal_t *mailbox, u8_t content, u64_t id);    // 0 for id if its by kernel.

// one line IPC-MAILBOX

ipcmailbox_oneline_t *alloc_onelineipcmailbox();
void free_onelineipcmailbox(ipcmailbox_oneline_t *mailbox);

u64_t largewrite_uniipcmailbox(ipcmailbox_oneline_t *mailbox, u64_t content, u64_t id);  // 0 for id if its by kernel.
u64_t mediumwrite_uniipcmailbox(ipcmailbox_oneline_t *mailbox, u32_t content, u64_t id); // 0 for id if its by kernel.
u64_t smallwrite_uniipcmailbox(ipcmailbox_oneline_t *mailbox, u16_t content, u64_t id);  // 0 for id if its by kernel.
u64_t tinywrite_uniipcmailbox(ipcmailbox_oneline_t *mailbox, u8_t content, u64_t id);    // 0 for id if its by kernel.

// dual line IPC-MAILBOX

#endif