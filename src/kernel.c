#include "../include/terminal/terminal.h"
#include "../include/terminal/vga.h"
#include "../include/terminal/print.h"
#include "../include/terminal/stack.h"
#include "../include/gdt/gdt.h"

void kmain(void) 
{
	init_gdt();

    terminal_initialize();
    
		print_gdt_memory();
    
    // test_and_print_stack();

    terminal_write("42\n");
		
}