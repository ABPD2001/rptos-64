ENTRY(minit)
SECTIONS {
    /* INITIAL CODE TO SETUP CORES */
    . = 0x8000;
    .init:{
        __init_start__=.;
        ALIGN(8)
        *(.init)
    }
    .cores_info(NOLOAD):{
        ALIGN(4)
        __core_stack_table__=.;
        *(.cores_info)
    }

    /* TABLES */
    .vectors:{
        ALIGN(32)
        __vector_table__=.;
        *(.vectors)
    }
    .sync_same_el_table:{
        __sync_same_el_table_start__=.;
        ALIGN(4)
        *(.sync_same_el_table)
    }
    .sync_lower_el_table:{
        __sync_same_el_table_start__=.;
        ALIGN(4);
        *(.sync_same_el_table)
    }
    .irq_table:{
        ALIGN(4)
        __irq_table__=.;
        *(.irq_table)
    }
    .fiq_table:{
        ALIGN(4)
        __fiq_table__=.;
        *(.fiq_table)
    }
    .svc_table:{
        ALIGN(4)
        __svc_table__ = .;
        *(.svc_table)
    }
    
    /* HANDLERS & INSTRUCTIONS */

    .vector_table_handlers:{
        ALIGN(8)
        *(vector_table_handlers)
    }
    
    .sync_lower_el_handlers:{
        ALIGN(8)
        *(sync_lower_el_handlers)
    }

    .bss:{
        ALIGN(16)
        __bss_start__=.;
        *(.bss)
        *(COMMON)
        __bss_end__=.;
    }
    .rodata:{
        ALIGN(16)
        *(.rodata)
    }
    .text:{
        ALIGN(8)
        *(.text)
    }
    .stacks(NOLOAD):{
        ALIGN(16)
        __stack3_top__ = .;
        . += 8193; /* reserve 8KB */
        __stack2_top__ = .;
        . += 8193; /* reserve 8KB */
        __stack1_top__ = .;
        . += 8193; /* reserve 8KB */
        __stack0_top__ = .;
        . += 8193; /* reserve 8KB */
    }
}