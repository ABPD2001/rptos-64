__attribute__((section(".sync_lower_el_handlers")));
#include "./synchronous.h"

void lower_el_wfi_wfe_handler()
{
    const u8_t cid = core_id();
    volatile struct pcb_t *ctask = __core_info_table__ + 32 + cid * 8;

    ctask->status = 3;     // set status to terminated, because direct access to wfi and wfe not granted in EL0.
    ctask->fault_code = 4; // set fault code to security breach (wfi/wfe).
    task_schaduler();
    task_dispatcher(); // ignore task and do new schaduling (because of saftey reasons).
}

void lower_el_unkown_handler()
{
    u64_t syndrome;
    asm volatile("mrs %0,ESR_EL1"
                 : "=r"(syndrome)
                 :
                 :);

    const u8_t cid = core_id();
    volatile struct pcb_t *ctask = __core_info_table__ + 32 + cid * 8;

    ctask->status = 3;                        // set status to terminated.
    ctask->fault_code = 3;                    // set fault code to unkown instruction.
    ctask->fault_dump = syndrome & 0x1FFFFFF; // masked 0:24 bits of syndrome.

    task_schaduler();
    task_dispatcher(); // ignore task and do new schaduling (because of saftey reasons).
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

    task_schaduler();
    task_dispatcher(); // ignore task and do new schaduling (because of saftey reasons).
}

void lower_el_pc_alignment_handler()
{
    u64_t elr;
    __asm__ volatile(
        "mrs %0,ELR_EL1"
        : "=r"(elr)
        :
        :);

    const u8_t cid = core_id();
    volatile struct pcb_t *ctask = __core_info_table__ + 32 + cid * 8;

    ctask->status = 3;       // set status to terminated.
    ctask->fault_code = 2;   // set fault code to pc alignment fault.
    ctask->fault_dump = elr; // set fault dump to address of cause instruction.

    task_schaduler();
    task_dispatcher(); // ignore task and do new schaduling (because of saftey reasons).
}

void lower_el_simd_fp_err_handler()
{
    u64_t elr;
    __asm__ volatile(
        "mrs %0,ELR_EL1"
        : "=r"(elr)
        :
        :);

    const u8_t cid = core_id();
    volatile struct pcb_t *ctask = __core_info_table__ + 32 + cid * 8;

    ctask->status = 3;       // set status to terminated.
    ctask->fault_code = 3;   // set fault code to simd/fp fault.
    ctask->fault_dump = elr; // set fault dump to address of cause instruction.

    task_schaduler();
    task_dispatcher(); // ignore task and do new schaduling (because of saftey reasons).
}

void lower_el_fp_err_handler()
{
    u64_t elr;
    __asm__ volatile(
        "mrs %0,ELR_EL1"
        : "=r"(elr)
        :
        :);

    const u8_t cid = core_id();
    volatile struct pcb_t *ctask = __core_info_table__ + 32 + cid * 8;

    ctask->status = 3;       // set status to terminated.
    ctask->fault_code = 5;   // set fault code to instruction abort fault.
    ctask->fault_dump = elr; // set fault dump to address of cause instruction.

    task_schaduler();
    task_dispatcher(); // ignore task and do new schaduling (because of saftey reasons).
}

void lower_el_d_abort_lower_handler()
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
    ctask->fault_code = 5;   // set fault code to data abort fault.
    ctask->fault_dump = far; // set fault dump to address of cause instruction.

    task_schaduler();
    task_dispatcher(); // ignore task and do new schaduling (because of saftey reasons).
}

void lower_el_i_abort_lower_handler()
{
    u64_t elr;
    __asm__ volatile(
        "mrs %0,ELR_EL1"
        : "=r"(elr)
        :
        :);

    const u8_t cid = core_id();
    volatile struct pcb_t *ctask = __core_info_table__ + 32 + cid * 8;

    ctask->status = 3;       // set status to terminated.
    ctask->fault_code = 4;   // set fault code to pc alignment fault.
    ctask->fault_dump = elr; // set fault dump to address of cause instruction.

    task_schaduler();
    task_dispatcher(); // ignore task and do new schaduling (because of saftey reasons).
}
void lower_el_d_abort_lower_handler();

void same_el_wfi_wfe_handler()
{
    volatile struct system_exceptions_statistics_t *exceptions_statistics = generic_system_exception_statistics_base;
    exceptions_statistics += core_id(); // calculate absolute pointer to struct.

    exceptions_statistics->kernel_wfi_wfe_counts++; // just increment.
}

void same_el_fp_error_handler()
{
    u64_t cpacr;
    asm volatile("mrs %0, CPACR_EL1"
                 : "=r"(cpacr)
                 :
                 :);

    if (!(cpacr & (1 << 20))) // if wasnt powered up.
        cpacr |= (1 << 20);   // enable fpu for kernel only.
    else
        system_panic(); // panic.
}

void same_el_step_handler()
{
    volatile struct system_exceptions_statistics_t *exceptions_statistics = generic_system_exception_statistics_base;
    exceptions_statistics += core_id(); // calculate absolute pointer to struct.

    exceptions_statistics->kernel_wfi_wfe_counts++; // just increment.
}

void same_el_sp_alignment_handler()
{
    u64_t sp;
    asm volatile(
        "mrs %0,SP_EL1"
        : "=r"(sp)
        :
        :);

    sp += 16 - sp % 16; // align stack pointer.

    asm volatile(
        "msr SP_EL1, %0"
        : "=r"(sp)
        :
        :);
}

void same_el_breakpoint_handler()
{
    u64_t elr;
    u64_t spsr;
    u64_t sp;

    asm volatile("mrs %0,ELR_EL1"
                 : "=r"(elr)
                 :
                 :);

    asm volatile("mrs %0, SPSR_EL1"
                 : "=r"(spsr)
                 :
                 :);

    asm volatile("mrs %0, SP_EL1"
                 : "=r"(sp)
                 :
                 :)

        volatile struct breakpoint_t *breakpoint = generic_system_breakpoints_base;
    breakpoint += core_id() * 2;

    if (breakpoint->time > (breakpoint + 1)->time)
        breakpoint++; // use next breakpoint.

    breakpoint->pc = elr;
    breakpoint->spsr = spsr;
    breakpoint->time = read_stimer_us();

    for (u64_t i = 0; i < 128; i++)
    {
        breakpoint->sp[i] = ((char *)sp)[i];
    }
}

void same_el_simd_fp_err_handler()
{
    u64_t cpacr;
    asm volatile("mrs %0, CPACR_EL1"
                 : "=r"(cpacr)
                 :
                 :);

    if (!(cpacr & (1 << 20))) // if wasnt powered up.
        cpacr |= (1 << 20);   // enable fpu for kernel only.
    else
        system_panic(); // panic.
}