#include "ports.h"

void pic_remap() {
    port_byte_out(0x20, 0x11); // Start PIC1 init
    port_byte_out(0xA0, 0x11); // Start PIC2 init

    port_byte_out(0x21, 0x20); // PIC1 vector offset = 0x20
    port_byte_out(0xA1, 0x28); // PIC2 vector offset = 0x28

    port_byte_out(0x21, 0x04); // Tell PIC1 about PIC2 at IRQ2
    port_byte_out(0xA1, 0x02); // Tell PIC2 its cascade identity

    port_byte_out(0x21, 0x01); // PIC1: 8086 mode
    port_byte_out(0xA1, 0x01); // PIC2: 8086 mode

    // Mask all except keyboard (IRQ1 = bit 1)
    port_byte_out(0x21, 0xFD);  // 11111101 - unmask only IRQ1
    port_byte_out(0xA1, 0xFF);  // Mask all on PIC2
}