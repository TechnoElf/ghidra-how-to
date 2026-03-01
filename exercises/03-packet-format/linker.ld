MEMORY {
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 16K
    SRAM  (rwx) : ORIGIN = 0x20000000, LENGTH = 8K
}

ENTRY(_start)

_stack_top = ORIGIN(SRAM) + LENGTH(SRAM);

SECTIONS {
    .text : {
        KEEP(*(.text._start))
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
    } > FLASH

    _etext = .;

    .data : {
        _sdata = .;
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > SRAM AT> FLASH

    _sidata = LOADADDR(.data);

    .bss (NOLOAD) : {
        _sbss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > SRAM

    _end = .;
}
