#ifndef TYPE_BASE_H
#define TYPE_BASE_H

#define NULL ((void *)0)  // Define NULL if not already defined
#define true ((void *)1)  // Define true if not already defined
#define false ((void *)0) // Define false if not already defined

#define AUX_BASE 0x7e215000
#define AUX_ENABLES_REG (AUX_BASE + 0x04)
#define AUX_IER_REG (AUX_BASE + 0x44)
#define AUX_MU_IO_REG (AUX_BASE + 0x40)
#define AUX_MU_LSR_REG (AUX_BASE + 0x54)
#define AUX_MU_CNTL_REG (AUX_BASE + 0x60)
#define AUX_MU_BAUD_REG (AUX_BASE + 0x68)

#define MUART_DEFAULT_BAUD 9600
#define MUART_DEFAULT_DATABITS 1      // 8 bit mode.
#define MUART_DEFAULT_ENABLATION 0x1F // 8 bit mode.

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

#define GPIO_INPUT 0b000
#define GPIO_OUTPUT 0b001
#define GPIO_UART 0b010

extern unsigned long __global_timer_ticks__;
extern unsigned long __global_muart_settings__;
extern unsigned long __global_muart_metadata__;
extern unsigned long __global_muart_statistics__;
extern unsigned long __pcb_bank_base__;
extern unsigned long __pcb_queue_base__;
extern unsigned long __core_info_table__;
extern unsigned long __timer_request_bank_base__;

typedef unsigned long u64_t;
typedef signed long s64_t;

typedef unsigned int u32_t;
typedef signed int s32_t;

typedef unsigned short u16_t;
typedef signed short s16_t;

typedef unsigned char u8_t;
typedef signed char s8_t;
#endif