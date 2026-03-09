#ifndef TYPES_TASK_H
#define TYPES_TASK_H
#include "./base.h"
#include "./mmu.h"

struct pcb_t // 368 Bytes
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
    u64_t status;                             // 0: created, 1:ready, 2:running, 3:terminated, 4:waiting.
    u64_t priority;                           // 0~7: 0 is least and 7 is most.
    volatile struct memframes_header_t pages; // pointer to memory pages.
    u64_t fault_code;                         // 1: stack alignment fault ,2: pc alignment fault
    u64_t fault_dump;
    u64_t preipherals; // using this to free all allocated preipherals by this task when this task is terminating. --> each 4 bits is a preipheral id. (0000 --> mini uart, 0001 --> uart-0, 0010 --> uart-2, 0011 --> uart-3, 0100 --> uart-4, 0101 --> uart-5, 0110 --> timer request, 0111 --> gpio ownership, 1000 --> software lock gained, 1001 --> ipc mailbox owned)
    u64_t preipherals_count;
    volatile struct pcb_t *parent; // if is null, itself is parent.
    u64_t pc;                      // program counter.
    u64_t ttbr;                    // TTBR.
    volatile struct pcb_t *next;
    u64_t event_handlers[3]; // for address for 4 handlers (for event betwen 1~3 and 0 is only kernel access).
    u64_t wait_instruction;  // something like descritpion for wait statement.
    u8_t wait_reason;        // 0: waiting for mini-uart (tx), 1: waiting for mini-uart (rx), 2: waiting for mini-uart to free, 3: waiting for mini-uart to allocate ,4: waiting for timer requests, 5: waiting for true value of address, 6: waiting for false value of address, 7: waiting for mailbox (receive), 8: waiting for event.
    u8_t flags;              // 0-1: dedicated core, 2: ready flag.
    u8_t event_number;       // 0: termination (force), 1: termination (soft), 2: pause, 3: resume
    u8_t reserved[5];        // padding.
};

#endif