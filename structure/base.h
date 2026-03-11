#ifndef TYPE_BASE_H
#define TYPE_BASE_H

#define NULL ((void *)0) // Define NULL if not already defined
#define true ((void *)1) // Define true if not already defined
#define false 0          // Define false if not already defined
#define U64FILL (~((u64_t)0))
#define U32FILL (~((u32_t)0))

#define SCHADULING_QUANTUM_TIME 25 // by milliseconds.

#define PREIPH_MUART_FLAG 0b0000
#define PREIPH_UART0_FLAG 0b0001
#define PREIPH_UART2_FLAG 0b0010
#define PREIPH_UART3_FLAG 0b0011
#define PREIPH_UART4_FLAG 0b0100
#define PREIPH_UART5_FLAG 0b0101
#define PREIPH_TIMER_REQ_FLAG 0b0110
#define PREIPH_GPIO_FLAG 0b0111
#define PREIPH_SOFTLOCK_FLAG 0b1000
#define PREIPH_IPCMAILBOX_FLAG 0b1001

#define WAIT_MUART_TX 0
#define WAIT_MUART_RX 1
#define WAIT_MUART_FREE 2
#define WAIT_MUART_ALLOC 3
#define WAIT_TIMER_REQUEST 4
#define WAIT_TRUE_VALUE_ADDRESS 5
#define WAIT_FALSE_VALUE_ADDRESS 6
#define WAIT_IPC_MAILBOX_RECEIVE 7
#define WAIT_EVENT 8

#define AUX_BASE 0x7e215000
#define AUX_ENABLES_REG (AUX_BASE + 0x04)
#define AUX_IER_REG (AUX_BASE + 0x44)
#define AUX_IIR_REG (AUX_BASE + 0x48)
#define AUX_MU_IO_REG (AUX_BASE + 0x40)
#define AUX_MU_LSR_REG (AUX_BASE + 0x54)
#define AUX_MU_CNTL_REG (AUX_BASE + 0x60)
#define AUX_MU_STAT_REG (AUX_BASE + 0x64)
#define AUX_MU_BAUD_REG (AUX_BASE + 0x68)

#define MUART_DEFAULT_BAUD 9600
#define MUART_DEFAULT_DATABITS 1      // 8 bit mode.
#define MUART_DEFAULT_ENABLATION 0x1F // everything is enable (fifo, tx, rx, entire muart unit).

#define GPIO_BASE 0x7e200000
#define GPIO_FSEL0 (GPIO_BASE + 0x00)
#define GPIO_FSEL1 (GPIO_BASE + 0x04)
#define GPIO_FSEL2 (GPIO_BASE + 0x08)
#define GPIO_FSEL3 (GPIO_BASE + 0x0C)
#define GPIO_FSEL4 (GPIO_BASE + 0x10)
#define GPIO_FSEL5 (GPIO_BASE + 0x14)
#define GPIO_OUT_SET0 (GPIO_BASE + 0x1C)
#define GPIO_OUT_SET1 (GPIO_BASE + 0x20)
#define GPIO_OUT_CLR0 (GPIO_BASE + 0x24)
#define GPIO_OUT_CLR1 (GPIO_BASE + 0x28)

#define GPIO_INPUT 0b000  // input
#define GPIO_OUTPUT 0b001 // output
#define GPIO_MUART 0b010  // mini uart (alt 5)
#define GPIO_ALT2 0b110   // uart 0
#define GPIO_ALT3 0b111   // uart 0
#define GPIO_ALT4 0b011   // uart 2,3,4

#define UART0_BASE 0x7e201000
#define UART2_BASE 0x7e201400
#define UART3_BASE 0x7e201600
#define UART4_BASE 0x7e201800
#define UART5_BASE 0x7e201a00

#define UART_DR 0x00
#define UART_FR 0x18
#define UART_IBRD 0x24
#define UART_FBRD 0x28
#define UART_LCRH 0x2C
#define UART_CR 0x30
#define UART_IFLS 0x34
#define UART_IMSC 0x38
#define UART_RIS 0x3C
#define UART_MIS 0x40
#define UART_ICR 0x44
#define UART_DMACR 0x48
#define UART_ITCR 0x80
#define UART_ITIP 0x84
#define UART_ITOP 0x88
#define UART_TDR 0x8C

#define GICC_BASE 0xFF842000 // GIC CPU Interface

#define GICC_CTLR 0x0000
#define GICC_PMR 0x0004
#define GICC_BPR 0x0008
#define GICC_IAR 0x000C
#define GICC_EOIR 0x0010
#define GICC_RPR 0x0014
#define GICC_HPPIR 0x0018
#define GICC_ABPR 0x001C
#define GICC_AIAR 0x0020
#define GICC_AEOIR 0x0024
#define GICC_AHPPIR 0x0028
#define GICC_APR_BASE 0x00D0
#define GICC_NSAPR_BASE 0x00E0
#define GICC_IIDR 0x00FC
#define GICC_DIR 0x1000

#define GICD_BASE 0xFF841000 // GIC Distributor

#define GICD_CTLR 0x000
#define GICD_TYPER 0x004
#define GICD_IIDR 0x008
#define GICD_IGROUPR_BASE 0x080
#define GICD_ISENABLER_BASE 0x100
#define GICD_ICENABLER_BASE 0x180
#define GICD_ISPENDR_BASE 0x200
#define GICD_ICPENDR_BASE 0x280
#define GICD_ISACTIVER_BASE 0x300
#define GICD_ICACTIVER_BASE 0x380
#define GICD_IPRIORITYR_BASE 0x400
#define GICD_ITARGETSR_BASE 0x800
#define GICD_ICFGR_BASE 0xC00
#define GICD_NSACR_BASE 0xE00
#define GICD_SGIR 0xF00
#define GICD_CPENDSGIR_BASE 0xF10
#define GICD_SPENDSGIR_BASE 0xF20

#define GIC_SGI_MODE_DIRECT 0b00
#define GIC_SGI_MODE_BROADCASR 0b01
#define GIC_SGI_MODE_ME 0b10

#define SYSTEM_TIMER_BASE 0x7E003000
#define SYSTEM_TIMER_CS 0x00
#define SYSTEM_TIMER_CLO 0x04
#define SYSTEM_TIMER_CHI 0x08
#define SYSTEM_TIMER_C1 0x10

#define MMU_GRANUEL_SIZE_4KB 0b00
#define MMU_GRANUEL_SIZE_16KB 0b01
#define MMU_GRANUEL_SIZE_64KB 0b11

#define MMU_IPA_SIZE_30BITS 0b000
#define MMU_IPA_SIZE_48BITS 0b101

#define MMU_IORGN_NON_CACHABLE 0b00
#define MMU_IORGN_WB_WA 0b01
#define MMU_IORGN_WT 0b10
#define MMU_IORGN_WB 0b11

#define MMU_SH_NON_SHARABLE 0b00
#define MMU_SH_OUTER_SHARABLE 0b10
#define MMU_SH_INNER_SHARABLE 0b11

#define PSCI_CPU_SUSPEND_CORES_POWER_LEVEL 0b00
#define PSCI_CPU_SUSPEND_CLUSTERS_POWER_LEVEL 0b01
#define PSCI_CPU_SUSPEND_SYSTEM_POWER_LEVEL 0b10

#define PSCI_CPU_SUSPEND_STANDBY_STATETYPE 0b0
#define PSCI_CPU_SUSPEND_POWERDOWN_STATETYPE 0b1

#define SGI_WAKEUP_SERVICE_ID 7
#define SGI_SYSTEM_TERMINATION_ID 8

extern unsigned long __global_timer_ticks__;
extern unsigned long __pcb_bank_base__;
extern unsigned long __core_info_table__;
extern unsigned long __timer_request_bank_base__;
extern unsigned long __gpio_ownerships_bank_base__;
extern unsigned long __global_software_locks_bank_base__;

extern unsigned long __pcb_ready_queues_base__;
extern unsigned long __pcb_waiting_queue_base__;
extern unsigned long __pcb_terminated_queue_base__;
extern unsigned long __pcb_created_queue_base__;
extern unsigned long __timer_requests_queue_base__;
extern unsigned long __pcb_queue_lock_base__;
extern unsigned long __queues_temporary_base__;

extern unsigned long __user_region_start__;
extern unsigned long __global_ipcmailboxes_segments_bank_base__;
extern unsigned long __global_ipcmailbox_headers_bank_base__;

extern unsigned long __global_muart_settings__;
extern unsigned long __global_muart_metadata__;
extern unsigned long __global_muart_statistics__;

extern unsigned long __global_uart0_statistics__;
extern unsigned long __global_uart0_settings__;
extern unsigned long __global_uart0_metadata__;

extern unsigned long __global_uart2_statistics__;
extern unsigned long __global_uart2_settings__;
extern unsigned long __global_uart2_metadata__;

extern unsigned long __global_uart3_statistics__;
extern unsigned long __global_uart3_settings__;
extern unsigned long __global_uart3_metadata__;

extern unsigned long __global_uart4_statistics__;
extern unsigned long __global_uart4_settings__;
extern unsigned long __global_uart4_metadata__;

extern unsigned long __global_uart5_statistics__;
extern unsigned long __global_uart5_settings__;
extern unsigned long __global_uart5_metadata__;

extern unsigned long __global_gic400_metadata__;

extern unsigned long __generic_base_irq_statistics__;

extern unsigned long __system_panic_log__;

extern unsigned long __system_debug_log__;

extern unsigned long __generic_base_schaduler_statistics__;

extern unsigned long __global_tasks_dump_bank_base__;

extern unsigned long __generic_base_system_exception_statistics__;

extern unsigned long __system_memory_frame_bank_base__;
extern unsigned long __memory_paging_settings_base__;

extern unsigned long __cccb_bank_base__;

typedef unsigned long u64_t;
typedef signed long s64_t;

typedef unsigned int u32_t;
typedef signed int s32_t;

typedef unsigned short u16_t;
typedef signed short s16_t;

typedef unsigned char u8_t;
typedef signed char s8_t;
#endif