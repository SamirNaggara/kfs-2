#include "../../include/terminal/print.h"
#include "../../include/gdt/gdt.h"

/**
 * Dumps the current state of the kernel stack to the terminal.
 * @param num_entries: Number of 32-bit words to display from ESP upwards.
 */
void print_kernel_stack(int num_entries) {
    uint32_t esp;
    // Capture the current Stack Pointer (ESP) using inline assembly
    asm volatile("mov %%esp, %0" : "=r"(esp));

    printk("\n[KERNEL STACK DUMP]\n");
    printk("----------------------------------------------------------\n");
    printk("  Memory Addr | Off(Hex) | Off(Dec) | Value (Hex) \n");
    printk("----------------------------------------------------------\n");

    // Treat ESP as a pointer to 32-bit unsigned integers
    uint32_t *ptr = (uint32_t *)esp;

    for (int i = 0; i < num_entries; i++) {
        uint32_t offset = i * 4; // Each stack entry is 4 bytes wide
        
        printk("  0x%x  |  +0x%x   |   +%d    |  0x%x\n", 
               (uint32_t)&ptr[i], 
               offset, 
               offset, 
               ptr[i]);
    }
    printk("----------------------------------------------------------\n\n");
}

/**
 * Test utility that pushes recognizable hex patterns to the stack
 * to verify that the dump is reading memory correctly.
 */
void test_and_print_stack() {
    // Recognizeable "Magic" markers
    uint32_t magic1 = 0xDEADBEEF;
    uint32_t magic2 = 0xCAFEBABE;
    uint32_t magic3 = 0x42424242;

    // Optimization barrier: forces the compiler to store these variables 
    // in memory (on the stack) rather than keeping them in registers.
    asm volatile("" :: "m"(magic1), "m"(magic2), "m"(magic3));

    printk("Stack dump test initiated...\n");
    print_kernel_stack(20);
}