#!/bin/bash
set -e

echo "🔨 Assembling bootloader..."
nasm -f bin boot/boot.asm -o boot.o

echo "🧠 Assembling kernel entry..."
nasm -f elf32 kernel/entry.asm -o kernel_entry.o

echo "🧬 Compiling kernel core..."
x86_64-elf-gcc -m32 -ffreestanding -c kernel/kernel.c -o kernel.o

echo "📦 Compiling IDT and loader..."
x86_64-elf-gcc -m32 -ffreestanding -c kernel/idt.c -o idt.o
nasm -f elf32 kernel/load_idt.asm -o load_idt.o

echo "📡 Compiling PIC remapping..."
x86_64-elf-gcc -m32 -ffreestanding -c kernel/pic.c -o pic.o

echo "⌨️  Compiling keyboard interrupt handling..."
nasm -f elf32 kernel/interrupts.asm -o keyboard_stub.o
x86_64-elf-gcc -m32 -ffreestanding -c kernel/keyboard.c -o keyboard.o
x86_64-elf-gcc -m32 -ffreestanding -c kernel/ports.c -o ports.o

echo "🔗 Linking everything into kernel..."
x86_64-elf-ld -m elf_i386 -T linker.ld -nostdlib \
  kernel_entry.o kernel.o \
  idt.o load_idt.o \
  keyboard_stub.o keyboard.o ports.o \
  pic.o \
  -o kernel.bin --oformat binary

echo "📦 Creating OS image..."
cat boot.o kernel.bin > os-image

echo "🚀 Running in QEMU..."
qemu-system-i386 -fda os-image
