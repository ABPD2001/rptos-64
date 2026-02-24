#ifndef GIC400_H
#define GIC400_H
#include "../structure/base.h"
#include "../structure/gic400.h"
#include "../lib/math.h"

extern volatile struct gic400_metadata_t *global_gic400_metadata;

// note: ID 30 is for generic timer (PPI).

void gic400_pdinfo(); // gic-400 read product information
void gic400_typer();  // gic-400 read GICD_TYPER.

void gic400_discovery(); // gic-400 full discovery.

void gic400_disable_pedings(); // gic-400 disable all peding irqs.
void gic400_disable_actives(); // gic-400 disable all active irqs.

void gic400_enirq(u16_t id);                        // gic-400 enable irq.
void gic400_disenirq(u16_t id);                     // gic-400 disable irq.
void gic400_targetirq(u16_t id, u8_t target_cores); // gic-400 target irq
void gic400_priorityirq(u16_t id, u8_t priority);   // gic-400 priority irq.

void gic400_setbpr(u8_t binary_point); // gic-400 set binary point register.

void gic400_sgi(u8_t id, u8_t cores); // send a sgi to cores.
#endif