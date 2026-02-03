#ifndef FW_LIST_H
#define FW_LIST_H
#include "./types/task.h"

typedef struct
{
    struct pcb_t *pcb;
    struct fwlist_elm_t *next;
} fwlist_elm_t;

typedef struct
{
    struct fwlist_elm_t *head;
    struct fwlist_elm_t *tail;
} fwlist_header_t;

#endif