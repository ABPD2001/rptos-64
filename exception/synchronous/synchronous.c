__attribute__((section(".sync_lower_el_handlers")));
#include "./synchronous.h"

void lower_el_wfi_wfe_handler()
{
    const u8_t cid = core_id();
    volatile struct pcb_t *ctask = core_tasks[cid];

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
    volatile struct pcb_t *ctask = core_tasks[cid];

    ctask->status = 3;                        // set status to terminated.
    ctask->fault_code = 9;                    // set fault code to unkown instruction.
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
    volatile struct pcb_t *ctask = core_tasks[cid];

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
    volatile struct pcb_t *ctask = core_tasks[cid];

    ctask->status = 3;       // set status to terminated.
    ctask->fault_code = 0;   // set fault code to pc alignment fault.
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
    volatile struct pcb_t *ctask = core_tasks[cid];

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
    volatile struct pcb_t *ctask = core_tasks[cid];

    ctask->status = 3;       // set status to terminated.
    ctask->fault_code = 5;   // set fault code to instruction abort fault.
    ctask->fault_dump = elr; // set fault dump to address of cause instruction.

    task_schaduler();
    task_dispatcher(); // ignore task and do new schaduling (because of saftey reasons).
}

void lower_el_d_abort_lower_handler()
{
    const u8_t cid = core_id();
    volatile struct pcb_t *ctask = core_tasks[cid];

    u64_t iss;
    u64_t far;

    asm volatile("mrs %0,ESR_EL1"
                 : "=r"(iss2)
                 :
                 :);

    asm volatile("mrs %0,FAR_EL1"
                 : "=r"(far)
                 :
                 :);

    iss &= 0xffffff << 31; // mask only iss.
    const u8_t dscf = iss & 0x1f;

    if (dscf > 0b010000 && dscf > 0b011000) // if it was synchronous external abort
    {
        const u8_t state = iss & 0b11 << 11;
        if (state == 0b11) // if external abort was restartable
            return;        // back from routine, this cause another exceution of tasks and retrying again.

        system_panic(); // else panic, because repairment routine is too heavy to execute.
    }

    if (dscf >= 0b001100 && dscf <= 0b001111)
    {                            // if it was permision fault.
        ctask->status = 3;       // set status to terminated.
        ctask->fault_code = 15;  // set fault code to segmentation fault.
        ctask->fault_dump = far; // set fault dump to address of cause instruction.

        task_schaduler();
        task_dispatcher(); // ignore task and do new schaduling (because of saftey reasons).
    }
    if (dscf == 0b100001)
    {                            // if it was virtual alignment fault
        ctask->status = 3;       // set status to terminated.
        ctask->fault_code = 16;  // set fault code to virtual alignemnt fault.
        ctask->fault_dump = far; // set fault dump to address of cause instruction.

        task_schaduler();
        task_dispatcher(); // ignore task and do new schaduling (because of saftey reasons).
    }

    volatile struct memframe_t *temp_page = NULL;
    u64_t pages_count = 0;

    // calculate current count of memory pages (task).
    while (1)
    {
        if (!temp_page)
        {
            temp_page = ctask->pages.head; // set temporary page pointer to head.
            pages_count++;
            temp_page = temp_page->next_frame; // navigate to next frame.
            continue;
        }
        if (temp_page->frame_id == ctask->pages.tail->frame_id)
            break;
        temp_page = temp_page->next_frame; // navigate to next frame.
        pages_count++;
    }

    u64_t attmepted_pages_count = (far - pages_count * 4 * KB) / 4 * KB;

    if (attmepted_pages_count > 2)
    {                            // if attempted range was more than two pages, then attempt was invalid.
        ctask->status = 3;       // set status to terminated.
        ctask->fault_code = 6;   // set fault code to data abort fault.
        ctask->fault_dump = far; // set fault dump to address of cause instruction.

        task_schaduler();
        task_dispatcher(); // ignore task and do new schaduling (because of saftey reasons).
    }

    // allocate pages for attempt.
    for (u64_t i = 0; i < attmepted_pages_count; i++)
    {
        volatile struct memframe_t *allocated_page = alloc_page(ctask->id); // allocate a page.

        if (!allocated_page)
        {
            ctask->status = 3;       // set status to terminated.
            ctask->fault_code = 13;  // set fault code to memory page reservation failure.
            ctask->fault_dump = far; // set fault dump to address of cause instruction.

            task_schaduler();
            task_dispatcher(); // ignore task and do new schaduling (because of saftey reasons).
        }

        mh_push_back(ctask->pages.head, ctask->pages.head, allocated_page); // push page into memory pages of task.

        volatile struct kmem_page_t *kframe = determine_frame(ctask->ttbr); // determine id of allocated frame for virtualization of frame.
        volatile u64_t *kframe_pointer = ctask->ttbr;                       // set a pointer to navigate into kernel frame.

        if (!kframe)
        {
            ctask->status = 3;       // set status to terminated.
            ctask->fault_code = 14;  // set fault code to memory pages virtualization fault.
            ctask->fault_dump = far; // set fault dump to address of cause instruction.

            task_schaduler();
            task_dispatcher(); // ignore task and do new schaduling (because of saftey reasons).
        }

        // note: each 2KB is 256 dwords, each 8KB is 1024 dwords, each 32KB is 4096 dwords, each 128KB is 16384 dwords and each 512KB is 65536 dwords.
        if (2 + (kframe->size / 1024 == 512 ? 8 : kframe->size / 1024 == 128 ? 2
                                                                             : 1) +
                pages_count >=
            kframe->size / 8) // if it was exceeded the limit of its frame size.
        {
            if (kframe->size / 1024 == 512) // if it was 512KB frame (largest possible size of kernel frame size allocation).
            {
                //  set status to terminated.
                ctask->fault_code = 14;  // set fault code to memory pages virtualization fault.
                ctask->fault_dump = far; // set fault dump to address of cause instruction.

                task_schaduler();
                task_dispatcher();
            }

            volatile struct kmem_page_t *new_alloc_kframe = alloc_kframe(kframe->size / 1024 * 4 * KB); // allocate new quad size of current size.
            if (!new_alloc_kframe)
            {
                //  set status to terminated.
                ctask->fault_code = 14;  // set fault code to memory pages virtualization fault.
                ctask->fault_dump = far; // set fault dump to address of cause instruction.

                task_schaduler();
                task_dispatcher();
            }
            volatile u64_t *new_allocated_frame_pointer = new_alloc_kframe->start_address; // set a pointer for new allocated frame.
            // transfer l0 and l1 table descriptors.
            new_allocated_frame_pointer[0] = kframe_pointer[0]; // l0 -1
            new_allocated_frame_pointer[1] = kframe_pointer[1]; // l1 - 1
            new_allocated_frame_pointer[2] = kframe_pointer[2]; // l2 - 1

            if (new_alloc_kframe->size / 1024 == 512)
            {                                                       // if new frame was 512KB.
                new_allocated_frame_pointer[3] = kframe_pointer[3]; // l2 - 2
                for (u64_t i = 0; i < pages_count; i++)
                {
                    new_allocated_frame_pointer[i + 9] = kframe_pointer[i + 4]; // transfer l3 page descriptors into new kernel frame.
                }
            }
            else
            {
                for (u64_t i = 0; i < pages_count; i++)
                {
                    new_allocated_frame_pointer[i + 4] = kframe_pointer[i + 3]; // transfer l3 page descriptors into new kernel frame.
                }
            }

            // after all transfer and frame re-organization...

            free_frame(kframe->id);                       // free previous frame.
            kframe = new_alloc_kframe;                    // set frame to new allocated frame.
            kframe_pointer = new_allocated_frame_pointer; // set frame pointer to new allocated frame pointer.

            if (kframe->size / 1024 == 512)
            {
                if (pages_count + 1 > 8192 * 7)
                    kframe_pointer[9] = (kframe_pointer[57353] & 0xfffffffff) << 11; // set last l2 table descriptor (it points to l3 no. 57344).
                if (pages_count + 1 > 8192 * 6)
                    kframe_pointer[8] = (kframe_pointer[49161] & 0xfffffffff) << 11; // set 7th l2 table descriptor (it points to l3 no. 49161).
                if (pages_count + 1 > 8192 * 5)
                    kframe_pointer[7] = (kframe_pointer[40969] & 0xfffffffff) << 11; // set 6th l2 table descriptor (it points to l3 no. 40969).
                if (pages_count + 1 > 8192 * 4)
                    kframe_pointer[6] = (kframe_pointer[32777] & 0xfffffffff) << 11; // set 5th l2 table descriptor (it points to l3 no. 32777).
                if (pages_count + 1 > 8192 * 3)
                    kframe_pointer[5] = (kframe_pointer[24585] & 0xfffffffff) << 11; // set 4th l2 table descriptor (it points to l3 no. 24585).
                if (pages_count + 1 > 8192 * 2)
                    kframe_pointer[4] = (kframe_pointer[16393] & 0xfffffffff) << 11; // set third l2 table descriptor (it points to l3 no. 16393).
                if (pages_count + 1 > 8192)
                    kframe_pointer[3] = (kframe_pointer[8201] & 0xfffffffff) << 11; // set second l2 table descriptor (it points to l3 no. 8201).

                kframe_pointer[pages_count + 10] = (allocated_page->start_address * 0xfffffffff) << 11; // define new l3 page descriptor.
            }
            else if (kframe->size / 1024 == 128)
            {
                if (pages_count + 1 > 8192 * 2)
                    kframe_pointer[4] = (kframe_pointer[16388] & 0xfffffffff) << 11; // set third l2 table descriptor (it points to l3 no. 16393).
                if (pages_count + 1 > 8192)
                    kframe_pointer[3] = (kframe_pointer[8196] & 0xfffffffff) << 11; // set second l2 table descriptor (it points to l3 no. 8201).

                kframe_pointer[pages_count + 4] = (allocated_page->start_address * 0xfffffffff) << 11; // define new l3 page descriptor.
            }
            else
                kframe_pointer[pages_count + 3] = (allocated_page->start_address * 0xfffffffff) << 11; // define new l3 page descriptor.
        }
        else
            kframe_pointer[3 + pages_count] = (allocated_page->start_address & 0xfffffffff) << 11; // define new l3 page descriptor.
    }
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
    volatile struct pcb_t *ctask = core_tasks[cid];

    ctask->status = 3;       // set status to terminated.
    ctask->fault_code = 7;   // set fault code to i-abort fault.
    ctask->fault_dump = elr; // set fault dump to address of cause instruction.

    task_schaduler();
    task_dispatcher(); // ignore task and do new schaduling (because of saftey reasons).
}

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

void same_el_d_abort_same_handler()
{
    u64_t iss;
    u64_t far;

    asm volatile("mrs %0,ESR_EL1"
                 : "=r"(iss2)
                 :
                 :);

    asm volatile("mrs %0,FAR_EL1"
                 : "=r"(far)
                 :
                 :);

    iss &= 0xffffff << 31; // mask only iss.
    const u8_t dscf = iss & 0x1f;

    if (dscf > 0b010000 && dscf > 0b011000) // if it was synchronous external abort
    {
        const u8_t state = iss & 0b11 << 11;
        if (state == 0b11) // if external abort was restartable
            return;        // back from routine, this cause another exceution of tasks and retrying again.

        system_panic(); // else panic, because repairment routine is too heavy to execute.
    }
}