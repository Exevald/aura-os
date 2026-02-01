#include "fs.h"
#include "../utils/utils.h"

void FS::Format()
{
	constexpr uint8_t buffer[512] = {};
	ATADriver::WriteSectors(ROOT_LBA, 1, (uint16_t*)buffer);
}

void FS::CreateFile(const char* name, const uint32_t lba, const uint32_t size)
{
	FileEntry entries[MAX_FILES];
	ATADriver::ReadSectors(ROOT_LBA, 1, reinterpret_cast<uint16_t*>(entries));

	for (auto& entry : entries)
	{
		if (entry.active == 0)
		{
			memset(&entry, 0, sizeof(FileEntry));
			for (int j = 0; j < 31 && name[j]; j++)
			{
				entry.name[j] = name[j];
			}
			entry.startLBA = lba;
			entry.sizeBytes = size;
			entry.active = 1;
			ATADriver::WriteSectors(ROOT_LBA, 1, reinterpret_cast<uint16_t*>(entries));
			return;
		}
	}
}

FileEntry* FS::FindFile(const char* name)
{
	static FileEntry entries[MAX_FILES];
	ATADriver::ReadSectors(ROOT_LBA, 1, reinterpret_cast<uint16_t*>(entries));
	for (auto& entry : entries)
	{
		if (entry.active)
		{
			bool match = true;
			for (int j = 0; j < 32; j++)
			{
				if (entry.name[j] != name[j])
				{
					match = false;
					break;
				}
				if (name[j] == '\0')
				{
					break;
				}
			}
			if (match)
			{
				return &entry;
			}
		}
	}
	return nullptr;
}

uint32_t FS::GetNextFreeLBA()
{
	FileEntry entries[8];
	ATADriver::ReadSectors(ROOT_LBA, 1, reinterpret_cast<uint16_t*>(entries));
	uint32_t max_lba = ROOT_LBA + 1;
	for (const auto& entry : entries)
	{
		if (entry.active)
		{
			if (const uint32_t end = entry.startLBA + (entry.sizeBytes / 512) + 1; end > max_lba)
			{
				max_lba = end;
			}
		}
	}
	return max_lba;
}