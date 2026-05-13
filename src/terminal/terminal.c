#include "../../include/terminal/vga.h"
#include "../../include/terminal/terminal.h"

/**
 * Terminal state structure.
 * Holds cursor position, current color attribute, and a pointer 
 * to the VGA text buffer at 0xB8000.
 */
struct terminal {
    int x;
    int y;
    unsigned char color;
    volatile unsigned short *buffer;
};

static struct terminal term;

/**
 * Initializes the terminal by resetting the cursor, 
 * setting the default color, and clearing the screen.
 */
void terminal_initialize(void) {
    term.x = 0;
    term.y = 0;
    term.color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    term.buffer = (unsigned short*) 0xB8000;

    const int total_size = VGA_WIDTH * VGA_HEIGHT;

    // Fill the screen with empty spaces using the initial color
    for (int i = 0; i < total_size; i++) {
        term.buffer[i] = vga_entry(' ', term.color);
    }

    vga_update_cursor(term.x, term.y);
}

/**
 * Updates the current color attribute for all subsequent characters.
 */
void terminal_set_color(enum vga_color fg, enum vga_color bg) {
    term.color = vga_entry_color(fg, bg);
}

/**
 * Shifts all rows up by one. 
 * Clears the last line and moves the cursor to the start of the new line.
 */
static void terminal_scroll(void) {
    // Shift every line (except the first) up by one row
    for (int y = 1; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            int to = (y - 1) * VGA_WIDTH + x;
            int from = y * VGA_WIDTH + x;
            term.buffer[to] = term.buffer[from];
        }
    }
    
    // Clear the bottom-most row with spaces
    for (int x = 0; x < VGA_WIDTH; x++) {
        int index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
        term.buffer[index] = vga_entry(' ', term.color);
    }
    
    term.y = VGA_HEIGHT - 1;
    vga_update_cursor(term.x, term.y);
}

/**
 * Prints a single character to the current cursor position.
 * Handles newline '\n' logic, line wrapping, and screen scrolling.
 */
void terminal_put_char(char c) {
    if (c == '\n') {
        term.x = 0;
        term.y++;
    } else {
        int index = term.y * VGA_WIDTH + term.x;
        term.buffer[index] = vga_entry((unsigned char)c, term.color);
        term.x++;
    }

    // Wrap to the next line if the end of the screen width is reached
    if (term.x >= VGA_WIDTH) {
        term.x = 0;
        term.y++;
    }

    // Trigger scrolling if the cursor exceeds screen height
    if (term.y >= VGA_HEIGHT) {
        terminal_scroll();
    }
    
    vga_update_cursor(term.x, term.y);
}

/**
 * Writes a null-terminated string by iterating through each character.
 */
void terminal_write(const char* data) {
    for (int i = 0; data[i] != '\0'; i++) {
        terminal_put_char(data[i]);
    }
}