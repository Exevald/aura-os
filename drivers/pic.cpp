#include "pic.h"
#include "../lib/utils/ports.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1
#define ICW1_INIT 0x11
#define ICW4_8086 0x01

void PICRemap(const int offset1, const int offset2)
{
	Port::OutByte(PIC1_COMMAND, ICW1_INIT);
	Port::IoWait();
	Port::OutByte(PIC2_COMMAND, ICW1_INIT);
	Port::IoWait();

	Port::OutByte(PIC1_DATA, offset1);
	Port::IoWait();
	Port::OutByte(PIC2_DATA, offset2);
	Port::IoWait();

	Port::OutByte(PIC1_DATA, 4);
	Port::IoWait();
	Port::OutByte(PIC2_DATA, 2);
	Port::IoWait();

	Port::OutByte(PIC1_DATA, ICW4_8086);
	Port::IoWait();
	Port::OutByte(PIC2_DATA, ICW4_8086);
	Port::IoWait();
	Port::OutByte(PIC1_DATA, 0xFC);
	Port::OutByte(PIC2_DATA, 0xFF);
}