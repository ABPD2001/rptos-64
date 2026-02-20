#include "./ipcmailbox.h"

struct ipcmailbox_t *alloc_ipcmailbox()
{
    volatile struct ipcmailbox_t *nav_mailbox = global_ipcmailbox_bank;

    for (u64_t i = 0; i < 63; i++)
    {
        u64_t status = (nav_mailbox->metadata & 0x18) >> 3;
        if (!status)
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
        if (!nav_segment->status)
            return nav_segment;
        nav_segment++; // go to next segment.
    }

    return 0; // return 0 as error (out of space).
}

void free_ipcmailbox(volatile struct ipcmailbox_t *mailbox)
{
    mailbox->metadata &= ~(0x18);       // clear status flag in metadata.
    mailbox->accessibility = NULL;      // clear accessiblity.
    mailbox->blacklist_tasks_id = NULL; // clear blacklist.
    mailbox->whitelist_tasks_id = NULL; // clear whitelist.
    mailbox->task_owner = NULL;         // clear task owner.
    mailbox->maximum_length = NULL;     // clear maximum length.
    mailbox->access_mutex = NULL;       // clear mutex.
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

u64_t write_ipcmailbox(volatile struct ipcmailbox_t *mailbox, u64_t content_pt1, u64_t content_pt2, u64_t author_id)
{
    volatile struct ipcmailbox_segment_t *nav_segment = global_ipcmailbox_segments_bank;

    for (u64_t i = 0; i < 511; i++)
    {
        if (!nav_segment->status) // if it was empty
        {
            // lock the mutex.
            nav_segment->context.content_pt1 = content_pt1; // set first part of data.
            nav_segment->context.content_pt2 = content_pt2; // set second part of data.
            nav_segment->context.author_task_id = author_id;
            nav_segment->mailbox_id = mailbox->id;
            nav_segment->mailbox_type = mailbox->metadata & 0x7;

            return 0; // done, no problem.
        }
    }

    return 1; // out of segment space.
}

struct ipcmailbox_message_t read_ipcmailbox(volatile struct ipcmailbox_t *mailbox)
{
    volatile struct ipcmailbox_segment_t *nav_segment = global_ipcmailbox_segments_bank + 56 * 511;
    struct ipcmailbox_message_t output_msg;

    for (u64_t i = 0; i < 512; i++)
    {
        if (nav_segment->mailbox_id == mailbox->id)
        {
            // copy struct pointer into local struct (for security reasons).
            output_msg.content_pt1 = nav_segment->context.content_pt1;
            output_msg.content_pt2 = nav_segment->context.content_pt2;
            output_msg.author_task_id = nav_segment->context.author_task_id;
            output_msg.done = nav_segment->context.done;

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