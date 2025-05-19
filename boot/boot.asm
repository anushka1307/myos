; bootloader.asm - This will be our first sector
; Save this as "bootloader.asm"
[bits 16]
[org 0x7c00]

KERNEL_OFFSET equ 0x1000 ; Memory location where kernel will be loaded

start:
    ; Setup stack
    mov bp, 0x9000
    mov sp, bp

    ; Store boot drive number
    mov [BOOT_DRIVE], dl

    ; Display loading message
    mov si, MSG_LOADING
    call print_string

    ; Load kernel from disk
    call load_kernel
    
    ; Switch to protected mode
    call switch_to_pm
    
    ; We never return from switch_to_pm
    jmp $

; Function to load kernel from disk
load_kernel:
    mov bx, KERNEL_OFFSET  ; Memory address to load kernel
    mov dh, 15            ; Number of sectors to read (adjust as needed)
    mov dl, [BOOT_DRIVE]  ; Drive number saved earlier
    call disk_load
    ret

; Function to load sectors from disk
disk_load:
    push dx
    
    mov ah, 0x02          ; BIOS read sectors
    mov al, dh            ; Number of sectors to read
    mov ch, 0             ; Cylinder 0
    mov dh, 0             ; Head 0
    mov cl, 2             ; Start from sector 2 (sector 1 is our bootloader)
    
    int 0x13              ; BIOS disk interrupt
    
    jc disk_error         ; Jump if disk read error (carry flag set)
    
    pop dx
    cmp dh, al            ; Check if we read the correct number of sectors
    jne disk_error
    ret

disk_error:
    mov si, MSG_DISK_ERROR
    call print_string
    jmp $

; Function to print a string
print_string:
    mov ah, 0x0E          ; BIOS teletype function
.loop:
    lodsb                 ; Load byte from SI into AL and increment SI
    or al, al             ; Check if AL is 0 (end of string)
    jz .done
    int 0x10              ; Print the character
    jmp .loop
.done:
    ret

; GDT for protected mode
gdt_start:
    ; Null descriptor (required)
    dd 0x0
    dd 0x0
    
gdt_code:
    dw 0xffff             ; Limit (bits 0-15)
    dw 0x0                ; Base (bits 0-15)
    db 0x0                ; Base (bits 16-23)
    db 10011010b          ; Access byte
    db 11001111b          ; Flags + Limit (bits 16-19)
    db 0x0                ; Base (bits 24-31)
    
gdt_data:
    dw 0xffff             ; Limit (bits 0-15)
    dw 0x0                ; Base (bits 0-15)
    db 0x0                ; Base (bits 16-23)
    db 10010010b          ; Access byte
    db 11001111b          ; Flags + Limit (bits 16-19)
    db 0x0                ; Base (bits 24-31)
    
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; Size of GDT
    dd gdt_start                ; Address of GDT

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

; Function to switch to protected mode
switch_to_pm:
    cli                   ; Disable interrupts
    lgdt [gdt_descriptor] ; Load GDT
    
    ; Enable protected mode bit in CR0
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax
    
    ; Far jump to flush pipeline and load CS with CODE_SEG
    jmp CODE_SEG:init_pm

[bits 32]
; Initialize protected mode
init_pm:
    ; Update segment registers
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Setup new stack
    mov ebp, 0x90000
    mov esp, ebp
    
    ; Jump to the kernel
    call KERNEL_OFFSET
    jmp $                 ; In case kernel returns

; Data
BOOT_DRIVE db 0
MSG_LOADING db "Loading kernel...", 0
MSG_DISK_ERROR db "Disk read error!", 0

; Fill remaining space and add boot signature
times 510 - ($ - $$) db 0
dw 0xAA55