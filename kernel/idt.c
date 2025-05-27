#include <stdint.h>
#include "idt.h"
#include "pic.h"

// Each IDT entry is 8 bytes
struct IDTEntry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t  zero;
    uint8_t  type_attr;
    uint16_t offset_high;
} __attribute__((packed));

// Pointer to IDT, for lidt instruction
struct IDTPointer {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

// Define a table of 256 entries
#define IDT_SIZE 256
static struct IDTEntry idt[IDT_SIZE];
static struct IDTPointer idt_ptr;

// Forward declaration
void idt_set_gate(int num, uint32_t handler, uint16_t selector, uint8_t type_attr);

// External symbols from other files
extern void keyboard_handler_stub();
extern void load_idt(uint32_t);

void idt_set_gate(int num, uint32_t handler, uint16_t selector, uint8_t type_attr) {
    idt[num].offset_low = handler & 0xFFFF;
    idt[num].selector = selector;
    idt[num].zero = 0;
    idt[num].type_attr = type_attr;
    idt[num].offset_high = (handler >> 16) & 0xFFFF;
}

void idt_init() {
    pic_remap();  // First: remap IRQs

    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base = (uint32_t)&idt;

    for (int i = 0; i < IDT_SIZE; i++) {
        idt_set_gate(i, 0, 0, 0);
    }

    idt_set_gate(0x21, (uint32_t)keyboard_handler_stub, 0x08, 0x8E);  // IRQ1
    load_idt((uint32_t)&idt_ptr);
}
