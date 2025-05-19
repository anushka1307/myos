#!/bin/bash
# build.sh - Builds your hobby OS

set -e

echo "🔨 Assembling bootloader..."
nasm -f bin boot/boot.asm -o boot.o

echo "🧠 Assembling kernel entry..."
nasm -f elf32 kernel/entry.asm -o kernel_entry.o

echo "🧬 Compiling kernel C code..."
x86_64-elf-gcc -m32 -ffreestanding -c kernel/kernel.c -o kernel.o

echo "🧱 Linking kernel..."
x86_64-elf-ld -m elf_i386 -T linker.ld -nostdlib kernel_entry.o kernel.o -o kernel.bin --oformat binary

echo "📦 Creating OS image..."
cat boot.o kernel.bin > os-image

echo "🚀 Running in QEMU..."
qemu-system-i386 -fda os-image
