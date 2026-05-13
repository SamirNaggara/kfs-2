#ifndef TERMINAL_H
#define TERMINAL_H

#include "vga.h"

/**
 * Initializes the terminal state.
 * Clears the screen buffer and sets the hardware cursor to (0,0).
 */
void terminal_initialize(void);

/**
 * Updates the current color attribute for subsequent characters.
 * @param fg: Foreground color.
 * @param bg: Background color.
 */
void terminal_set_color(enum vga_color fg, enum vga_color bg);

/**
 * Prints a single character at the current cursor position.
 * Handles special characters like '\n' and automatic scrolling.
 */
void terminal_put_char(char c);

/**
 * Writes a null-terminated string to the terminal.
 * Internally calls terminal_put_char for each character.
 */
void terminal_write(const char* data);

#endif