ENTRY(_start)

SECTIONS {
    . = 1M;
    .text : {
        KEEP(*(.multiboot))
        *(.text)
    }
    .data : { *(.data) }
    .bss  : { *(.bss)  }
}