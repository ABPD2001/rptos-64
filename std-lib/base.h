#ifndef STDLIB_BASE_H
#define STDLIB_BASE_H

#define NULL ((void *)0) // Define NULL if not already defined
#define true ((void *)1) // Define true if not already defined
#define false 0          // Define false if not already defined
#define U64FILL (~((u64_t)0))
#define U32FILL (~((u32_t)0))

#define WAIT_MUART_TX 1
#define WAIT_MUART_RX 2
#define WAIT_MUART_FREE 3
#define WAIT_MUART_ALLOC 4
#define WAIT_TIMER_REQUEST 5
#define WAIT_TRUE_VALUE_ADDRESS 6
#define WAIT_FALSE_VALUE_ADDRESS 7
#define WAIT_IPC_MAILBOX_RECEIVE 8
#define WAIT_EVENT 9

typedef unsigned long u64_t;
typedef signed long s64_t;

typedef unsigned int u32_t;
typedef signed int s32_t;

typedef unsigned short u16_t;
typedef signed short s16_t;

typedef unsigned char u8_t;
typedef signed char s8_t;
#endif