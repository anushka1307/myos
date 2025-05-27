[bits 32]
global keyboard_handler_stub
extern keyboard_handler

keyboard_handler_stub:
    pusha                     ; ✅ Save registers
    call keyboard_handler     ; ✅ Call C handler
    popa                      ; ✅ Restore registers
    iretd                     ; ✅ Correct return instruction for 32-bit
