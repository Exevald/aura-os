#include "ata_driver.h"

static constexpr uint16_t STATUS_BSY = 0x80;
static constexpr uint16_t STATUS_DRQ = 0x08;

void ATADriver::WaitReady()
{
	while (Port::InByte(0x1F7) & STATUS_BSY)
		;
}

void ATADriver::WaitDRQ()
{
	while (!(Port::InByte(0x1F7) & STATUS_DRQ))
		;
}

void ATADriver::ReadSectors(uint32_t lba, uint8_t count, uint16_t* buffer)
{
	WaitReady();

	// дать имена портам
	Port::OutByte(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));
	Port::IoWait();

	Port::OutByte(0x1F2, count);
	Port::OutByte(0x1F3, static_cast<uint8_t>(lba));
	Port::OutByte(0x1F4, static_cast<uint8_t>(lba >> 8));
	Port::OutByte(0x1F5, static_cast<uint8_t>(lba >> 16));
	Port::OutByte(0x1F7, 0x20);

	for (int j = 0; j < count; j++)
	{
		WaitReady();
		WaitDRQ();
		for (int i = 0; i < 256; i++)
		{
			uint16_t data;
			asm volatile("inw %1, %0" : "=a"(data) : "Nd"(0x1F0));
			buffer[j * 256 + i] = data;
		}
		for (int k = 0; k < 4; k++)
		{
			Port::IoWait();
		}
	}
}

void ATADriver::WriteSectors(uint32_t lba, uint8_t count, const uint16_t* buffer)
{
	WaitReady();
	Port::OutByte(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));
	Port::IoWait();

	Port::OutByte(0x1F2, count);
	Port::OutByte(0x1F3, (uint8_t)lba);
	Port::OutByte(0x1F4, (uint8_t)(lba >> 8));
	Port::OutByte(0x1F5, (uint8_t)(lba >> 16));
	Port::OutByte(0x1F7, 0x30);

	for (int j = 0; j < count; j++)
	{
		WaitReady();
		WaitDRQ();
		for (int i = 0; i < 256; i++)
		{
			uint16_t data = buffer[j * 256 + i];
			asm volatile("outw %0, %1" : : "a"(data), "Nd"(0x1F0));
		}
	}
}