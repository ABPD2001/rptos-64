ENTRY(reset_handler)
SECTIONS {
    . = 0x00;
    .reset:{
        ALIGN(4)
        *(.reset_handler)
    }
    .vectors:{
        *(.vectors)
    }
    .vector_table_handlers:{
        *(.vector_table_handlers)
    }
    .bss:{
        ALIGN(16)
        __bss_start=.;
        *(.bss)
        *(COMMON)
        __bss_end=.;
    }
}