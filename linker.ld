OUTPUT_ARCH(riscv)
ENTRY(_start)

MEMORY
{
    RAM (rwx) : ORIGIN = 0x80000000, LENGTH = 128M
}

SECTIONS
{
    .text :
    {
        *(.text.init)
        *(.text*)
        *(.rodata*)
    } > RAM

    .sdata :
    {
        __global_pointer$ = . + 0x800;
        *(.sdata*)
    } > RAM

    .data :
    {
        *(.data*)
    } > RAM

    .bss (NOLOAD) :
    {
        __bss_start = .;
        *(.bss*)
        *(COMMON)
        __bss_end = .;
    } > RAM

    .stack (NOLOAD) :
    {
        . = ALIGN(16);
        __stack_bottom = .;
        . += 16K;
        _stack_top = .;
    } > RAM

    __kernel_end = .;
    __heap_start = ALIGN(4096);
    __heap_end = ORIGIN(RAM) + LENGTH(RAM);
}
