#include "./gic400.h"

void gic400_pdinfo()
{
    volatile u32_t *gicd_iidr = GICD_BASE + GICD_IIDR;

    global_gic400_metadata->implementer = *gicd_iidr & 0x008;
    global_gic400_metadata->revision = (*gicd_iidr & 0xEFF) >> 12;
    global_gic400_metadata->product_id = (*gicd_iidr & 0xF8000) >> 24;
}

void gic400_typer()
{
    volatile u32_t *gicd_typer = GICD_BASE + GICD_TYPER;

    global_gic400_metadata->interrup_lines_number = *gicd_typer & 0x1F;
    global_gic400_metadata->cpu_interface_number = (*gicd_typer & 0xD0) >> 5;
    global_gic400_metadata->security_extension = (*gicd_typer & 0x4FF) > 1;
    global_gic400_metadata->lspi_count = (*gicd_typer & 0xF000) >> 11;
}

void gic400_disable_pedings()
{
    volatile u32_t *gicd_icpendr = GICD_BASE + GICD_ICPENDR_BASE;

    for (u64_t i = 0; i < (global_gic400_metadata->interrup_lines_number + 1) / 32; i++)
    {
        *gicd_icpendr = U32FILL;
        gicd_icpendr++;
    }
}

void gic400_disable_actives()
{
    volatile u32_t *gicd_icativer = GICD_BASE + GICD_ICACTIVER_BASE;

    for (u64_t i = 0; i < (global_gic400_metadata->interrup_lines_number + 1) / 32; i++)
    {
        *gicd_icativer = U32FILL;
        gicd_icativer++;
    }
}

void gic400_enirq(u16_t id)
{
    volatile u32_t *gicd_isenabler = GICD_BASE + GICD_ISENABLER_BASE;

    if (id / 32 > global_gic400_metadata->interrup_lines_number / 32)
        return;
    gicd_isenabler += built_in_max(0, global_gic400_metadata->interrup_lines_number / 32 - 1);
    *gicd_isenabler |= 1 << global_gic400_metadata->interrup_lines_number % 32;
}

void gic400_disenirq(u16_t id)
{
    volatile u32_t *gicd_icenabler = GICD_BASE + GICD_ICENABLER_BASE;

    if (id / 32 > (global_gic400_metadata->interrup_lines_number + 1) / 32)
        return;
    gicd_icenabler += built_in_max(0, (global_gic400_metadata->interrup_lines_number + 1) / 32 - 1);
    *gicd_icenabler |= 1 << (global_gic400_metadata->interrup_lines_number + 1) % 32;
}

void gic400_targetirq(u16_t id, u8_t target_cores)
{
    volatile u32_t *gicd_itargetsr = GICD_BASE + GICD_ITARGETSR_BASE;

    if (id / 32 > (global_gic400_metadata->interrup_lines_number + 1) / 32)
        return;
    gicd_itargetsr += built_in_max(0, (global_gic400_metadata->interrup_lines_number + 1) / 4 - 1);
    *gicd_itargetsr &= 0xFF << (global_gic400_metadata->interrup_lines_number + 1) % 32;
    *gicd_itargetsr |= target_cores << (global_gic400_metadata->interrup_lines_number + 1) % 32;
}

void gic400_priorityirq(u16_t id, u8_t priority)
{

    volatile u32_t *gicd_ipriotiyr = GICD_BASE + GICD_IPRIORITYR_BASE;

    if (id / 32 > (global_gic400_metadata->interrup_lines_number + 1) / 32)
        return;
    gicd_ipriotiyr += built_in_max(0, (global_gic400_metadata->interrup_lines_number + 1) / 4 - 1);
    *gicd_ipriotiyr &= 0xFF << (global_gic400_metadata->interrup_lines_number + 1) % 32;
    *gicd_ipriotiyr |= priority << (global_gic400_metadata->interrup_lines_number + 1) % 32;
}

void gic400_setbpr(u8_t binary_point)
{
    volatile u32_t *gicc_bpr = GICC_BASE + GICC_BPR;
    *gicc_bpr = (u32_t)binary_point;
}

void gic400_sgi(u8_t id, u8_t mode, u8_t targetlist)
{
    volatile u32_t *gicc_sgir = GICD_BASE + GICD_SGIR;
    u32_t temp = 0;

    temp |= (id & 0xF);
    temp |= targetlist << 16;
    temp |= mode << 24;

    *gicc_sgir = temp; // only one memory operation.
}

void gic400_interfacectl(u8_t eoimodens, u8_t enableGrp1)
{
    volatile u32_t *gicc_ctlr = GICC_BASE + GICC_CTLR;

    if (eoimodens)
        *gicc_ctlr |= (1 << 10); // enable 11th bit.
    else
        *gicc_ctlr &= ~(1 << 10); // disable 11th bit.
    if (enableGrp1)
        *gicc_ctlr |= (1 << 1); // enable 1th bit.
    else
        *gicc_ctlr &= ~(1 << 1); // disable 1th bit.
}

void gic400_setGrp1_distributor(u8_t enable)
{
    volatile u32_t *gicd_ctlr = GICD_BASE + GICD_CTLR;

    if (enable)
        *gicd_ctlr |= (1 << 1); // enable 1th bit.
    else
        *gicd_ctlr &= ~(1 << 1); // disable 1th bit.
}

void initialize_gic400()
{
    volatile u32_t *gicc_ctlr = GICC_BASE + GICC_CTLR;
    volatile u32_t *gicd_ctlr = GICD_BASE + GICD_CTLR;

    *gicc_ctlr |= (1 << 10); // enable 11th bit.
    *gicc_ctlr |= (1 << 1);  // enable 1th bit.
    *gicd_ctlr |= (1 << 1);  // enable 1th bit.
}

u32_t gic400_ackhowledge()
{
    volatile u32_t *gicc_iar = GICC_BASE + GICC_IAR;
    return *gicc_iar;
}

void gic400_end_interrupt(u32_t ack_val)
{
    volatile u32_t *gicc_eoir = GICC_BASE + GICC_EOIR;
    *gicc_eoir = ack_val;
}

void gic400_deactivate_interrupt(u32_t ack_val)
{
    volatile u32_t *gicc_dir = GICC_BASE + GICC_DIR;
    *gicc_dir = ack_val;
}