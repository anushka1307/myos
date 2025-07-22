# 🪐 HobbyOS – 32-bit Hobby Operating System

HobbyOS is a simple 32-bit x86 hobby operating system written in C and Assembly. It runs on QEMU, uses a custom bootloader, supports text-based VGA output, and includes keyboard interrupt handling, memory management, and a basic shell interface.

## 🚀 Features

- 🧠 **Bootloader**: Custom bootloader written in assembly
- 🎨 **VGA Text Mode Output**: Prints text to screen using direct video memory
- ⌨️ **Keyboard Support**: IRQ1 handler with support for characters, backspace, and enter
- 🧾 **Command Line Interface (CLI)**: Built-in commands like:
  - `help`: list available commands
  - `clear`: clear the screen
  - `echo`: repeat user input
  - `info`: system information
- 🛡️ **Interrupt Descriptor Table (IDT)**: Basic IDT setup with IRQ remapping
- 🧮 **Memory Management**:
  - Memory detection (up to 32MB RAM)
  - Simple heap-based allocator (`kmalloc`)
  - Custom memory map printing via `memory_print_info()`

## 🧪 Running the OS (macOS/Linux)

### Prerequisites

- `nasm`
- `qemu`
- `gcc` cross compiler for i386 (or native support with proper flags)

### Build and Run

```bash
make           # Build kernel and bootloader
make run       # Launch AnushkaOS in QEMU
