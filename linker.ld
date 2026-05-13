/* Kernel entry point (the 'start' label defined in boot.s) */
ENTRY(start)

SECTIONS
{
    /* * Load the kernel at 1 Megabyte (1MB).
     * This avoids overwriting the BIOS and GRUB reserved areas in lower memory.
     */
    . = 1M;

    /* * Executable code section.
     * The .multiboot header is placed first so GRUB can find it within the first 8KB.
     * Aligned to 4KB (standard page size).
     */
    .text BLOCK(4K) : ALIGN(4K)
    {
        *(.multiboot)
        *(.text)
    }

    /* Read-only data (constants, strings) */
    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata)
    }

    /* Initialized global and static variables */
    .data BLOCK(4K) : ALIGN(4K)
    {
        *(.data)
    }

    /* * Uninitialized data and stack.
     * COMMON handles global variables declared in multiple files.
     */
    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
    }
}