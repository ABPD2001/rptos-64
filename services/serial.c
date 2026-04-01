__attributes__((section(".kernel_service_serial")));
#include "../std-lib/header.h"

void kernel_serial_service()
{
    const u64_t tid = taskid();
    if (allocate_serial()) // allocate serial
        terminate(18, 1);  // software failure with dump as code 1 (means failed to allocate muart).

    volatile struct ipcmailbox_t *mailbox = mxcreate(0x3, NULL, NULL, NULL, NULL, 64); // no whitelist.
    struct ipcmailbox_message_t message;
    if (!mailbox)         // create mailbox.
        terminate(18, 2); // with dump of failed to create mailbox (ipc).

    while (1)
    {
        wait(mailbox, WAIT_IPC_MAILBOX_RECEIVE); // wait for message.
        mxread(mailbox->id, &message, tid);      // read mailbox when awake.

        // content_pt1:
        // 0: write/read.
        // 1-33: length.
        // 34-42: delimiter.
        // 43-63: reserved.
        // content_pt2:
        // 0-63: buffer address.

        // also must check validity of buffer, if it was invalid, then push back that info blacklist.
        if (message.content_pt1 & 0x1)                                                                                                                        // if first bit set, its write, else read.
            mxwrite(mailbox->id, serial_put((char *)message.content_pt2, ((u64_t)message.content_pt1) & (0xFFFFFFFF << 1)), 0, true, message.author_task_id); // response output code of mini-uart write.
        else
            mxwrite(mailbox->id, serial_read((char *)message.content_pt2, message.content_pt1 & (0xFFFFFFFF << 1)), 0, true, message.author_task_id); // respone output code of mini-uart read.
    }
    terminate(18, 3); // software failure with dump code of unexcepted termination.
}