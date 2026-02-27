#ifndef FW_LIST_H
#define FW_LIST_H
#include "../structure/task.h"
#include "../structure/timer.h"

// TASK
typedef struct // 16 B
{
    volatile struct pcb_t *head;
    volatile struct pcb_t *tail;
} fwlist_header_t;

void fw_push_back(fwlist_header_t *header, volatile struct pcb_t *task);
void fw_push_front(fwlist_header_t *header, volatile struct pcb_t *task);
u64_t fw_rm(fwlist_header_t *header, u64_t idx);
volatile struct pcb_t *fw_at(fwlist_header_t *header, u64_t idx);
u64_t fw_len(fwlist_header_t *header);

// TIMER

typedef struct // 16 B
{
    volatile struct timer_request_t *head;
    volatile struct timer_request_t *tail;
} tfwlist_header_t;

void tfw_push_back(tfwlist_header_t *header, volatile struct timer_request_t *request);
volatile struct timer_request_t *tfw_find(volatile struct tfwlist_header_t *header, u64_t id); // task id.
u64_t tfw_idx(volatile struct tfwlist_header_t *header, u64_t id);                             // request id.
u64_t tfw_rm(volatile struct tfwlist_header_t *header, u64_t idx);
#endif