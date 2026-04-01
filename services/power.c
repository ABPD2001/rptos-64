__attribute__((section(".kernel_service_power")));
#include "../std-lib/header.h"

void kernel_service_power()
{
    volatile u64_t mailbox = mxcreate(0x3, NULL, NULL, NULL, NULL, 16); // limited request size.
    struct ipcmailbox_message_t message;

    if (!mailbox)
        terminate(18, 1); // software failure with as code 1 (means failed to create ipc mailbox).

    while (1)
    {
        wait(mailbox, WAIT_IPC_MAILBOX_RECEIVE); // wait for message.
        mxread(mailbox, &message, 0);            // read mailbox when awake.

        if (message.content_pt1 == 1)
        {
            mxwrite(mailbox, 1, NULL, true, message.author_task_id); // send a message pending.
            const s64_t res = system_shutdown();                     // if resulted, means failed.
            if (res)
                mxwrite(mailbox, 1, res, true, message.author_task_id); // send a message as failure
        }
        else if (message.content_pt1 == 2)
        {
            mxwrite(mailbox, 1, NULL, true, message.author_task_id); // send a message pending.
            const s64_t res = system_reboot();                       // if resulted, means failed.
            if (res)
                mxwrite(mailbox, 1, res, true, message.author_task_id); // send a message as failure
        }
        else
        {
            // if wasnt valid, then push back the author task into blacklist.

            volatile struct ipcmailbox_t *mailbox_struct = mailbox;
            u64_t blacklist_pt1 = mailbox_struct->blacklist_tasks_pt1_id;
            u64_t blacklist_pt2 = mailbox_struct->blacklist_tasks_pt2_id;

            if (message.author_task_id > 63)
                blacklist_pt2 |= (1 << message.author_task_id - 63);
            else
                blacklist_pt1 |= (1 << message.author_task_id);

            struct ipcmailbox_settings_t ipcmailbox_settings = {mailbox_struct->task_owner, blacklist_pt1, blacklist_pt2, mailbox_struct->whitelist_tasks_pt1_id, mailbox_struct->whitelist_tasks_pt2_id, mailbox_struct->accessibility, mailbox_struct->maximum_length, mailbox_struct->metadata};
            mxedit(mailbox, &ipcmailbox_settings);
        }
    }
    terminate(18, 2); // software failure with dump code of unexcepted termination.
}