#include "./fwlist.h"

void fw_push_back(fwlist_header_t *header, pcb_t *task)
{
    if (header->head == NULL && header->tail == NULL)
    {
        header->head = task;
        header->tail = header->head;
        return;
    }

    header->tail->next = task; // set pointer of headers tail to new task.
    header->tail = task;       // set new tail for header.
}

void fw_push_front(fwlist_header_t *header, pcb_t *task)
{
    if (header->head == NULL && header->tail == NULL)
    {
        header->head = task;
        header->tail = header->head;
        return;
    }
    task->next = header->head; // set pointer of new task to headers head.
    header->head = task;       // set new head for header.
}

u64_t fw_rm(fwlist_header_t *header, u64_t idx)
{
    pcb_t *pointed_task = header->head;
    pcb_t *prev_task = NULL;

    idx--;
    if (target_task == NULL)
        return 1;
    for (; idx != -1; idx--)
    {
        if (pointed_task->next == NULL)
        {
            if (idx)
                return 2;
            break;
        }
        prev_task = pointed_task;
        pointed_task = pointed_task->next;
    }

    if (pointed_task == header->tail && pointed_task == header->head)
    {
        header->head = NULL;
        header->tail = NULL;

        return 0;
    }

    if (pointed_task == header->head)
    {

        header->head = pointed_task->next;
        pointed_task->next = NULL; // just in case...
    }
    else if (pointed_task == header->tail)
    {

        header->tail = prev_task;
        prev_task->next = NULL; // just in case...
    }

    return 0;
}

pcb_t *fw_at(fwlist_header_t *header, u64_t idx)
{
    pcb_t *target_task = header->head;
    idx--;

    if (target_task == NULL)
        return 1;
    for (; idx != -1; idx--)
    {
        if (target_task->next == NULL)
        {
            if (idx)
                return 2;
            break;
        }
        target_task = target_task->next;
    }

    return target_task;
}

u64_t fw_len(fwlist_header_t *header)
{
    if (header->head == NULL && header->tail == NULL)
        return 0;
    pcb_t *temp_task = header->head;
    u64_t length = 1; // 1 initial value, because its already pointing on a item...

    while (temp_task->next == NULL)
    {
        length++;
        temp_task = temp_task->next;
    }

    return length;
}