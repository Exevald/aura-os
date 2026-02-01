#pragma once

#include <stdint.h>

struct Registers
{
	uint32_t ds;
	uint32_t edi, esi, ebp, esp_dummy, ebx, edx, ecx, eax;
	uint32_t interruptionNumber, errorCode;
	uint32_t eip, cs, eflags, useresp, ss;
} __attribute__((packed));