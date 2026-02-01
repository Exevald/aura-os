#include "idt.h"

IDTEntry idt[256];
IDTPtr idtPtr;

extern "C" void irq0();
extern "C" void irq1();
extern "C" void isr128();

void SetIDTGate(const int num, const uint32_t base, const uint16_t sel, const uint8_t flags)
{
	idt[num].offsetLow = base & 0xFFFF;
	idt[num].offsetHigh = (base >> 16) & 0xFFFF;
	idt[num].selector = sel;
	idt[num].zero = 0;
	idt[num].type_attr = flags;
}

void LoadIDT()
{
	idtPtr.limit = (sizeof(IDTEntry) * 256) - 1;
	idtPtr.base = reinterpret_cast<uint32_t>(&idt);

	for (auto& i : idt)
	{
		i.type_attr = 0;
	}

	SetIDTGate(32, reinterpret_cast<uint32_t>(irq0), 0x08, 0x8E);
	SetIDTGate(33, reinterpret_cast<uint32_t>(irq1), 0x08, 0x8E);
	SetIDTGate(128, reinterpret_cast<uint32_t>(isr128), 0x08, 0x8E);

	asm volatile("lidt %0" : : "m"(idtPtr));
}