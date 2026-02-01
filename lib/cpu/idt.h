#pragma once
#include <stdint.h>

struct IDTEntry
{
	uint16_t offsetLow;
	uint16_t selector;
	uint8_t zero;
	uint8_t type_attr;
	uint16_t offsetHigh;
} __attribute__((packed));

struct IDTPtr
{
	uint16_t limit;
	uint32_t base;
} __attribute__((packed));

void LoadIDT();