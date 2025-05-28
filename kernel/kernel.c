#include "idt.h"
#include "keyboard.h"
// External functions from keyboard.c
extern void clear_screen();
extern void print_string_at(const char* str, int x, int y, uint8_t color);
extern void show_prompt();

void kernel_main() {
    clear_screen();
    
    // Print header and welcome message
    print_string_at("MyOS Command Line Interface", 0, 0, 0x0B); // Cyan header
    print_string_at("Type 'help' for available commands", 0, 1, 0x07); // Gray subtitle
    
    idt_init();
    __asm__ __volatile__("sti");  // Enable interrupts
    
    // Show initial prompt
    show_prompt();
    
    // Kernel main loop
    while (1) {
        __asm__ __volatile__("hlt"); // Halt CPU until interrupt
    }
}