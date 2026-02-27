#ifndef TYPES_TASK_H
#define TYPES_TASK_H
#include "./base.h"

typedef struct pcb_t // 368  B
{
    // its recommended to apply simd registers later...
    u64_t spsr;
    u64_t x30;
    u64_t x29;
    u64_t x28;
    u64_t x27;
    u64_t x26;
    u64_t x25;
    u64_t x24;
    u64_t x23;
    u64_t x22;
    u64_t x21;
    u64_t x20;
    u64_t x19;
    u64_t x18;
    u64_t x17;
    u64_t x16;
    u64_t x15;
    u64_t x14;
    u64_t x13;
    u64_t x12;
    u64_t x11;
    u64_t x10;
    u64_t x9;
    u64_t x8;
    u64_t x7;
    u64_t x6;
    u64_t x5;
    u64_t x4;
    u64_t x3;
    u64_t x2;
    u64_t x1;
    u64_t x0;
    u64_t sp;
    u64_t id;
    u64_t status;   // 0: created, 1:ready, 2:running, 3:terminated, 4:waiting, 5:sleeping
    u64_t priority; // 0~7: 0 is least and 7 is most.
    u64_t stack_start;
    u64_t stack_end;
    u64_t fault_code;  // 1: stack alignment fault ,2: pc alignment fault
    u64_t preipherals; // using this to free all allocated preipherals by this task when this task is terminating. --> each 4 bits is a preipheral id. (0000 --> mini uart, 0001 --> uart-0, 0010 --> uart-2, 0011 --> uart-3, 0100 --> uart-4, 0101 --> timer request, 0110 --> gpio ownership, 0111 --> software lock gained, 1000 --> ipc mailbox owned)
    u64_t preipherals_count;
    u64_t parent;
    u64_t *childs;
    u64_t fault_dump;
    u64_t flags; // 0-1: dedicated core, 2: ready flag.
    u64_t pc;    // program counter.
    struct pcb_t *next;
};

#endif