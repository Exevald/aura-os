#pragma once

#include "../lib/utils/ports.h"

class ATADriver
{
public:
	static void WaitReady();
	static void WaitDRQ();
	static void ReadSectors(uint32_t lba, uint8_t count, uint16_t* buffer);
	static void WriteSectors(uint32_t lba, uint8_t count, const uint16_t* buffer);
};