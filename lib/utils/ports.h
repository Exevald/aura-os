#pragma once

#include <stdint.h>

class Port
{
public:
	static uint8_t InByte(uint16_t port)
	{
		uint8_t result;
		asm volatile("inb %1, %0" : "=a"(result) : "Nd"(port));
		return result;
	}

	static void OutByte(uint16_t port, uint8_t data)
	{
		asm volatile("outb %0, %1" : : "a"(data), "Nd"(port));
	}

	static void IoWait()
	{
		OutByte(0x80, 0);
	}
};