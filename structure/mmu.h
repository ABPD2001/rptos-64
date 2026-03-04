#ifndef TYPES_MMU_H
#define TYPES_MMU_H
#include "./base.h"

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

#endif