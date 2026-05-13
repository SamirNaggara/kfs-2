#ifndef PRINTK_H
#define PRINTK_H

/**
 * Kernel-space formatted output.
 * Uses variadic arguments (...) to process format specifiers like %d, %s, %x.
 */
void printk(const char *format, ...);

/**
 * Standard-style formatted output wrapper.
 * Currently returns 0 as character counting is not yet implemented.
 */
int printf(const char *format, ...);

#endif