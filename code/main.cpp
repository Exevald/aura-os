#include <stdint.h>

void print(const char* string)
{
	asm volatile("int $0x80" : : "a"(1), "b"(string));
}

void readInput(const char* buf, uint32_t size)
{
	asm volatile("int $0x80" : : "a"(2), "b"(buf), "c"(size));
}

extern "C" void _start()
{
	print("\nPlease enter your name: ");

	char buffer[64];
	readInput(buffer, 64);

	print("\nHello, ");
	print(buffer);
	print("\n\0");

	asm volatile("int $0x80" : : "a"(1), "b"(0));
}