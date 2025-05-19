ENTRY(_start)

SECTIONS {
    . = 0x1000;           /* Kernel loaded at 0x1000 physical memory address */
    
    .text : {
        *(.text)          /* All text sections from all files */
    }
    
    .rodata : {
        *(.rodata)        /* All read-only data sections */
    }
    
    .data : {
        *(.data)          /* All data sections */
    }
    
    .bss : {
        *(COMMON)         /* Common symbols */
        *(.bss)           /* All BSS sections */
    }
}