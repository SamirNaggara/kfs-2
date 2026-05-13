#ifndef IO_H
#define IO_H

/**
 * Sends an 8-bit value to a specific hardware I/O port.
 * Wrapper for the assembly 'out' instruction.
 */
void outb(unsigned short port, unsigned char val);

/**
 * Reads an 8-bit value from a specific hardware I/O port.
 * Wrapper for the assembly 'in' instruction.
 */
unsigned char inb(unsigned short port);

#endif