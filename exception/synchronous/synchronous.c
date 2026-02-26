__attribute__((section(".sync_lower_el_handlers")));
#include "./synchronous.h"

void lower_el_wfi_wfe_handler()
{
    const u8_t cid = core_id();
    volatile struct pcb_t *ctask = __core_info_table__ + 32 + cid * 8;

    ctask->status = 3; // set status to terminated, because direct access to wfi and wfe not granted in EL0.
    task_schaduler();
    task_dispatcher(); // ignore task and do new schaduling (because of saftey reasons).
}

void lower_el_unkown_handler(u64_t syndrome)
{
    const u8_t cid = core_id();
    volatile struct pcb_t *ctask = __core_info_table__ + 32 + cid * 8;

    ctask->status = 3;                        // set status to terminated.
    ctask->fault_code = 3;                    // set fault code to unkown instruction.
    ctask->fault_dump = syndrome & 0x1FFFFFF; // masked 0:24 bits of syndrome.
}

void lower_el_sp_alignment_handler()
{
    u64_t far;

    __asm__ volatile(
        "mrs %0,FAR_EL1"
        : "=r"(far)
        :
        :);

    const u8_t cid = core_id();
    volatile struct pcb_t *ctask = __core_info_table__ + 32 + cid * 8;

    ctask->status = 3;       // set status to terminated.
    ctask->fault_code = 1;   // set fault code to stack alignment fault.
    ctask->fault_dump = far; // set fault dump to cause memory address.
}

void lower_el_pc_alignment_handler()
{
    u64_t elr;
    __asm__ volatile(
        "ldr %0, [sp]"
        : "=r"(elr)
        :
        :);

    const u8_t cid = core_id();
    volatile struct pcb_t *ctask = __core_info_table__ + 32 + cid * 8;

    ctask->status = 3;       // set status to terminated.
    ctask->fault_code = 2;   // set fault code to pc alignment fault.
    ctask->fault_dump = elr; // set fault dump to address of cause instruction.
}