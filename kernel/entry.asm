; entry.asm - Entry point for our kernel
; Save this as "entry.asm"

[bits 32]
[extern kernel_main]       ; Declare that we will be referencing the external kernel_main symbol

global _start
_start:
    call kernel_main       ; Call our main C kernel function
    jmp $                  ; Infinite loop if kernel returns