#include "../../include/gdt/gdt.h"

// Pointer to the GDT starting at the required physical address 0x800
struct gdt_entry *gdt = (struct gdt_entry *)0x800;
struct gdt_ptr gp;

/**
 * Configures a single GDT entry (segment descriptor).
 * Splits 32-bit base and 20-bit limit into the specific bit-fields 
 * required by the x86 hardware architecture.
 */
void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    // Encode the Base Address (32 bits)
    gdt[num].base_low    = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high   = (base >> 24) & 0xFF;

    // Encode the Segment Limit (20 bits)
    gdt[num].limit_low   = (limit & 0xFFFF);
    gdt[num].granularity = (limit >> 16) & 0x0F;

    // Encode Flags (Granularity, Size, etc.) and Access Rights
    gdt[num].granularity |= gran & 0xF0;
    gdt[num].access      = access;
}

/**
 * Initializes the Global Descriptor Table (GDT) with 7 segments.
 * This kernel uses a Flat Memory Model, meaning all segments overlap 
 * and span the entire 4GB of addressable memory space.
 * * --- FLAT MEMORY MODEL PARAMETERS ---
 * - Base  : 0x0 (All segments start at physical address 0)
 * - Limit : 0xFFFFFFFF (All segments span the maximum 32-bit limit)
 * * --- GRANULARITY BYTE : 0xCF (0b11001111) ---
 * [G] [D/B] [L] [AVL] | [Limit 19:16]
 * 1    1    0    0   |   1 1 1 1
 * 			* G (Granularity) : 1 = Multiplies the limit by 4KB blocks (allows reaching 4GB).
 * 				D/B (Size)      : 1 = 32-bit protected mode segment.
 * 				L (Long Mode)   : 0 = Not a 64-bit segment.
 * 				AVL (Available) : 0 = Unused by hardware, available for OS use.
 * 				Limit 19:16     : 1111 = The upper 4 bits of our 0xFFFFFFFF limit.
 * * --- ACCESS BYTE FORMAT ---
 * [P] [DPL1 DPL2] [S] | [E] [DC] [RW] [A]
 * * P   (Present)   : 1 = Segment is present in memory.
 * DPL (Privilege) : 00 = Ring 0 (Kernel) | 11 = Ring 3 (User).
 * S   (Type)      : 1 = Code or Data segment (0 would mean System segment).
 * E   (Executable): 1 = Code segment | 0 = Data segment.
 * DC  (Direction) : 0 = Grows up normally (Data) / Non-conforming (Code).
 * RW  (Read/Write): If Code (E=1): 1 = Readable. If Data (E=0): 1 = Writable.
 * A   (Accessed)  : 0 = CPU sets this to 1 when the segment is used.
 */
void init_gdt() {
    // 1. Set the GDTR pointer: total size of the table minus 1, and physical base address
    gp.limit = (sizeof(struct gdt_entry) * 7) - 1;
    gp.base  = 0x00000800;

    // 2. Index 0: Null Segment (Strictly required by x86 architecture)
    gdt_set_gate(0, 0, 0, 0, 0);
    
    /* ========================================================================
     * KERNEL SEGMENTS (Ring 0) - DPL bits are set to 00
     * ======================================================================== */
    
    // Index 1: Kernel Code 
    // Access: Present(1) Ring0(00) Normal(1) | Code(1) Non-Conf(0) Read(1) Acc(0)
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0b10011010, 0xCF);
    
    // Index 2: Kernel Data  
    // Access: Present(1) Ring0(00) Normal(1) | Data(0) GrowsUp(0) Write(1) Acc(0)
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0b10010010, 0xCF);
    
    // Index 3: Kernel Stack 
    // Identical to Kernel Data. Separated by convention for the SS register.
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0b10010010, 0xCF);
    
    /* ========================================================================
     * USER SEGMENTS (Ring 3) - DPL bits are set to 11
     * ======================================================================== */
    
    // Index 4: User Code
    // Access: Present(1) Ring3(11) Normal(1) | Code(1) Non-Conf(0) Read(1) Acc(0)
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0b11111010, 0xCF);
    
    // Index 5: User Data
    // Access: Present(1) Ring3(11) Normal(1) | Data(0) GrowsUp(0) Write(1) Acc(0)
    gdt_set_gate(5, 0, 0xFFFFFFFF, 0b11110010, 0xCF);
    
    // Index 6: User Stack
    // Identical to User Data. Separated by convention.
    gdt_set_gate(6, 0, 0xFFFFFFFF, 0b11110010, 0xCF);

    // 3. Force the CPU to update its internal segment caches with the new GDT
    gdt_flush();
}

/**
 * Dumps the raw GDT memory content from address 0x800.
 * Useful for verifying that the bit-packing is correct.
 */
void print_gdt_memory() {
    uint32_t *gdt_memory = (uint32_t *)0x800;

    printk("\n[GDT MEMORY DUMP AT 0x800]\n");
    printk("--------------------------------------------------\n");
    printk(" Index | Base Address | Value (Hex) : LOW | HIGH \n");
    printk("--------------------------------------------------\n");

    for (int i = 0; i < 7; i++) {
        // Each GDT entry is 8 bytes (two 32-bit integers)
        uint32_t mem_index = i * 2;
        uint32_t low_part = gdt_memory[mem_index];
        uint32_t high_part = gdt_memory[mem_index + 1];

        uint32_t address = 0x800 + (i * 8);

        printk("  [%x]  | 0x%x | 0x%x : 0x%x\n", i, address, low_part, high_part);
    }
    printk("--------------------------------------------------\n\n");
}