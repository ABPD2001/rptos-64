#ifndef TYPES_GIC400_H
#define TYPES_GIC400_H
#include "./base.h"

// 16 Bytes.
typedef struct gic400_metadata_t // information of gic-400 interrupt controller (this struct is read-only after discovery process).
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

#endif