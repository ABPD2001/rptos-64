ENTRY(minit)
SECTIONS {
    /* INITIAL CODE TO SETUP CORES */
    . = 0x80000;
    
    .init ALIGN(8) : {                
        __init_start__ = .;
        *(.init)
    }
    
    .cores_info (NOLOAD) : {
        . = ALIGN(8);
        __core_info_table__ = .;
        . += 72; /* 9 dwords reserved. */
        /*
        (table in bytes)
        0-7: core 0 stack.
        8-15: core 1 stack.
        16-23: core 2 stack.
        24-31: core 3 stack.
        32-39: core 0 task.
        40-47: core 1 task.
        48-55: core 2 task.
        56-63: core 3 task.
        64-71: cores signal (each byte is for a core).
        */
    }

    /* TABLES */
    .vectors ALIGN(32) : {
        __vector_table__ = .;
        *(.vectors)
    }
    
    .sync_same_el_table ALIGN(4) : {
        __sync_same_el_table_start__ = .;
        *(.sync_same_el_table)
    }
    
    .sync_lower_el_table ALIGN(4) : {
        __sync_lower_el_table_start__ = .;
        *(.sync_same_el_table)
    }
    
    .irq_table ALIGN(4) : {
        __irq_table__ = .;
        *(.irq_table)
    }
/*     
    .fiq_table ALIGN(4) : {
        __fiq_table__ = .;
        *(.fiq_table)
    }
     */
    .svc_table ALIGN(4) : {
        __svc_table__ = .;
        *(.svc_table)
    }
    
    /* HANDLERS & INSTRUCTIONS */

    .vector_table_handlers ALIGN(8) : {
        *(.vector_table_handlers)
    }
    
    .sync_lower_el_handlers ALIGN(8) : {
        *(.sync_lower_el_handlers)
    }
    .svc_handlers ALIGN(8) : {
        *(.svc_handlers)
    }
    .irq_handler ALIGN(8) : {
        *(.irq_handlers)
    }

    .bss ALIGN(16) : {
        __bss_start__ = .;
        *(.bss)
        *(COMMON)
        __bss_end__ = .;
    }
    
    .rodata ALIGN(16) : {
        *(.rodata)
    }
    
    .text ALIGN(8) : {
        *(.text)
    }
    
    .stacks (NOLOAD) : {
        . = ALIGN(16);
        __stack3_top__ = .;
        . += 8192; /* reserve 8KB */
        __stack2_top__ = .;
        . += 8192; /* reserve 8KB */
        __stack1_top__ = .;
        . += 8192; /* reserve 8KB */
        __stack0_top__ = .;
        . += 8192; /* reserve 8KB */
    }
    
    /* KERNEL GLOBAL STRUCTURE */
    .external_preipherals (NOLOAD) : {
        . = ALIGN(8);
        __global_timer_ticks__ = .;
        . += 8; /* reserve dword for global timer ticks. */
        
        __global_muart_settings__ = .;
        . += 8; /* reserve 8 bytes for global mini uart settings struct. */ 
        __global_muart_metadata__ = .;
        . += 54; /* reserve 54 bytes for global mini uart metadata struct. */  

        __global_gic400_metadata__ = .;
        . += 16; /* reserve 16 bytes for gic-400 metadata. */
    }

    .statistics (NOLOAD) : {
        . = ALIGN(8);
        __global_muart_statistics__ = .;
        . += 24; /* reserve 24 bytes for global struct of muart statistics. */

        __generic_base_irq_statistics__ = .;
        . += 64; /* reserve (16*4) bytes for generic irq handlation statistics. */

        __generic_base_schaduler_statistics__ = .;
        . += 24; /* reserve (3*8) bytes for generic schaduler statistics. */

        __generic_base_system_exception_statistics__ = .;
        . += 128; /* reserve (32*4) bytes for generic statistics of exceptions. */
      }
    
    /* BANKS AND QUEUES */

    .banks (NOLOAD) : {
        . = ALIGN(8);
        __cccb_bank_base__ = .;
        . += 384; /* Reserve (96*4) bytes for struct of cccb. (for 4 cores) */

        __pcb_bank_base__ = .;
        . += 47104; /* Reserve 128 struct of pcb. (368*128) Bytes. */
        
        __gpio_ownerships_bank_base__ = .;
        . += 1024; /* Reserve 64 struct of ownership. (16*64) Bytes. */
        
        __global_ipcmailboxes_segments_bank_base__ = .;
        . += 65536; /* Reserve 1024 struct of ipcmailbox segment (1024*64) Bytes. */
        
        __global_ipcmailbox_headers_bank_base__ = .;
        . += 3584; /* Reserve 64 struct of ipcmailbox header (16*56) Bytes. */

        __global_software_locks_bank_base__ = .;
        . += 8192; /* Reserve 256 struct of software locks. (256*32) Bytes. */

        __timer_request_bank_base__ = .;
        . += 8192; /* Reserve 128 struct of timer request. (24*64) Bytes. */
    
        __global_tasks_dump_bank_base__ = .;
        . += 1792; /* Reserve 32 struct of task dump. (32*56) */
    }
    
    .queues (NOLOAD) : {
        . = ALIGN(16);
        
        __pcb_ready_queues_base__ = .;
        . += 128; /* multi-level queue header (ready state only), for 8 priority levels. */
        
        __pcb_waiting_queue_base__ = .;
        . += 16; /* a queue header for waiting. */
        
        __pcb_terminated_queue_base__ = .;
        . += 16; /* a queue header for terminated. */
        
        __pcb_created_queue_base__ = .;
        . += 16; /* a queue header for created. */
        
        __timer_requests_queue_base__ = .;
        . += 16; /* a queue header for timer requests. */

        __pcb_queue_lock_base__ = .;
        . += 8; /* reserve a mutex for queues. */
    
        __queues_temporary_base__ = .;
        . += 8; /* reserve for 2 words.s */    
    }

    /* system memory-based logs */

    .logs (NOLOAD) : {
        . = ALIGN(8);
        __system_panic_log__ = .;
        . += 120; /* reserve 120 bytes for struct of panic log. */

        __system_debug_log__ = .;
        . += 1216; /* reserve (4*2*152) bytes for structs of breakpoint. */
    }

    /* memory frames (MMU) */
    .mmu (NOLOAD) : {
        . = ALIGN(8);
        __system_memory_frame_bank_base__ = .;
        . += 33554432; /* reserve 28 MB for memory frames metadata. */
        
        __memory_paging_settings_base__ = .;
        . += 8; /* reserve 8 bytes for struct of memory paging settings. */
    
        __global_kernel_page_bank_base__ = .;
        . += 752; /* Reserve 47 pages for kernel use. */

        __global_kernel_frames_bank_base__ = .;
        . += 752; /* Reserve 47 frames for kernel use. */

        __kernel_memory_lock__ = .;
        . += 8; /* Reserve a mutex lock. */
    }

    /* Pre-builded services. */

    .kernel_service_power ALIGN(8) : {
        __kernel_service_power_base__ = .;
        *(.kernel_service_power)
    }

    .kernel_service_serial ALIGN(8) : {
        __kernel_service_serial_base__ = .;
        *(.kernel_service_serial)
    }
    .kernel_services_virtaul_maps ALIGN(8) : {
        __kernel_sevices_virtual_maps__ = .;
        . += 64; /* allocate 2 l0 table, 2 l1 table, 2 l2 table and 2 l3 table. */
    }

    __user_region_start__ = .;
}