# Makefile to build the OS

AS=nasm
CC=/opt/homebrew/opt/x86_64-elf-gcc/bin/x86_64-elf-gcc
LD=/opt/homebrew/opt/x86_64-elf-binutils/bin/x86_64-elf-ld
CFLAGS=-m32 -ffreestanding -O2 -Wall -Wextra
LDFLAGS=-m elf_i386 -T linker.ld -nostdlib

all: os-image

boot.o: boot/boot.asm
	$(AS) -f bin boot/boot.asm -o boot.o

kernel.o: kernel/kernel.c
	$(CC) $(CFLAGS) -c kernel/kernel.c -o kernel.o

kernel_entry.o: kernel/entry.asm
	nasm -f elf kernel/entry.asm -o kernel_entry.o

kernel.bin: kernel_entry.o kernel.o
	$(LD) $(LDFLAGS) kernel_entry.o kernel.o -o kernel.bin

os-image: boot.o kernel.bin
	cat boot.o kernel.bin > os-image

clean:
	rm -f *.o *.bin os-image
