#ifndef TYPES_EXTERN_H
#define TYPES_EXTERN_H
#include "./base.h"

extern volatile struct pcb_t *global_pcb_bank;             // limit of 64 tasks.
extern volatile struct gpio_ownership_t *global_gpio_bank; // limit of 64 ownerships.
extern volatile struct timer_request_t *global_timer_requests_bank;
extern volatile struct slcb_t *global_software_locks_bank; // limit of 128 software locks.

extern volatile struct ipcmailbox_t *global_ipcmailbox_bank;                  // limit of 64 headers.
extern volatile struct ipcmailbox_segment_t *global_ipcmailbox_segments_bank; // limit of 512 segments.

extern volatile u64_t *global_system_ticks;
extern volatile struct muart_settings_t *global_mini_uart_settings;
extern volatile struct muart_metadata_t *global_mini_uart_metadata;
extern volatile struct muart_statistics_t *global_mini_uart_statistics;

extern volatile struct irq_statistic_t *generic_irq_statistics_base;

extern volatile struct gic400_metadata_t *global_gic400_metadata;

extern volatile struct system_panic_log_t *system_panic_log;

extern volatile struct system_exceptions_statistics_t *generic_system_exception_statistics_base;

extern volatile struct fwlist_header_t **created_queues;
extern volatile struct fwlist_header_t **pri0_ready_queues;
extern volatile struct fwlist_header_t **pri1_ready_queues;
extern volatile struct fwlist_header_t **pri2_ready_queues;
extern volatile struct fwlist_header_t **pri3_ready_queues;
extern volatile struct fwlist_header_t **pri4_ready_queues;
extern volatile struct fwlist_header_t **pri5_ready_queues;
extern volatile struct fwlist_header_t **pri6_ready_queues;
extern volatile struct fwlist_header_t **pri7_ready_queues;
extern volatile struct fwlist_header_t **waiting_queues;
extern volatile struct fwlist_header_t **terminated_queues;
extern volatile struct fwlist_header_t **sleeping_queues;

extern volatile struct tfwlist_header_t **timer_requestes_queues;

extern volatile struct task_dump_t *global_tasks_dump_bank;

extern volatile struct system_breakpoint_t *generic_system_breakpoints_base;

extern volatile u64_t **core_tasks;
extern volatile struct cccb_t **core_contexts;

extern volatile struct memframe_t *memory_frames;
extern volatile struct memory_paging_settings_t *memory_paging_settings;

extern volatile u32_t *pri_map;
extern volatile u32_t *sch_ticks;
#endif