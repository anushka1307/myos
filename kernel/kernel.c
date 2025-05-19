// kernel.c - our main C kernel code
// Save this as "kernel.c"

void kernel_main() {
    // Video memory pointer - VGA text mode buffer starts at 0xB8000
    char* video_mem = (char*) 0xB8000;
    
    // Message to display
    const char* message = "Hello from the kernel!";
    
    // Display the message (VGA text mode is 2 bytes per character - ASCII code and attribute)
    int i = 0;
    while (message[i] != 0) {
        // Character at even-numbered addresses, attribute at odd-numbered addresses
        video_mem[i * 2] = message[i];           // Character
        video_mem[i * 2 + 1] = 0x0F;             // White on black
        i++;
    }
}