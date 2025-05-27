[bits 32]
global _start
extern kernel_main

section .text

_start:
    mov esp, 0x90000      ; ✅ Set up stack before calling any function
    mov ebp, esp

    call kernel_main

    cli
    hlt
