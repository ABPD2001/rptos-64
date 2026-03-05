#ifndef TYPES_MMU_H
#define TYPES_MMU_H
#include "./base.h"
#include "./softwarelock.h"

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

// if using 917,504 frame (4KB frame) as 3.5 GB usage, total size of frames is 28 MB.
// (512MB - 28MB) = 484 MB on this case is reserved for kernel.

#endif