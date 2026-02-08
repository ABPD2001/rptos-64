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

extern unsigned long __global_timer_ticks__;
extern unsigned long __global_muart_settings__;
extern unsigned long __global_muart_metadata__;
extern unsigned long __pcb_bank_base__;
extern unsigned long __pcb_queue_base__;
extern unsigned long __core_info_table__;

typedef unsigned long u64_t;
typedef signed long s64_t;

typedef unsigned int u32_t;
typedef signed int s32_t;

typedef unsigned short u16_t;
typedef signed short s16_t;

typedef unsigned char u8_t;
typedef signed char s8_t;
#endif