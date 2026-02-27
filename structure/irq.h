#ifndef TYPES_IRQ_H
#define TYPES_IRQ_H
#include "./base.h"
#include "./softwarelock.h"

struct irq_statistic_t
{
    u64_t oop_irqs_count; // out of prediction interrupt counts.
    u64_t oop_sgis_count; // out of prediction software generated interrupt counts.
};

#endif