MEMORY
{
    FLASH (rx)  : ORIGIN = 0x00000000, LENGTH = 512K
    RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 512K
}

_estack = ORIGIN(RAM) + LENGTH(RAM); /*place stack at the end of RAM*/

/*program header of ELF*/
PHDRS
{
    text PT_LOAD FLAGS(5); /* 5 = read-only */
    data PT_LOAD FLAGS(6); /* 6 = read-write */
    bss PT_NULL;
}

/*section placement*/
SECTIONS
{
    /* The startup code goes first into FLASH */
    .isr_vector :
    {
        KEEP(*(.isr_vector))
    } > FLASH :text

    /* The program code and other data goes into FLASH */
    .text :
    {
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
        _etext = .;
    } > FLASH :text

    /* The .data section goes into RAM, load it from FLASH */
    .data : AT(_etext)
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } > RAM :data

    /* The .data section load address is in FLASH */
    _sidata = LOADADDR(.data);

    /* The .bss section goes into RAM */
    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > RAM :bss

    /* The .results section goes into RAM */
    .results 0x2007FF00 (NOLOAD) :
    {
        KEEP(*(.results))
    } > RAM :bss

    /DISCARD/ :
    {
        *(.comment*)
        *(.note*)
        *(.eh_frame*)
        *(.ARM.exidx*)
        *(.ARM.extab*)
    }
}
