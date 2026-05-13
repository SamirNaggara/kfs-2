#ifndef GDT_H
#define GDT_H

#include "../terminal/print.h"

/* Fixed-width types for i386 architecture */
typedef unsigned char  uint8_t;  
typedef unsigned short uint16_t; 
typedef unsigned int   uint32_t; 

/**
 * GDT Entry structure (8 bytes).
 * __attribute__((packed)) prevents the compiler from adding padding,
 * which is critical for hardware compatibility.
 */
struct gdt_entry {
    uint16_t limit_low;    // Limit bits 0-15
    uint16_t base_low;     // Base bits 0-15
    uint8_t  base_middle;  // Base bits 16-23
    uint8_t  access;       // Access byte (Privilege levels, Segment type)
    uint8_t  granularity;  // Limit bits 16-19 + Flags (Size, Granularity)
    uint8_t  base_high;    // Base bits 24-31
} __attribute__((packed));

/**
 * GDT Pointer structure (GDTR).
 * This is the 6-byte structure passed to the 'lgdt' instruction.
 */
struct gdt_ptr {
    uint16_t limit;        // Table size - 1
    uint32_t base;         // Table start address (0x800 for KFS-2)
} __attribute__((packed));

/* --- GDT Management Prototypes --- */

// Initializes all segments (Null, Kernel/User Code, Data, Stack)
void init_gdt();

// Encodes a segment descriptor into the GDT array
void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

// Assembly helper (boot.s) to load GDTR and refresh segment registers
extern void gdt_flush();

// Debug utility to dump GDT entries stored at 0x800
void print_gdt_memory();

#endif