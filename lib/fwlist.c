#include "./fwlist.h"

void fw_push_back(fwlist_header_t *header, volatile struct pcb_t *task)
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

void fw_push_front(fwlist_header_t *header, volatile struct pcb_t *task)
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
    volatile struct pcb_t *pointed_task = header->head;
    volatile struct pcb_t *prev_task = NULL;

    idx--;
    if (pointed_task == NULL)
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

volatile struct pcb_t *fw_at(fwlist_header_t *header, u64_t idx)
{
    volatile struct pcb_t *target_task = header->head;
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
    volatile struct pcb_t *temp_task = header->head;
    u64_t length = 1; // 1 initial value, because its already pointing on a item...

    while (temp_task->next == NULL)
    {
        length++;
        temp_task = temp_task->next;
    }

    return length;
}

void tfw_push_back(volatile struct tfwlist_header_t *header, volatile struct timer_request_t *request)
{
    if (header->head == NULL && header->tail == NULL)
    {
        header->head = request;
        header->tail = header->head;
        return;
    }

    header->tail->next = request; // set pointer of headers tail to new request.
    header->tail = request;       // set new tail for header.
}

volatile struct timer_request_t *tfw_find(volatile struct tfwlist_header_t *header, u64_t id)
{
    volatile struct timer_request_t *temp_request = header->head;

    if (temp_request == NULL)
        return 1;
    // means list is empty.

    while (1)
    {
        if (temp_request->task_id == id)
            return temp_request;
        if (temp_request->next == NULL)
            break;
        temp_request = temp_request->next; // seek to next timer request.
    }

    return 0; // means not found.
}

u64_t tfw_rm(volatile struct tfwlist_header_t *header, u64_t idx)
{
    volatile struct timer_request_t *pointed_request = header->head;
    volatile struct timer_request_t *prev_request = NULL;

    idx--;
    if (pointed_request == NULL)
        return 1;
    for (; idx != -1; idx--)
    {
        if (pointed_request->next == NULL)
        {
            if (idx)
                return 2;
            break;
        }
        prev_request = pointed_request;
        pointed_request = pointed_request->next;
    }

    if (pointed_request == header->tail && pointed_request == header->head)
    {
        header->head = NULL;
        header->tail = NULL;

        return 0;
    }

    if (pointed_request == header->head)
    {

        header->head = pointed_request->next;
        pointed_request->next = NULL; // just in case...
    }
    else if (pointed_request == header->tail)
    {

        header->tail = prev_request;
        prev_request->next = NULL; // just in case...
    }

    return 0;
}

u64_t tfw_idx(tfwlist_header_t *header, u64_t id)
{
    timer_request_t *temp_req = header->head;
    u64_t idx = 0;

    while (1)
    {
        if (temp_req->id == id)
            return idx; // if found, return idx.
        idx++;
        if (temp_req->next == NULL)
            break;
        temp_req = temp_req->next; // seek to next request in queue.
    }
}