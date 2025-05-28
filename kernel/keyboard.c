// #include "ports.h"
// #include <stdint.h>

// // VGA text buffer
// static char* vga_buffer = (char*)0xB8000;
// static int cursor_x = 2;   // Start after "$ "
// static int cursor_y = 2;   // Start on line 2

// // Command buffer
// #define MAX_COMMAND_LENGTH 78
// static char command_buffer[MAX_COMMAND_LENGTH];
// static int command_index = 0;

// // US QWERTY scancode to ASCII mapping (for pressed keys only)
// static char scancode_to_ascii[] = {
//     0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
//     '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
//     0,    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
//     0,    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
//     '*',  0,  ' '  // Space key is scancode 57
// };

// // String comparison function
// int strcmp(const char* str1, const char* str2) {
//     while (*str1 && (*str1 == *str2)) {
//         str1++;
//         str2++;
//     }
//     return *str1 - *str2;
// }

// // String copy function
// void strcpy(char* dest, const char* src) {
//     while (*src) {
//         *dest = *src;
//         dest++;
//         src++;
//     }
//     *dest = '\0';
// }

// // String length function
// int strlen(const char* str) {
//     int len = 0;
//     while (str[len]) len++;
//     return len;
// }

// void clear_screen() {
//     for (int i = 0; i < 80 * 25 * 2; i += 2) {
//         vga_buffer[i] = ' ';
//         vga_buffer[i + 1] = 0x0F;
//     }
// }

// void print_string_at(const char* str, int x, int y, uint8_t color) {
//     int pos = (y * 80 + x) * 2;
//     for (int i = 0; str[i] != 0; i++) {
//         vga_buffer[pos + i * 2] = str[i];
//         vga_buffer[pos + i * 2 + 1] = color;
//     }
// }

// void print_line(const char* str) {
//     print_string_at(str, 0, cursor_y, 0x0F);
//     cursor_y++;
//     if (cursor_y >= 25) {
//         cursor_y = 24;
//         // Simple scroll - move everything up one line
//         for (int i = 0; i < 24 * 80 * 2; i++) {
//             vga_buffer[i] = vga_buffer[i + 80 * 2];
//         }
//         // Clear bottom line
//         for (int i = 24 * 80 * 2; i < 25 * 80 * 2; i += 2) {
//             vga_buffer[i] = ' ';
//             vga_buffer[i + 1] = 0x0F;
//         }
//     }
// }

// void update_cursor() {
//     // Set cursor position using VGA cursor registers
//     uint16_t pos = cursor_y * 80 + cursor_x;
    
//     // Send the high byte
//     port_byte_out(0x3D4, 0x0F);
//     port_byte_out(0x3D5, (uint8_t)(pos & 0xFF));
    
//     // Send the low byte
//     port_byte_out(0x3D4, 0x0E);
//     port_byte_out(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
// }

// void show_prompt() {
//     print_string_at("$ ", 0, cursor_y, 0x0A); // Green prompt
//     cursor_x = 2;
//     update_cursor();
// }

// void execute_command(const char* cmd) {
//     cursor_y++; // Move to next line to show output
    
//     if (strlen(cmd) == 0) {
//         // Empty command, just show new prompt
//         show_prompt();
//         return;
//     }
    
//     if (strcmp(cmd, "help") == 0) {
//         print_line("Available commands:");
//         print_line("  help    - Show this help message");
//         print_line("  clear   - Clear the screen");
//         print_line("  echo    - Echo back your input");
//         print_line("  info    - Show system information");
//         print_line("  hello   - Say hello!");
//     }
//     else if (strcmp(cmd, "clear") == 0) {
//         clear_screen();
//         print_string_at("MyOS Command Line Interface", 0, 0, 0x0B);
//         cursor_y = 2;
//         show_prompt();
//         return;
//     }
//     else if (strcmp(cmd, "info") == 0) {
//         print_line("MyOS v1.0");
//         print_line("Architecture: x86");
//         print_line("Features: Keyboard input, Basic CLI");
//     }
//     else if (strcmp(cmd, "hello") == 0) {
//         print_line("Hello from MyOS kernel!");
//     }
//     else if (strlen(cmd) > 5 && cmd[0] == 'e' && cmd[1] == 'c' && cmd[2] == 'h' && cmd[3] == 'o' && cmd[4] == ' ') {
//         // Echo command with arguments
//         print_line(cmd + 5); // Skip "echo "
//     }
//     else {
//         print_line("Unknown command. Type 'help' for available commands.");
//     }
    
//     show_prompt();
// }

// void put_char(char c) {
//     if (c == '\n') {
//         // Execute the command
//         command_buffer[command_index] = '\0';
//         execute_command(command_buffer);
//         command_index = 0;
//         return;
//     }
    
//     if (c == '\b') {  // Backspace
//         if (command_index > 0 && cursor_x > 2) {
//             cursor_x--;
//             command_index--;
//             int pos = (cursor_y * 80 + cursor_x) * 2;
//             vga_buffer[pos] = ' ';
//             vga_buffer[pos + 1] = 0x0F;
//             update_cursor();
//         }
//         return;
//     }
    
//     // Add character to command buffer and display
//     if (command_index < MAX_COMMAND_LENGTH - 1 && cursor_x < 80) {
//         command_buffer[command_index] = c;
//         command_index++;
        
//         int pos = (cursor_y * 80 + cursor_x) * 2;
//         vga_buffer[pos] = c;
//         vga_buffer[pos + 1] = 0x0F;
//         cursor_x++;
//         update_cursor();
//     }
// }

// void keyboard_handler() {
//     uint8_t scancode = port_byte_in(0x60);
    
//     // Only handle key presses (bit 7 = 0), ignore key releases
//     if (!(scancode & 0x80)) {
//         if (scancode < sizeof(scancode_to_ascii) && scancode_to_ascii[scancode] != 0) {
//             put_char(scancode_to_ascii[scancode]);
//         }
//     }
    
//     port_byte_out(0x20, 0x20); // Send EOI to PIC
// }


#include "ports.h"
#include <stdint.h>

// VGA text buffer
static char* vga_buffer = (char*)0xB8000;
static int cursor_x = 2;   // Start after "$ "
static int cursor_y = 2;   // Start on line 2

// Command buffer and cursor position within the command
#define MAX_COMMAND_LENGTH 78
static char command_buffer[MAX_COMMAND_LENGTH];
static int command_length = 0;  // Current length of command
static int cursor_pos = 0;      // Cursor position within command (0 = start)

// Command history
#define MAX_HISTORY 50
static char command_history[MAX_HISTORY][MAX_COMMAND_LENGTH];
static int history_count = 0;
static int history_index = -1;  // -1 means current command, not browsing history
static char temp_command[MAX_COMMAND_LENGTH]; // Store current command when browsing history

// Keyboard state for modifiers
static uint8_t shift_pressed = 0;
static uint8_t caps_lock = 0;

// Extended scancode handling
static uint8_t extended_scancode = 0;

// US QWERTY scancode to ASCII mapping (for pressed keys only)
static char scancode_to_ascii[] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0,    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*',  0,  ' '  // Space key is scancode 57
};

// Shifted characters
static char scancode_to_ascii_shifted[] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0,    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0,    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*',  0,  ' '
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

// Memory move function for string manipulation
void memmove(char* dest, const char* src, int n) {
    if (dest < src) {
        for (int i = 0; i < n; i++) {
            dest[i] = src[i];
        }
    } else {
        for (int i = n - 1; i >= 0; i--) {
            dest[i] = src[i];
        }
    }
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

void redraw_command_line() {
    // Clear the current line from prompt onwards
    int start_pos = (cursor_y * 80 + 2) * 2;
    for (int i = 0; i < (78 * 2); i += 2) {
        vga_buffer[start_pos + i] = ' ';
        vga_buffer[start_pos + i + 1] = 0x0F;
    }
    
    // Redraw the command
    command_buffer[command_length] = '\0';
    print_string_at(command_buffer, 2, cursor_y, 0x0F);
    
    // Update cursor position
    cursor_x = 2 + cursor_pos;
    update_cursor();
}

void show_prompt() {
    print_string_at("$ ", 0, cursor_y, 0x0A); // Green prompt
    cursor_x = 2;
    cursor_pos = 0;
    command_length = 0;
    command_buffer[0] = '\0';
    history_index = -1;  // Reset history browsing
    update_cursor();
}

void add_to_history(const char* cmd) {
    if (strlen(cmd) == 0) return;  // Don't add empty commands
    
    // Don't add if it's the same as the last command
    if (history_count > 0 && strcmp(command_history[history_count - 1], cmd) == 0) {
        return;
    }
    
    if (history_count < MAX_HISTORY) {
        strcpy(command_history[history_count], cmd);
        history_count++;
    } else {
        // Shift history up and add new command at the end
        for (int i = 0; i < MAX_HISTORY - 1; i++) {
            strcpy(command_history[i], command_history[i + 1]);
        }
        strcpy(command_history[MAX_HISTORY - 1], cmd);
    }
}

void load_history_command(int index) {
    if (index >= 0 && index < history_count) {
        strcpy(command_buffer, command_history[index]);
        command_length = strlen(command_buffer);
        cursor_pos = command_length;  // Move cursor to end
        redraw_command_line();
    }
}

void handle_up_arrow() {
    if (history_count == 0) return;
    
    if (history_index == -1) {
        // Save current command before browsing history
        command_buffer[command_length] = '\0';
        strcpy(temp_command, command_buffer);
        history_index = history_count - 1;
    } else if (history_index > 0) {
        history_index--;
    }
    
    load_history_command(history_index);
}

void handle_down_arrow() {
    if (history_index == -1) return;
    
    if (history_index < history_count - 1) {
        history_index++;
        load_history_command(history_index);
    } else {
        // Restore the command that was being typed
        history_index = -1;
        strcpy(command_buffer, temp_command);
        command_length = strlen(command_buffer);
        cursor_pos = command_length;
        redraw_command_line();
    }
}

void handle_left_arrow() {
    if (cursor_pos > 0) {
        cursor_pos--;
        cursor_x--;
        update_cursor();
    }
}

void handle_right_arrow() {
    if (cursor_pos < command_length) {
        cursor_pos++;
        cursor_x++;
        update_cursor();
    }
}

void handle_delete() {
    if (cursor_pos < command_length) {
        // Shift characters left to delete character at cursor
        memmove(&command_buffer[cursor_pos], &command_buffer[cursor_pos + 1], 
                command_length - cursor_pos);
        command_length--;
        redraw_command_line();
    }
}

void execute_command(const char* cmd) {
    cursor_y++; // Move to next line to show output
    
    if (strlen(cmd) == 0) {
        // Empty command, just show new prompt
        show_prompt();
        return;
    }
    
    // Add command to history
    add_to_history(cmd);
    
    if (strcmp(cmd, "help") == 0) {
        print_line("Available commands:");
        print_line("  help    - Show this help message");
        print_line("  clear   - Clear the screen");
        print_line("  echo    - Echo back your input");
        print_line("  info    - Show system information");
        print_line("  hello   - Say hello!");
        print_line("  history - Show command history");
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
        print_line("Features: Keyboard input, Basic CLI, Command history");
    }
    else if (strcmp(cmd, "hello") == 0) {
        print_line("Hello from MyOS kernel!");
    }
    else if (strcmp(cmd, "history") == 0) {
        if (history_count == 0) {
            print_line("No command history available.");
        } else {
            print_line("Command history:");
            for (int i = 0; i < history_count; i++) {
                char line[MAX_COMMAND_LENGTH + 10];
                // Simple number formatting
                int num = i + 1;
                int digits = 0;
                int temp_num = num;
                while (temp_num > 0) {
                    digits++;
                    temp_num /= 10;
                }
                if (digits == 0) digits = 1;
                
                // Format: "  1  command"
                line[0] = ' ';
                line[1] = ' ';
                int pos = 2;
                
                // Add number
                char num_str[10];
                int num_pos = 0;
                temp_num = num;
                do {
                    num_str[num_pos++] = '0' + (temp_num % 10);
                    temp_num /= 10;
                } while (temp_num > 0);
                
                // Reverse number string
                for (int j = num_pos - 1; j >= 0; j--) {
                    line[pos++] = num_str[j];
                }
                
                line[pos++] = ' ';
                line[pos++] = ' ';
                
                // Add command
                strcpy(&line[pos], command_history[i]);
                print_line(line);
            }
        }
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

char get_char_with_modifiers(uint8_t scancode) {
    if (scancode >= sizeof(scancode_to_ascii)) return 0;
    
    char c = scancode_to_ascii[scancode];
    if (c == 0) return 0;
    
    // Handle shift and caps lock for letters
    if (c >= 'a' && c <= 'z') {
        if ((shift_pressed && !caps_lock) || (!shift_pressed && caps_lock)) {
            return c - 32; // Convert to uppercase
        }
    }
    // Handle shift for other characters
    else if (shift_pressed && scancode < sizeof(scancode_to_ascii_shifted)) {
        return scancode_to_ascii_shifted[scancode];
    }
    
    return c;
}

void insert_char(char c) {
    if (command_length >= MAX_COMMAND_LENGTH - 1) return;
    
    // If cursor is at the end, just append
    if (cursor_pos == command_length) {
        command_buffer[cursor_pos] = c;
        command_length++;
        cursor_pos++;
        command_buffer[command_length] = '\0';
        
        // Display character
        int pos = (cursor_y * 80 + cursor_x) * 2;
        vga_buffer[pos] = c;
        vga_buffer[pos + 1] = 0x0F;
        cursor_x++;
        update_cursor();
    } else {
        // Insert character in the middle - need to shift everything right
        memmove(&command_buffer[cursor_pos + 1], &command_buffer[cursor_pos], 
                command_length - cursor_pos + 1);
        command_buffer[cursor_pos] = c;
        command_length++;
        cursor_pos++;
        redraw_command_line();
    }
}

void handle_backspace() {
    if (cursor_pos > 0) {
        // Move everything left to delete character before cursor
        memmove(&command_buffer[cursor_pos - 1], &command_buffer[cursor_pos], 
                command_length - cursor_pos + 1);
        cursor_pos--;
        command_length--;
        redraw_command_line();
    }
}

void keyboard_handler() {
    uint8_t scancode = port_byte_in(0x60);
    
    // Handle extended scancodes (0xE0 prefix)
    if (scancode == 0xE0) {
        extended_scancode = 1;
        port_byte_out(0x20, 0x20); // Send EOI to PIC
        return;
    }
    
    uint8_t key_pressed = !(scancode & 0x80);
    uint8_t key_code = scancode & 0x7F;
    
    if (extended_scancode) {
        extended_scancode = 0;
        if (key_pressed) {
            switch (key_code) {
                case 0x48: // Up arrow
                    handle_up_arrow();
                    break;
                case 0x50: // Down arrow
                    handle_down_arrow();
                    break;
                case 0x4B: // Left arrow
                    handle_left_arrow();
                    break;
                case 0x4D: // Right arrow
                    handle_right_arrow();
                    break;
                case 0x53: // Delete
                    handle_delete();
                    break;
            }
        }
    } else {
        // Handle modifier keys
        if (key_code == 0x2A || key_code == 0x36) { // Left/Right Shift
            shift_pressed = key_pressed;
        }
        else if (key_code == 0x3A && key_pressed) { // Caps Lock
            caps_lock = !caps_lock;
        }
        else if (key_pressed) {
            // Handle regular keys
            if (key_code == 0x1C) { // Enter
                command_buffer[command_length] = '\0';
                execute_command(command_buffer);
            }
            else if (key_code == 0x0E) { // Backspace
                handle_backspace();
            }
            else {
                char c = get_char_with_modifiers(key_code);
                if (c != 0) {
                    insert_char(c);
                }
            }
        }
    }
    
    port_byte_out(0x20, 0x20); // Send EOI to PIC
}