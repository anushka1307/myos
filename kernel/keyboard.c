#include "ports.h"
#include <stdint.h>

// VGA text buffer
static char* vga_buffer = (char*)0xB8000;
static int cursor_x = 2;   // Start after "$ "
static int cursor_y = 2;   // Start on line 2

// Command buffer
#define MAX_COMMAND_LENGTH 78
static char command_buffer[MAX_COMMAND_LENGTH];
static int command_index = 0;

// US QWERTY scancode to ASCII mapping (for pressed keys only)
static char scancode_to_ascii[] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0,    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*',  0,  ' '  // Space key is scancode 57
};

// String comparison function
int strcmp(const char* str1, const char* str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *str1 - *str2;
}

// String copy function
void strcpy(char* dest, const char* src) {
    while (*src) {
        *dest = *src;
        dest++;
        src++;
    }
    *dest = '\0';
}

// String length function
int strlen(const char* str) {
    int len = 0;
    while (str[len]) len++;
    return len;
}

void clear_screen() {
    for (int i = 0; i < 80 * 25 * 2; i += 2) {
        vga_buffer[i] = ' ';
        vga_buffer[i + 1] = 0x0F;
    }
}

void print_string_at(const char* str, int x, int y, uint8_t color) {
    int pos = (y * 80 + x) * 2;
    for (int i = 0; str[i] != 0; i++) {
        vga_buffer[pos + i * 2] = str[i];
        vga_buffer[pos + i * 2 + 1] = color;
    }
}

void print_line(const char* str) {
    print_string_at(str, 0, cursor_y, 0x0F);
    cursor_y++;
    if (cursor_y >= 25) {
        cursor_y = 24;
        // Simple scroll - move everything up one line
        for (int i = 0; i < 24 * 80 * 2; i++) {
            vga_buffer[i] = vga_buffer[i + 80 * 2];
        }
        // Clear bottom line
        for (int i = 24 * 80 * 2; i < 25 * 80 * 2; i += 2) {
            vga_buffer[i] = ' ';
            vga_buffer[i + 1] = 0x0F;
        }
    }
}

void update_cursor() {
    // Set cursor position using VGA cursor registers
    uint16_t pos = cursor_y * 80 + cursor_x;
    
    // Send the high byte
    port_byte_out(0x3D4, 0x0F);
    port_byte_out(0x3D5, (uint8_t)(pos & 0xFF));
    
    // Send the low byte
    port_byte_out(0x3D4, 0x0E);
    port_byte_out(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void show_prompt() {
    print_string_at("$ ", 0, cursor_y, 0x0A); // Green prompt
    cursor_x = 2;
    update_cursor();
}

void execute_command(const char* cmd) {
    cursor_y++; // Move to next line to show output
    
    if (strlen(cmd) == 0) {
        // Empty command, just show new prompt
        show_prompt();
        return;
    }
    
    if (strcmp(cmd, "help") == 0) {
        print_line("Available commands:");
        print_line("  help    - Show this help message");
        print_line("  clear   - Clear the screen");
        print_line("  echo    - Echo back your input");
        print_line("  info    - Show system information");
        print_line("  hello   - Say hello!");
    }
    else if (strcmp(cmd, "clear") == 0) {
        clear_screen();
        print_string_at("MyOS Command Line Interface", 0, 0, 0x0B);
        cursor_y = 2;
        show_prompt();
        return;
    }
    else if (strcmp(cmd, "info") == 0) {
        print_line("MyOS v1.0");
        print_line("Architecture: x86");
        print_line("Features: Keyboard input, Basic CLI");
    }
    else if (strcmp(cmd, "hello") == 0) {
        print_line("Hello from MyOS kernel!");
    }
    else if (strlen(cmd) > 5 && cmd[0] == 'e' && cmd[1] == 'c' && cmd[2] == 'h' && cmd[3] == 'o' && cmd[4] == ' ') {
        // Echo command with arguments
        print_line(cmd + 5); // Skip "echo "
    }
    else {
        print_line("Unknown command. Type 'help' for available commands.");
    }
    
    show_prompt();
}

void put_char(char c) {
    if (c == '\n') {
        // Execute the command
        command_buffer[command_index] = '\0';
        execute_command(command_buffer);
        command_index = 0;
        return;
    }
    
    if (c == '\b') {  // Backspace
        if (command_index > 0 && cursor_x > 2) {
            cursor_x--;
            command_index--;
            int pos = (cursor_y * 80 + cursor_x) * 2;
            vga_buffer[pos] = ' ';
            vga_buffer[pos + 1] = 0x0F;
            update_cursor();
        }
        return;
    }
    
    // Add character to command buffer and display
    if (command_index < MAX_COMMAND_LENGTH - 1 && cursor_x < 80) {
        command_buffer[command_index] = c;
        command_index++;
        
        int pos = (cursor_y * 80 + cursor_x) * 2;
        vga_buffer[pos] = c;
        vga_buffer[pos + 1] = 0x0F;
        cursor_x++;
        update_cursor();
    }
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