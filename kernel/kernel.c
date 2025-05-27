#include "idt.h"

void clear_screen() {
    char* vga = (char*)0xB8000;
    for (int i = 0; i < 80 * 25 * 2; i += 2) {
        vga[i] = ' ';      // Character
        vga[i + 1] = 0x0F; // White on black
    }
}

void print_string(const char* str, int x, int y, uint8_t color) {
    char* vga = (char*)0xB8000;
    int pos = (y * 80 + x) * 2;
    
    for (int i = 0; str[i] != 0; i++) {
        vga[pos + i * 2] = str[i];
        vga[pos + i * 2 + 1] = color;
    }
}

void kernel_main() {
    clear_screen();
    
    // Print header
    print_string("MyOS - Keyboard Test", 0, 0, 0x0B); // Cyan text
    print_string("Type something: ", 0, 1, 0x0F);     // White text
    
    idt_init();
    __asm__ __volatile__("sti");  // Enable interrupts
    
    // Kernel main loop
    while (1) {
        __asm__ __volatile__("hlt"); // Halt CPU until interrupt
    }
}