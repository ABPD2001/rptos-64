#include "./lib/core.h"
#include "./lib/mmu.h"
#include "./lib/memory.h"
#include "./lib/math.h"
#include "./lib/schaduler.h"

#include "./structure/core.h"
#include "./structure/dump.h"
#include "./structure/gic400.h"
#include "./structure/gpio.h"
#include "./structure/ipcmailbox.h"
#include "./structure/irq.h"
#include "./structure/mmu.h"
#include "./structure/muart.h"
#include "./structure/panic.h"
#include "./structure/task.h"

#include "./drivers/gic400.h"
#include "./drivers/gtimer.h"
#include "./drivers/muart.h"

volatile struct pcb_t *global_pcb_bank = NULL;             // limit of 128 tasks.
volatile struct gpio_ownership_t *global_gpio_bank = NULL; // limit of 64 ownerships.
volatile struct timer_request_t *global_timer_requests_bank = NULL;
volatile struct slcb_t *global_software_locks_bank = NULL; // limit of 128 software locks.

volatile struct ipcmailbox_t *global_ipcmailbox_bank = NULL;                  // limit of 64 headers.
volatile struct ipcmailbox_segment_t *global_ipcmailbox_segments_bank = NULL; // limit of 1024 segments.

volatile u64_t *global_system_ticks = NULL;
volatile struct muart_settings_t *global_mini_uart_settings = NULL;
volatile struct muart_metadata_t *global_mini_uart_metadata = NULL;
volatile struct muart_statistics_t *global_mini_uart_statistics = NULL;

volatile struct task_dump_t *global_tasks_dump_bank = NULL;

volatile struct irq_statistic_t *generic_irq_statistics_base = NULL;

volatile struct system_exceptions_statistics_t *generic_system_exception_statistics_base = NULL;

volatile struct system_breakpoint_t *generic_system_breakpoints_base = NULL;

volatile struct gic400_metadata_t *global_gic400_metadata = NULL;

volatile struct system_panic_log_t *system_panic_log = NULL;

volatile struct fwlist_header_t *created_queue = NULL;
volatile struct fwlist_header_t *pri0_ready_queue = NULL;
volatile struct fwlist_header_t *pri1_ready_queue = NULL;
volatile struct fwlist_header_t *pri2_ready_queue = NULL;
volatile struct fwlist_header_t *pri3_ready_queue = NULL;
volatile struct fwlist_header_t *pri4_ready_queue = NULL;
volatile struct fwlist_header_t *pri5_ready_queue = NULL;
volatile struct fwlist_header_t *pri6_ready_queue = NULL;
volatile struct fwlist_header_t *pri7_ready_queue = NULL;
volatile struct fwlist_header_t *waiting_queue = NULL;
volatile struct fwlist_header_t *terminated_queue = NULL;

volatile struct mmu_asid_t **mmu_asids = NULL;

volatile struct tfwlist_header_t *global_timer_requests_queue = NULL;

volatile u64_t **core_tasks = NULL;

volatile struct cccb_t **core_contexts = NULL;

volatile struct memframe_t *memory_frames = NULL;
volatile struct memory_paging_settings_t *memory_paging_settings = NULL;

volatile struct kmem_page_t *kernel_pages = NULL;
volatile struct kmem_frame_t *kernel_frames = NULL;

volatile u32_t *pri_map = NULL;   // 0-7: core 0 current executing priority, 8-15: core 1, 16-23: core 2, 24-31: core 3.
volatile u32_t *sch_ticks = NULL; // 0-7: core 0 schaduling ticks, 8-15: core 1, 16-23: core 2, 24-31: core 3.

volatile mutex_t *queues_lock = NULL;
volatile mutex_t *schaduling_lock = NULL;
volatile mutex_t *kmem_lock = NULL;
volatile mutex_t **asid_banks_lock = NULL;

void kernel()
{
    const u8_t cid = core_id();

    // first, we set address of global pointers (.bss variables)
    global_system_ticks = __global_timer_ticks__;
    global_pcb_bank = __pcb_bank_base__;                      // reminder: it has limit of 128 tasks.
    global_timer_requests_bank = __timer_request_bank_base__; // reminder: it has limit of 128 requests.
    global_mini_uart_settings = __global_muart_settings__;
    global_mini_uart_metadata = __global_muart_metadata__;
    global_gpio_bank = __gpio_ownerships_bank_base__;                 // reminder: it has limit of 64 ownerships.
    global_software_locks_bank = __global_software_locks_bank_base__; // reminder: it has limit of 256 software locks.
    global_ipcmailbox_bank = __global_ipcmailbox_headers_bank_base__;
    global_ipcmailbox_segments_bank = __global_ipcmailboxes_segments_bank_base__;
    global_gic400_metadata = __global_gic400_metadata__;
    global_mini_uart_statistics = __global_muart_statistics__;
    system_panic_log = __system_panic_log__;
    global_tasks_dump_bank = __global_tasks_dump_bank_base__;
    generic_system_exception_statistics_base = __generic_base_system_exception_statistics__;
    generic_system_breakpoints_base = __system_debug_log__;
    core_contexts = __pcb_bank_base__;
    kernel_pages = __global_kernel_pages_bank_base__;
    kernel_frames = __global_kernel_frames_bank_base__;
    kmem_lock = __kernel_memory_lock__;
    mmu_asids = __cores_asid_bank_base__;
    asid_banks_lock = __cores_asid_bank_locks_base__;
    core_tasks = __core_info_table__ + 32;

    // initialize pcb queues.

    created_queue = __pcb_created_queue_base__;
    pri0_ready_queue = __pcb_ready_queues_base__;
    pri1_ready_queue = __pcb_ready_queues_base__ + 16;
    pri2_ready_queue = __pcb_ready_queues_base__ + 32;
    pri3_ready_queue = __pcb_ready_queues_base__ + 48;
    pri4_ready_queue = __pcb_ready_queues_base__ + 64;
    pri5_ready_queue = __pcb_ready_queues_base__ + 80;
    pri6_ready_queue = __pcb_ready_queues_base__ + 96;
    pri7_ready_queue = __pcb_ready_queues_base__ + 112;
    waiting_queue = __pcb_waiting_queue_base__;
    terminated_queue = __pcb_terminated_queue_base__;

    global_timer_requests_queue = __timer_requests_queue_base__;
    memory_frames = __system_memory_frame_bank_base__; // set frame bank base.
    memory_paging_settings = __memory_paging_settings_base__;

    pri_map = __queues_temporary_base__;
    *pri_map = 0; // set highest priority at first. (0 is most and 7 is least).
    sch_ticks = __queues_temporary_base__ + 4;
    *sch_ticks = 0; // just in case... (to prevent from unkown behavior).

    if (core_contexts[cid]->valid)                // if context was valid.
        restore_core_context(core_contexts[cid]); // restore core context.
    else                                          // else initialize.
    {

        turn_on_gtimer(); // turn on generic timer of current core.

        // then, if core id was zero, enabling multi-core mode and waiting until all cores acknowledged core zero.
        if (cid)
        {
            volatile u64_t *cores_signal = (__core_info_table__ + 64); // set pointer to counts.
            *cores_signal &= ~(0xFF << cid * 8);                       // clear current core event number.
            *cores_signal |= (0x1 << cid * 8);                         // set current core event number.
        }
        if (!cid)
        {
            initialize_muart();                                                                             // initialize mini-uart.
            gic400_setGrp1_distributor(true);                                                               // enable group 1 interrupt.
            multi_core_enable();                                                                            // wake up other cores.
            volatile u64_t *cores_signal = (__core_info_table__ + 64);                                      // set pointer to counts.
            while (1)                                                                                       // wait until all cores are ready.
                if ((*cores_signal & 0xFF00) && (*cores_signal & 0xFF0000) && (*cores_signal & 0xFF000000)) // if first and second and third bytes was signaled.
                    break;

            char buffer[16] = "rptos-64 is up.";
            buffer[15] = '\0';
            muart_write(buffer, 16);
        }

        // configure system control register.
        u32_t sctlr;
        asm volatile(
            "mrs %0,SCTLR_EL1"
            : "=r"(sctlr)
            :
            :);

        sctlr = 0x300007A; // set Alignment check, stack alignment check, c15 barrier, Endiannmass of data access in EL0, Exception endiannmass.

        // memory paging configuration and initialization.
        if (!cid)
        {
            memory_paging_settings->initial_pages = 1;      // set initial pages count to 1.
            memory_paging_settings->page_sizing = 0;        // set page sizing to 4KB.
            memory_paging_settings->eviction_threshold = 6; // set eviction threshold to 1 MB.

            const u32_t pages_numeric_size = (!memory_paging_settings->page_sizing ? 4096 : memory_paging_settings->page_sizing == 1 ? 16384
                                                                                                                                     : 65536);
            const u8_t page_size = memory_paging_settings->page_sizing;

            memory_paging_settings->pages_count = (4 * GB - __user_region_start__) / pages_numeric_size; // calculate count of pages by page size.

            volatile struct memframe_t *frame = memory_frames;
            u64_t raw_address = __user_region_start__;
            const u32_t pages_count = memory_paging_settings->pages_count;

            for (u64_t i = 0; i < pages_count; i++)
            {
                memory_frames->frame_id = i;                // set index (id).
                memory_frames->owner_task_id = 0;           // clear owner task.
                memory_frames->size = page_size;            // set size.
                memory_frames->start_address = raw_address; // set raw address.
                raw_address += pages_numeric_size;          // increment to next frame.
            }
        }
    }

    // initialize pcb blocks.

    for (u64_t i = 0; i < 128; i++)
    {
        global_pcb_bank[i].id = i;    // set id.
        global_pcb_bank[i].valid = 0; // invalidate.
        global_pcb_bank[i].events_handler = NULL;
        global_pcb_bank[i].event_number = 0;
        global_pcb_bank[i].fault_code = 0;
        global_pcb_bank[i].fault_dump = 0;
        global_pcb_bank[i].flags = 0;
        global_pcb_bank[i].next = NULL;
        global_pcb_bank[i].pages.head = NULL;
        global_pcb_bank[i].pages.tail = NULL;
        global_pcb_bank[i].parent = NULL;
        global_pcb_bank[i].pc = 0;
        global_pcb_bank[i].perimision_level = 0;
        global_pcb_bank[i].preipherals = 0;
        global_pcb_bank[i].preipherals_count = 0;
        global_pcb_bank[i].priority = 7; // lowest prioriy at initialize.
        global_pcb_bank[i].wait_instruction = 0;
        global_pcb_bank[i].wait_reason = 0;
        global_pcb_bank[i].ttbr = 0;

        // others will be changes at task schaduling or creation.
    }

    // initialize kernel memory system frames.
    u64_t kmem_counter_temp = 0; // temporary counter to determine current address.

    for (u64_t i = 0; i < 93; i++)
    {
        kernel_pages[i].id = i;                             // set id for page.
        kernel_frames[i].start_address = kmem_counter_temp; // set start address.

        if (i > 28) // if it was 2KB frame.
        {
            kernel_frames[i].size = KMEM_SIZE_ORG_2KB;
            kmem_counter_temp += 2 * KB; // increment to 2 KB.
        }
        else if (i > 13)
        { // if it was 8KB frame.
            kernel_frames[i].size = KMEM_SIZE_ORG_8KB;
            kmem_counter_temp += 8 * KB; // increment to 8 KB.
        }
        else if (i > 5)
        { // if it was 32KB frame.
            kernel_frames[i].size = KMEM_SIZE_ORG_32KB;
            kmem_counter_temp += 32 * KB; // increment to 32 KB.
        }
        else if (i > 1)
        { // if it was 128KB frame.
            kernel_frames[i].size = KMEM_SIZE_ORG_128KB;
            kmem_counter_temp += 128 * KB; // increment to 128 KB.
        }
        else if (!i)
        { // if it was 512KB frame.
            kernel_frames[i].size = KMEM_SIZE_ORG_512KB;
            kmem_counter_temp += 512 * KB; // increment to 512 KB.
        }
    }

    // <--- initialize pre-build services --->

    struct task_properties_t props_serial;
    struct task_properties_t props_power;

    props_power.priority = 3; // middle priority at first.
    props_power.core_dependency = true;
    props_power.core_migration_enable = false;
    props_power.ready_flag = true;
    props_power.events_handler = NULL;

    props_serial.priority = 3; // middle priority at first.
    props_serial.core_dependency = true;
    props_serial.core_migration_enable = false;
    props_serial.ready_flag = true;
    props_serial.events_handler = NULL;

    volatile struct pcb_t *power_service = create_ktask(props_power);
    volatile struct pcb_t *serial_service = create_ktask(props_serial);

    power_service->perimision_level = 2;               // most kernel access for power service.
    power_service->pc = __kernel_service_power_base__; // set program counter.

    serial_service->perimision_level = 0;                // user access for serial service.
    serial_service->pc = __kernel_service_serial_base__; // set program counter.

    volatile struct memframe_t *power_frame = alloc_page(power_service->id);
    volatile struct memframe_t *serial_frame = alloc_page(serial_service->id);

    mh_push_back(serial_service->pages.head, serial_service->pages.tail, serial_frame); // allocate only one page.
    mh_push_back(power_service->pages.head, power_service->pages.tail, power_frame);    // allocate only one page.

    // power virtualization
    volatile u64_t *power_service_vir = __kernel_sevices_virtual_maps__;
    volatile struct mmu_asid_t *power_service_asid = allocate_asid(power_service->id);
    u64_t ttbr;

    *power_service_vir = ((__kernel_sevices_virtual_maps__ + 1) & 0xfffffffff) << 11;       // set l0 table.
    *(power_service_vir + 1) = ((__kernel_sevices_virtual_maps__ + 2) & 0xfffffffff) << 11; // set l1 table.
    *(power_service_vir + 2) = ((__kernel_sevices_virtual_maps__ + 3) & 0xfffffffff) << 11; // set l2 table.
    *(power_service_vir + 3) = (power_frame->start_address & 0xfffffffff) << 11;            // set l3 table.
    set_ttbr0(power_service, power_service_asid->asid, 0, false);                           // set ttbr of power serivce.
    asm volatile("mrs %0,TTBR0_EL1"
                 : "=r"(ttbr)
                 :
                 :);
    power_service->ttbr = ttbr;

    // serial virtualization
    volatile u64_t *serial_service_vir = __kernel_sevices_virtual_maps__ + 4;
    volatile struct mmu_asid_t *serial_service_asid = allocate_asid(serial_service->id);
    *serial_service_vir = ((__kernel_sevices_virtual_maps__ + 5) & 0xfffffffff) << 11;       // set l0 table.
    *(serial_service_vir + 5) = ((__kernel_sevices_virtual_maps__ + 6) & 0xfffffffff) << 11; // set l1 table.
    *(serial_service_vir + 6) = ((__kernel_sevices_virtual_maps__ + 7) & 0xfffffffff) << 11; // set l2 table.
    *(serial_service_vir + 7) = (serial_frame->start_address & 0xfffffffff) << 11;           // set l3 table.
    set_ttbr0(serial_service, serial_service_asid->asid, 0, false);                          // set ttbr of power serivce.
    asm volatile("mrs %0,TTBR0_EL1"
                 : "=r"(ttbr)
                 :
                 :);
    serial_service->ttbr = ttbr;

    // push into queue.
    fw_push_back(pri3_ready_queue, power_service);  // push into queue.
    fw_push_back(pri3_ready_queue, serial_service); // push into queue.

    // <--- at last, configure gic-400 --->
    gic400_interfacectl(true, true); // enable EOIModeNS and Group 1.
    gic400_priorityirq(125, 0x80);   // AUX
    gic400_priorityirq(89, 0x90);    // UART
    gic400_priorityirq(30, 0xA0);    // generic timer
    gic400_priorityirq(97, 0xB0);    // system timer
    enable_daif();                   // enable IRQ, FIQ, SError, Debug

    // <--- run a task --->
    task_schaduler();  // schadule.
    task_dispatcher(); // dispatch.
}