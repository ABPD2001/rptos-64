#ifndef TYPE_BASE_H
#define TYPE_BASE_H

#define NULL ((void *)0)  // Define NULL if not already defined
#define true ((void *)1)  // Define true if not already defined
#define false ((void *)0) // Define false if not already defined

#define CORES_RUNNING_TASK_BASE
#define MUART_METADATA_BASE

extern unsigned long __core_stack_table__;

typedef unsigned long u64_t;
typedef signed long s64_t;

typedef unsigned int u32_t;
typedef signed int s32_t;

typedef unsigned short u16_t;
typedef signed short s16_t;

typedef unsigned char u8_t;
typedef signed char s8_t;
#endif