#include "../../include/terminal/print.h"
#include "../../include/terminal/terminal.h"

/* --- INTERNAL UTILITIES --- */

/**
 * Converts an unsigned integer to a string in a given base (10 or 16).
 * Extracts digits using modulo, stores them in a buffer, and prints in reverse.
 */
static void print_number(unsigned int num, int base) {
    if (num == 0) {
        terminal_put_char('0');
        return;
    }

    char buffer[32]; // Temporary storage for digits
    int i = 0;
    char *digits = "0123456789abcdef";

    // Extract digits from least to most significant
    while (num > 0) {
        buffer[i++] = digits[num % base];
        num /= base;
    }

    // Print digits in correct order
    while (i > 0) {
        i--;
        terminal_put_char(buffer[i]);
    }
}

/**
 * Handles signed integers for decimal display (%d).
 * Prints a '-' sign for negative values before conversion.
 */
static void print_signed_number(int num) {
    if (num < 0) {
        terminal_put_char('-');
        print_number((unsigned int)(-num), 10);
    } else {
        print_number((unsigned int)num, 10);
    }
}

/**
 * Core printing engine.
 * Parses the format string and retrieves variadic arguments from the stack.
 * Supports: %c (char), %s (string), %d (decimal), %x (hex), %% (percent).
 */
static void core_print(__builtin_va_list args, const char *format) {
    for (int i = 0; format[i] != '\0'; i++) {
        if (format[i] == '%' && format[i + 1] != '\0') {
            i++;
            switch (format[i]) {
                case 'c': 
                    terminal_put_char((char)__builtin_va_arg(args, int)); 
                    break;
                case 's': {
                    const char *s = __builtin_va_arg(args, const char *);
                    terminal_write(s ? s : "(null)");
                    break;
                }
                case 'd': 
                    print_signed_number(__builtin_va_arg(args, int)); 
                    break;
                case 'x': 
                    print_number(__builtin_va_arg(args, unsigned int), 16); 
                    break;
                case '%': 
                    terminal_put_char('%'); 
                    break;
                default:
                    // Print literal '%' and the unknown character
                    terminal_put_char('%');
                    terminal_put_char(format[i]);
                    break;
            }
        } else {
            terminal_put_char(format[i]);
        }
    }
}

/* --- PUBLIC API --- */

/**
 * Kernel-space formatted print.
 * Entry point for internal system logging.
 */
void printk(const char *format, ...) {
    __builtin_va_list args;
    __builtin_va_start(args, format);
    
    core_print(args, format);
    
    __builtin_va_end(args);
}

/**
 * Standard printf implementation.
 * Returns 0 for now (character count not yet implemented).
 */
int printf(const char *format, ...) {
    __builtin_va_list args;
    __builtin_va_start(args, format);
    
    core_print(args, format);
    
    __builtin_va_end(args);
    
    return 0; 
}