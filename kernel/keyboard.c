#include "ports.h"
#include <stdint.h>

// VGA text buffer
static char* vga_buffer = (char*)0xB8000;
static int cursor_x = 16;  // Start after "Type something: " (16 characters)
static int cursor_y = 1;   // Start on line 1

// US QWERTY scancode to ASCII mapping (for pressed keys only)
static char scancode_to_ascii[] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0,    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*',  0,  ' '  // Space key is scancode 57
};

void put_char(char c) {
    if (c == '\n' || cursor_x >= 80) {
        cursor_x = 16;  // Reset to after the prompt on new line
        cursor_y++;
        if (cursor_y >= 25) cursor_y = 1;  // Wrap around, skip line 0
        return;
    }
    
    if (c == '\b') {  // Backspace
        if (cursor_x > 16) {  // Don't backspace over the prompt
            cursor_x--;
            int pos = (cursor_y * 80 + cursor_x) * 2;
            vga_buffer[pos] = ' ';
            vga_buffer[pos + 1] = 0x0F;
        }
        return;
    }
    
    int pos = (cursor_y * 80 + cursor_x) * 2;
    vga_buffer[pos] = c;
    vga_buffer[pos + 1] = 0x0F;  // White text on black background
    cursor_x++;
}

void keyboard_handler() {
    uint8_t scancode = port_byte_in(0x60);
    
    // Only handle key presses (bit 7 = 0), ignore key releases
    if (!(scancode & 0x80)) {
        if (scancode < sizeof(scancode_to_ascii) && scancode_to_ascii[scancode] != 0) {
            put_char(scancode_to_ascii[scancode]);
        }
    }
    
    port_byte_out(0x20, 0x20); // Send EOI to PIC
}