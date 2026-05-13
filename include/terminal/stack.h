#ifndef STACK_H
#define STACK_H

#include "../gdt/gdt.h" // Provides fixed-width types like uint32_t

/**
 * Dumps the specified number of 32-bit entries from the current stack pointer (ESP).
 * Displays memory addresses, offsets, and raw hex values.
 * @param num_entries: Number of stack levels to inspect.
 */
void print_kernel_stack(int num_entries);

/**
 * Test utility that pushes "magic" hex values (e.g., 0xDEADBEEF) 
 * onto the stack to verify that the dump utility works correctly.
 */
void test_and_print_stack();

#endif