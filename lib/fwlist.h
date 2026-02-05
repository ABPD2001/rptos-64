#ifndef FW_LIST_H
#define FW_LIST_H
#include "./types/task.h"
typedef struct // 16 B
{
    struct pcb_t *head;
    struct pcb_t *tail;
} fwlist_header_t;

void fw_push_back(fwlist_header_t *header, pcb_t *task);
void fw_push_front(fwlist_header_t *header, pcb_t *task);
u64_t fw_rm(fwlist_header_t *header, u64_t idx);
pcb_t *fw_at(fwlist_header_t *header, u64_t idx);
u64_t fw_len(fwlist_header_t *header);
#endif