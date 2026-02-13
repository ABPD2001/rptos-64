#include "./synchronous.h"

__attribute__((section(".sync_lower_el_handlers")));

void lower_el_wfi_wfe_handler()
{
    const u8_t cid = core_id();
    volatile pcb_t *ctask = __core_info_table__ + 32 + cid * 8;

    ctask->status = 3; // set status to terminated, because direct access to wfi and wfe not granted in EL0.
    task_schaduler();
    task_dispatcher(); // ignore task and do new schaduling (because of saftey reasons).
}