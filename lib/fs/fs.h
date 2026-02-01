#pragma once

#include "../../drivers/ata_driver.h"

struct __attribute__((packed)) FileEntry
{
	char name[32];
	uint32_t startLBA;
	uint32_t sizeBytes;
	uint8_t active;
	uint8_t unused[23];
};

class FS
{
public:
	static constexpr uint32_t ROOT_LBA = 100;
	static constexpr int MAX_FILES = 8;

	static void Format();
	static void CreateFile(const char* name, uint32_t lba, uint32_t size);
	static FileEntry* FindFile(const char* name);
	static uint32_t GetNextFreeLBA();
};