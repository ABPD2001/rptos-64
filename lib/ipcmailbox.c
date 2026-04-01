#include "./ipcmailbox.h"

struct ipcmailbox_t *alloc_ipcmailbox()
{
    volatile struct ipcmailbox_t *nav_mailbox = global_ipcmailbox_bank;

    for (u64_t i = 0; i < 63; i++)
    {
        u64_t status = (nav_mailbox->metadata & 0x6) >> 2;
        if (!status && nav_mailbox->access_mutex)
            return nav_mailbox;
        nav_mailbox++; // go to next mailbox.
    }

    return 0; // return 0 as error (out of space).
}

struct ipcmailbox_segment_t *alloc_ipcmailboxsegment()
{
    volatile struct ipcmailbox_segment_t *nav_segment = global_ipcmailbox_segments_bank;

    for (u64_t i = 0; i < 511; i++)
    {
        if (!nav_segment->status && nav_segment->access_mutex)
            return nav_segment;
        nav_segment++; // go to next segment.
    }

    return 0; // return 0 as error (out of space).
}

void free_ipcmailbox(volatile struct ipcmailbox_t *mailbox)
{
    mailbox->metadata &= ~(0x6);            // clear status flag in metadata.
    mailbox->accessibility = NULL;          // clear accessiblity.
    mailbox->blacklist_tasks_pt1_id = NULL; // clear blacklist.
    mailbox->blacklist_tasks_pt2_id = NULL; // clear blacklist.
    mailbox->whitelist_tasks_pt1_id = NULL; // clear whitelist.
    mailbox->whitelist_tasks_pt2_id = NULL; // clear whitelist.
    mailbox->task_owner = NULL;             // clear task owner.
    mailbox->maximum_length = NULL;         // clear maximum length.
    mailbox->access_mutex = NULL;           // clear mutex.
}

void free_ipcmailboxsegment(volatile struct ipcmailbox_segment_t *segment)
{
    segment->access_mutex = NULL;           // clear access mutex.
    segment->context.author_task_id = NULL; // clear author id of context.
    segment->context.content_pt1 = 0;       // clear first part of segment context.
    segment->context.content_pt2 = 0;       // clear second part of segment context.
    segment->context.done = false;          // clear done flag in context.
    segment->mailbox_id = NULL;             // clear mailbox.
    segment->mailbox_type = 3;              // set mailbox to raw.
}

u64_t write_ipcmailbox(volatile struct ipcmailbox_t *mailbox, u64_t content_pt1, u64_t content_pt2, u64_t author_id, u64_t receiver_task_id) // set receiver_task_id to 0 for host receiver.
{
    volatile struct ipcmailbox_segment_t *segment = alloc_ipcmailboxsegment();
    if (!segment)
        return 1; // out of segment space.

    // gain the mutex.
    while (!gain_mutex(segment->access_mutex))
    {
        spinwait_mutex(segment->access_mutex);
    }

    segment->status = 1;                        // set status flag to 'filling'.
    segment->context.content_pt1 = content_pt1; // set first part of data.
    segment->context.content_pt2 = content_pt2; // set second part of data.
    segment->context.author_task_id = author_id;
    segment->mailbox_id = mailbox->id;
    segment->context.receiver_task_id = receiver_task_id;
    segment->mailbox_type = mailbox->metadata & (0x3);
    segment->mailbox_type = 2; // set status to 'fill'.

    release_mutex(segment->access_mutex); // free mutex.

    return 0; // done, no problem.
}

struct ipcmailbox_message_t read_ipcmailbox(volatile struct ipcmailbox_t *mailbox, u64_t receiver_task_id) // set receiver_task_id to 0 for checking messages for host, 129 for checking all readable messages.
{
    volatile struct ipcmailbox_segment_t *nav_segment = global_ipcmailbox_segments_bank + 56 * 511;
    struct ipcmailbox_message_t output_msg;

    if (receiver_task_id == 0)
        receiver_task_id = mailbox->task_owner;

    for (u64_t i = 0; i < 512; i++)
    {
        if (nav_segment->mailbox_id == mailbox->id && nav_segment->status == 2 && nav_segment->access_mutex)
        {
            if (receiver_task_id != 129 && nav_segment->context.receiver_task_id != receiver_task_id) // if receiver task id wasnt 0 and wasnt 129 and context receiver task id wasnt same with receiver task id.
                continue;

            // copy struct pointer into local struct (for security reasons).

            // gain mutex of nav_segment.
            while (!gain_mutex(nav_segment->access_mutex))
            {
                spinwait_mutex(nav_segment->access_mutex);
            }

            nav_segment->status = 3; // set status to 'reading'.
            output_msg.content_pt1 = nav_segment->context.content_pt1;
            output_msg.content_pt2 = nav_segment->context.content_pt2;
            output_msg.author_task_id = nav_segment->context.author_task_id;
            output_msg.done = nav_segment->context.done;
            nav_segment->status = 2; // set status to 'fill'.

            release_mutex(nav_segment->access_mutex); // release nav_segment lock.

            return output_msg;
        }

        nav_segment--; // back to previous segment.
    }

    // set empty struct as empty queue.
    output_msg.author_task_id = 0;
    output_msg.content_pt1 = 0;
    output_msg.content_pt2 = 0;
    output_msg.done = 0;

    return output_msg;
}

void edit_ipcmailbox(volatile struct ipcmailbox_t *mailbox, struct ipcmailbox_settings_t settings)
{
    // gain lock.
    while (!gain_mutex(mailbox->access_mutex))
    {
        spinwait_mutex(mailbox->access_mutex);
    }

    mailbox->accessibility = settings.accessibility;
    mailbox->blacklist_tasks_pt1_id = settings.blacklist_tasks_pt1_id;
    mailbox->blacklist_tasks_pt2_id = settings.blacklist_tasks_pt2_id;
    mailbox->whitelist_tasks_pt2_id = settings.whitelist_tasks_pt2_id;
    mailbox->whitelist_tasks_pt2_id = settings.whitelist_tasks_pt2_id;
    mailbox->maximum_length = settings.maximum_length;
    mailbox->task_owner = settings.task_owner;
    mailbox->metadata = settings.metadata;
    release_mutex(mailbox->access_mutex); // release lock.
}

u64_t is_ipc_empty(volatile struct ipcmailbox_t *mailbox, u64_t receiver_task_id)
{
    volatile struct ipcmailbox_segment_t *nav_segment = global_ipcmailbox_segments_bank;

    for (u64_t i = 0; i < 1024; i++)
    {
        if (nav_segment->context.receiver_task_id == receiver_task_id)
            return false; // return its not empty.
    }
    return true; // return its emprt.
}

u64_t ipc_not_allowed_in_lists(volatile struct ipcmailbox_t *mailbox, u64_t task_id)
{
    if ((mailbox->blacklist_tasks_pt1_id || mailbox->blacklist_tasks_pt2_id) && (mailbox->whitelist_tasks_pt1_id || mailbox->whitelist_tasks_pt2_id))
        return 1; // invalid input.
    if (task_id < 63 && mailbox->blacklist_tasks_pt1_id && (mailbox->blacklist_tasks_pt1_id & (1 << task_id)))
        return 2; // task included in blacklist.
    else if (task_id > 63 && mailbox->blacklist_tasks_pt2_id && (mailbox->blacklist_tasks_pt2_id & (1 << task_id - 63)))
        return 2; // task included in blacklist.
    else if (task_id < 63 && mailbox->whitelist_tasks_pt1_id && !(mailbox->whitelist_tasks_pt1_id & (1 << task_id)))
        return 3; // task does not included in whitelist.
    else if (task_id > 63 && mailbox->whitelist_tasks_pt2_id && !(mailbox->whitelist_tasks_pt2_id & (1 << task_id - 63)))
        return 3; // task does not included in whitelist.

    return 0; // included
};