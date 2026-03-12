#ifndef STDLIB_STRUCTURE_H
#define STDLIB_STRUCTURE_H
#include "./base.h"

typedef u64_t semaphore_t;
typedef u64_t mutex_t; // for more simplicy in usage and least cycles.

struct slcb_t
{
    u64_t owner_task;  // id of create by which task.
    u64_t gained_task; // id of gained by which task.
    u64_t *lock;
    u16_t id;       // unique identifier.
    u8_t type;      // 0: mutex, 1: semaphore.
    u32_t reserved; // padding (4 Bytes).
}; // software lock control block;

struct cccb_t // Core Context Control Block. (96 Bytes)
{
    u64_t valid;
    // mmu

    u64_t ttbr0;
    u64_t ttbr1;
    u64_t tcr;

    // system & exception

    u64_t sctlr;
    u64_t far;
    u64_t spsr;

    // Genric timer register.

    u64_t cntp_ctl;
    u64_t cntfrq;
    u64_t cntp_tval;
    u64_t cntpct;
};

struct system_breakpoint_t // 152 Bytes.
{
    u64_t pc;     // points to where the exception happens.
    u64_t spsr;   // saved PSTATE fields.
    u64_t time;   // value of timer.
    u8_t sp[128]; // last 128 bytes of kernel stack.
};

struct task_dump_t // 56 Bytes.
{
    u64_t task_id;           // id of dumped task.
    u64_t preipherals;       // allocated preipherals by task.
    u64_t preipherals_count; // count of allocated preipherals by task.
    u64_t fault_code;        // fault code.
    u64_t fault_dump;        // fault dump (description).
    u64_t flags;             // flags of task.
    u64_t stime;             // value of timer (time of dump).
};

struct system_exceptions_statistics_t
{
    u64_t softwaresteps;                 // count of software step exceptions.
    u64_t breakpoints;                   // count of breakpoint exceptions.
    u64_t kernel_wfi_wfe_counts;         // count of wfi/wfe exceptions from EL1.
    u64_t kernel_stack_alignment_faults; // count of stack alignment fault exceptions from EL1.
};

struct gic400_metadata_t // information of gic-400 interrupt controller (this struct is read-only after discovery process).
{
    u16_t implementer;          // general info.
    u8_t interrup_lines_number; // this used to calculate number of registers and bits.
    u8_t cpu_interface_number;  // number of cpu interface detected + virtual cpu interface detected.
    u8_t security_extension;    // set to 1 as enabled, set to 0 as disabled.
    u8_t lspi_count;            // count of locked-shared-preipheral-interrupt.
    u8_t revision;              // general info.
    u8_t varaint;               // general info.
    u8_t product_id;            // general info.
    u8_t reserved[15];          // padding (15 Bytes).
};

struct gpio_ownership_t // 16 Bytes.
{
    u64_t task_id;
    u64_t pin_number;
};

struct ipcmailbox_t // 56 Bytes.
{
    u64_t task_owner;
    u64_t id;
    mutex_t access_mutex;         // a mutex, where task wants to write.
    u64_t blacklist_tasks_pt1_id; // this list cant access (if isnt NULL).
    u64_t blacklist_tasks_pt2_id; // this list cant access (if isnt NULL).
    u64_t whitelist_tasks_pt1_id; // this list can access only (if isnt NULL).
    u64_t whitelist_tasks_pt2_id; // this list can access only (if isnt NULL).
    u64_t accessibility;          // 0: write access, 1: read access, 2: management, 3-63: reserved.
    u32_t maximum_length;
    u32_t metadata; // 0-1: 0->universal, 1->oneline, 2->dual, 2-3: (0 -> empty, 1 -> filling, 2 -> fill, 3 -> reading), 4-31: reserved.
};

struct ipcmailbox_settings_t // settings for only function call.
{
    u64_t task_owner;
    u64_t blacklist_tasks_pt1_id; // this list cant access (if isnt NULL).
    u64_t blacklist_tasks_pt2_id; // this list cant access (if isnt NULL).
    u64_t whitelist_tasks_pt1_id; // this list can access only (if isnt NULL).
    u64_t whitelist_tasks_pt2_id; // this list can access only (if isnt NULL).
    u64_t accessibility;          // 0: write access, 1: read access, 2: management, 3-63: reserved.
    u32_t maximum_length;
    u32_t metadata; // 0-1: 0->universal, 1->oneline, 2->dual, 2-3: (0 -> empty, 1 -> filling, 2 -> fill, 3 -> reading), 4-31: reserved.
};

struct ipcmailbox_message_t
{
    u64_t author_task_id;   // 0 for host.
    u64_t receiver_task_id; // 0 for anyone.
    u64_t content_pt1;      // content first 8 Bytes.
    u64_t content_pt2;      // content second 8 Bytes. (if content size is 16 bytes)
    u64_t done;             // set this 1 if is done.
};

struct ipcmailbox_segment_t // 64 Bytes.
{
    mutex_t access_mutex; // a mutex, where task wants to write/read.
    u64_t mailbox_id;
    u32_t mailbox_type;                  // 0: universal, 1: oneline, 2: dual, 3: raw.
    u32_t status;                        // 0: empty, 1: filling, 2: full, 3: reading. this is just for general information about mutex.
    struct ipcmailbox_message_t context; // usable context.
};

struct irq_statistic_t
{
    u64_t oop_irqs_count; // out of prediction interrupt counts.
    u64_t oop_sgis_count; // out of prediction software generated interrupt counts.
};

struct mmu_settings_t // 16 Bytes
{
    u8_t translation_granuel_size_user;
    u8_t translation_granuel_size_kernel;
    u8_t intermidate_physical_address_size;
    u8_t inner_cachablity_user;
    u8_t inner_cachablity_kernel;
    u8_t outer_cachablity_user;
    u8_t outer_cachablity_kernel;
    u8_t sharablity_user;
    u8_t sharablity_kernel;
    u8_t ttrnb_user;   // all 1 or 0.
    u8_t ttrnb_kernel; // all 1 or 0.
    u8_t reserved[5];  // 5 Bytes padding.
};

struct memframe_t // 32 Bytes.
{
    u64_t owner_task_id;
    u64_t start_address;
    u64_t next_frame;
    u32_t frame_id;
    u8_t size;            // 00: 4KB, 01: 16KB, 10: 64KB.
    u8_t lower_attribute; // Index, NS, AP, SH, AF
    u8_t upper_attribute; // PXN, UXN, <Reserved for Software usage>.
    u8_t reserved;        // padding;
};

struct memory_paging_settings_t // 8 Bytes.
{
    mutex_t access_mutex;    // access mutex, when any kind of access like trying to reserve page, remove page or anything else, mutex must be gain.
    u32_t pages_count;       // total count of pages.
    u8_t page_sizing;        // 00: 4KB, 01: 16KB, 10: 64KB.
    u8_t initial_pages;      // much frames should be reserved at first attempt of task (when created for first time).
    u8_t eviction_threshold; // when memory become less than threshold, eviction happens, means random tasks by priority gonna be evicted, formula of output is (threshold*128 KB)+256 KB.
    u8_t reserved;           // padding.
};

struct memframes_header_t // 16 Bytes.
{
    struct memframe_t *head;
    struct memframe_t *tail;
};

struct
    muart_settings_t // 8 bytes.
{
    u16_t baudrate;
    u8_t data_bits;
    u8_t enablation;  // bits --> 0: enable whole uart, 1: tx enable, 2: rx enable, 3: tx irq enable, 4: rx irq enable, 5-7: padding.
    u8_t reserved[4]; // padding.
};

struct muart_metadata_t // 54 bytes.
{
    struct muart_settings_t *settings; // reminder: this is a pointer.
    u64_t owner_task;
    u8_t *write_buffer; // this is a pointer.
    u64_t write_length;
    u64_t written_length;
    u8_t *read_buffer; // this is a pointer.
    u64_t read_maximum_length;
    u64_t read_length;
    u64_t timeout;
    mutex_t access_mutex; // this used for every kind of access to this metadata (when checking for status of muart).
    u64_t delimiter;      // a byte with padding.
};

struct muart_statistics_t
{
    u64_t receiver_overruns; // times of receiver overrun happens.
    u64_t wt_rejections;     // if buffer/write length isnt valid.
    u64_t rd_rejections;     // if buffer/read maximum length isnt valid.
};

struct system_panic_log_t // 120 Bytes.
{
    u64_t syndrome;     // ESR_EL1
    u64_t elr;          // ELR_EL1
    u64_t fp;           // X29
    u64_t lr;           // X30
    u64_t sp;           // Stack Pointer.
    u64_t spsr;         // SPSR_EL1
    u64_t far;          // FAR_EL1
    u64_t mpidr;        // MPIDR_EL1
    u64_t sctlr;        // SCTLR_EL1
    u64_t daif;         // daif register.
    u64_t gic400_hppir; // GICC_HPPIR
    u64_t gic400_iar;   // GICC_IAR
    u64_t stimer;       // value of system timer.
    u8_t stack[128];    // last 128 bytes of kernel stack
};

typedef struct schaduler_statistics_t
{
    u64_t terminate_counts;
    u64_t void_terminate_counts;
    u64_t tasks_count;
};

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
    u64_t priority;                           // 0~7: 0 is most and 7 is least.
    volatile struct memframes_header_t pages; // pointer to memory pages.
    u64_t fault_code;                         // 0: pc alignment fault, 1: stack alignment fault, 3: simd/fp fault, 4: security breach (wfi/wfe), 5: instruction abort, 6: data abort, 7: i-abort, 8: d-abort, 9: unkown instruction, 10: forbidden access to mini-uart (via waiting), 11: invalid ipc mailbox id (via waiting), 12: invalid access to ipc mailbox (via waiting).
    u64_t fault_dump;
    u64_t preipherals; // using this to free all allocated preipherals by this task when this task is terminating. --> each 4 bits is a preipheral id. (0000 --> mini uart, 0001 --> uart-0, 0010 --> uart-2, 0011 --> uart-3, 0100 --> uart-4, 0101 --> uart-5, 0110 --> timer request, 0111 --> gpio ownership, 1000 --> software lock gained, 1001 --> ipc mailbox owned)
    u64_t preipherals_count;
    volatile struct pcb_t *parent; // if is null, itself is parent.
    u64_t pc;                      // program counter.
    u64_t ttbr;                    // TTBR.
    volatile struct pcb_t *next;
    u64_t events_handler;   // for address for 4 handlers (for event betwen 1~3 and 0 is only kernel access).
    u64_t wait_instruction; // something like descritpion for wait statement.
    u8_t wait_reason;       // 0: waiting for mini-uart (tx), 1: waiting for mini-uart (rx), 2: waiting for mini-uart to free, 3: waiting for mini-uart to allocate, 4: waiting for timer requests, 5: waiting for true value of address, 6: waiting for false value of address, 7: waiting for mailbox (receive), 8: waiting for event.
    u8_t flags;             // 0-1: dedicated core, 2: ready flag, 3: core migration enable, 4: accessed before, 5: core dependency.
    u8_t event_number;      // 0: no event, 1: termination (force), 2: termination (soft), 3: pause, 4: resume.
    u8_t valid;             // if is set, it has a context, else is free.
    u8_t reserved[4];       // padding.
};

struct timer_request_t
{
    u64_t task_id;
    u64_t wake_ticks;
    volatile struct timer_request_t *next; // for forward list.
};

struct uart_statistics_t // 24 Bytes.
{
    u64_t overrun_errors;
    u64_t parity_errors;
    u64_t break_errors;
};

struct uart_settings_t // 48 Bytes.
{
    u64_t tx_gptable;
    u64_t rx_gptable;
    u64_t cts_gptable;
    u64_t rts_gptable;
    u16_t baudrate;

    u8_t tx_gpalt;
    u8_t rx_gpalt;
    u8_t cts_gpalt;
    u8_t rts_gpalt;

    u8_t tx_gpnth;
    u8_t rx_gpnth;
    u8_t rts_gpnth;
    u8_t cts_gpnth;

    u8_t communication_settings; // 0: parity enable, 1: Even parity select, 2: two stop bits select, 3: sticky parity select, 4: send break (communication is always remain connected), 5-7: reserved.
    u8_t word_length;            // (0 -> 5, 1 -> 6, 2 -> 7, 3 -> 8) bits word length.
    u8_t enable_fifos;           // set to 1 to enable fifos (rx & tx).
    u8_t threshold;              // 0-2: tx fifo threshold, 3-5: rx fifo threshold, 6-7: reserved.
    u8_t enable_unit;            // set to one for enabling current uart unit.
    u8_t reserved;               // padding (1 Byte)
};

struct uart_metadata_t // 54 Bytes.
{
    struct uart_settings_t *settings;
    u64_t owner_task;
    u8_t *write_buffer;
    u64_t write_length;
    u64_t written_length;
    u8_t *read_buffer;
    u64_t read_maximum_length;
    u64_t read_length;
    u64_t timeout;
    mutex_t access_mutex; // this used for every kind of access to this struct.
    u8_t *delimiter;
};

#endif