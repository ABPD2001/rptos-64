ENTRY(minit)
SECTIONS {
    /* INITIAL CODE TO SETUP CORES */
    . = 0x80000;
    .init:{
        __init_start__=.;
        ALIGN(8)
        *(.init)
    }
    .cores_info(NOLOAD):{
        ALIGN(4)
        __core_info_table__=.;
        .+=68; /* 10 dwords reserved. */
        /*
        (table in bytes)
        0-7: core 0 stack.
        8-15: core 1 stack.
        8-15: core 2 stack.
        16-23: core 3 stack.

        24-31: core 0 task.
        32-39: core 1 task.
        40-47: core 2 task.
        48-64: core 3 task.
        
        65-71: cores count (for acknowledging core 0 about "other cores are started")
        */
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
    
    /* KERNEL GLOBAL STRUCTURE */
    .external_preipherals(NOLAOD):{
        ALIGN(8);
        __global_timer_ticks__=.;
        .+=8; /* reserve dword for global timer ticks. */
        
        __global_muart_settings__ = .;
        .+=8; /* reserve 8 bytes for global mini uart settings struct. */ 
        __global_muart_metadata__ = .;
        .+=54; /* reserve 54 bytes for global mini uart metadata struct. */  
        __global_muart_statistics__ = .;
        .+=16; /* reserve 16 bytes for global mini uart statistics. */

        __global_uart0_statistics__ = .;
        .+=24; /* reserve 24 bytes for uart0 statistics. */
        __global_uart2_statistics__ = .;
        .+=24; /* reserve 24 bytes for uart2 statistics. */
        __global_uart3_statistics__ = .;
        .+=24; /* reserve 24 bytes for uart3 statistics. */
        __global_uart4_statistics__ = .;
        .+=24; /* reserve 24 bytes for uart4 statistics. */
        __global_uart5_statistics__ = .;
        .+=24; /* reserve 24 bytes for uart5 statistics. */
        
        __global_uart0_metadata__ = .;
        .+=54; /* reserve 54 bytes for uart0 metadata. */
        __global_uart2_metadata__ = .;
        .+=54; /* reserve 54 bytes for uart2 metadata. */
        __global_uart3_metadata__ = .;
        .+=54; /* reserve 54 bytes for uart3 metadata. */
        __global_uart4_metadata__ = .;
        .+=54; /* reserve 54 bytes for uart4 metadata. */
        __global_uart5_metadata__ = .;
        .+=54; /* reserve 54 bytes for uart5 metadata. */

        __global_uart0_settings__ = .;
        .+=48; /* reserve 48 bytes for uart0 settings. */
        __global_uart2_settings__ = .;
        .+=48; /* reserve 48 bytes for uart2 settings. */
        __global_uart3_settings__ = .;
        .+=48; /* reserve 48 bytes for uart3 settings. */
        __global_uart4_settings__ = .;
        .+=48; /* reserve 48 bytes for uart4 settings. */
        __global_uart5_settings__ = .;
        .+=48; /* reserve 48 bytes for uart5 settings. */

        __global_gic400_metadata__ = .;
        .+=16; /* reserve 16 bytes for gic-400 metadata. */
    }
    /* BANKS AND QUEUES */

    .banks(NOLOAD):{
        ALIGN(8) /* Alignment of 8 bytes for better and compatibility performance. */
        
        __pcb_bank_base__ = .;
        .+=23552; /* Reserve 64 struct of pcb. (368*64) Bytes. */
        
        __gpio_ownerships_bank_base__ = .;
        .+=1536; /* Reserve 64 struct of ownership. (24*64) Bytes. */
        
        __global_ipcmailboxes_segments_bank_base__ = .;
        .+=32768; /* Reserve 512 struct of ipcmailbox segment (512*64) Bytes. */
        __global_ipcmailbox_headers_bank_base__ = .;
        .+=3584; /* Reserve 64 struct of ipcmailbox header (16*56) Bytes. */

        __global_software_locks_bank_base__ = .;
        .+=4096; /* Reserve 128 struct of software locks. (128*32) Bytes. */

        __timer_request_bank_base__ = .;
        .+=1536; /* Reserve 64 struct of timer request. (24*64) Bytes. */
    }
    .pcb_queues(NOLAOD):{
        ALIGN(16) /* header struct size is 16 Bytes. */

        __pcb_queue_base__ = .;
        .+=712; /* Reserve 3 queues header of 4 states (and 8 ready queues). (8*4*16 + 3*16*4) + priority map for each core (8-bits each) + ticks for each core (8-bits each) */
        .+=64; /* Reserve 4 timer requests queues. */
    }
    __user_region_start__ = .;
}