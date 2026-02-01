#include "../../lib/fs/fs.h"
#include "commands.h"

void Command::Ls(Console* console)
{
	FileEntry entries[8];
	ATADriver::ReadSectors(FS::ROOT_LBA, 1, reinterpret_cast<uint16_t*>(entries));
	for (const auto& entry : entries)
		if (entry.active)
		{
			console->Write("\n");
			console->Write(entry.name);
		}
}