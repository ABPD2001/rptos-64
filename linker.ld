ENTRY(reset_handler)
SECTIONS {
    .bss:{
        ALIGN(16)
        __bss_start=.;
        *(.bss)
        *(COMMON)
        __bss_end=.;
    }
}