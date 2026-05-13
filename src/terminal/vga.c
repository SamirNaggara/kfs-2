#include "../../include/terminal/vga.h"
#include "../../include/terminal/io.h"

/**
 * Creates an 8-bit color attribute byte.
 * Merges foreground (bits 0-3) and background (bits 4-7) colors.
 */
unsigned char vga_entry_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

/**
 * Creates a 16-bit VGA display entry.
 * Combines an ASCII character with its color attribute.
 * Due to little-endianness, char is at bits 0-7 and color at bits 8-15.
 */
unsigned short vga_entry(unsigned char uc, unsigned char color) {
    return (unsigned short)uc | (unsigned short)color << 8;
}

/**
 * Updates the hardware cursor position using VGA I/O ports.
 * Port 0x3D4: Address register (selects internal VGA register)
 * Port 0x3D5: Data register (writes value to selected register)
 */
void vga_update_cursor(int x, int y) {
    // Calculate linear 1-D position
    unsigned short pos = y * VGA_WIDTH + x;
    
    // Select Low Byte register (0x0F) and send bits 0-7
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(pos & 0xFF));
    
    // Select High Byte register (0x0E) and send bits 8-15
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char)((pos >> 8) & 0xFF));
}