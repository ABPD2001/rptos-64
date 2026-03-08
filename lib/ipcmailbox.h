#ifndef LIB_IPC_MAILBOX_H
#define LIB_IPC_MAILBOX_H
#include "../structure/base.h"
#include "../structure/extern.h"
#include "../structure/ipcmailbox.h"

struct ipcmailbox_t *alloc_ipcmailbox();
struct ipcmailbox_segment_t *alloc_ipcmailboxsegment();

void free_ipcmailbox(volatile struct ipcmailbox_t *mailbox);
void free_ipcmailboxsegment(volatile struct ipcmailbox_segment_t *segment);

u64_t write_ipcmailbox(volatile struct ipcmailbox_t *mailbox, u64_t content_pt1, u64_t content_pt2, u64_t author_id, u64_t receiver_task_id);
struct ipcmailbox_message_t read_ipcmailbox(volatile struct ipcmailbox_t *mailbox, u64_t receiver_task_id);

#endif