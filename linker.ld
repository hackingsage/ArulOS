ENTRY(kernel_main)

SECTIONS {
    . = 0x100000;

    .text : {
        *(.multiboot)     /* <- MUST come first */
        *(.text)
    }

    .rodata : { *(.rodata) }
    .data   : { *(.data) }
    .bss    : { *(.bss) *(COMMON) }

    .heap (NOLOAD) : {
        _heap_start = .;
        . = . + 1M;
        _heap_end = .;
    }
}
