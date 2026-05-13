; --- Part 1: Multiboot Header Constants ---
; Values required by GRUB to recognize and boot the kernel
MAGIC    equ 0x1BADB002        ; Magic number for Multiboot 1
FLAGS    equ 0x0               ; Multiboot flags
CHECKSUM equ -(MAGIC + FLAGS)  ; Checksum to validate the header

; --- Part 2: Multiboot Header Section ---
; Must be aligned and placed early in the binary (see linker.ld)
SECTION .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

; --- Part 3: Stack Reservation ---
; Reserve 16 KB for the initial kernel stack
SECTION .bss
align 16
stack_bottom:
    resb 16384 
stack_top:

; --- Part 4: Kernel Entry Point ---
SECTION .text
global start
extern kmain

start:
    ; Initialize the Stack Pointer (ESP)
    mov esp, stack_top

    ; Call the C main function
    call kmain

    ; Infinite loop safeguard if kmain returns
    cli
.hang:
    hlt
    jmp .hang

; --- Part 5: Hardware I/O Helpers ---

; void outb(unsigned short port, unsigned char data);
global outb
outb:
    mov al, [esp + 8]    ; Load data (2nd arg)
    mov dx, [esp + 4]    ; Load port (1st arg)
    out dx, al           ; Send byte to port
    ret

; unsigned char inb(unsigned short port);
global inb
inb:
    mov dx, [esp + 4]    ; Load port (1st arg)
    xor eax, eax         ; Clear return register
    in al, dx            ; Read byte from port
    ret

; --- Part 6: GDT Management ---

; extern void gdt_flush();
; Updates the GDTR and reloads all segment registers
global gdt_flush
extern gp                ; GDT pointer defined in C

gdt_flush:
    lgdt [gp]            ; Load the new GDT pointer into the CPU

    ; 0x10 is the Data Segment selector (Index 2 in GDT: 2 * 8 = 16)
    mov ax, 0x10         
    mov ds, ax           ; Update Data Segment
    mov es, ax           ; Update Extra Segment
    mov fs, ax           ; Update F-Segment
    mov gs, ax           ; Update G-Segment
    mov ss, ax           ; Update Stack Segment

    ; A "Far Jump" is required to reload the Code Segment (CS).
    ; 0x08 is the Code Segment selector (Index 1 in GDT: 1 * 8 = 8).
    jmp 0x08:.flush

.flush:
    ret                  ; Return to C code with the new GDT active