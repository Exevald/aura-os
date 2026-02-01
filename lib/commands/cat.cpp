#include "../../lib/fs/fs.h"
#include "commands.h"

void Command::Cat(const char* filename, Console* console)
{
	const FileEntry* file = FS::FindFile(filename);
	if (!file)
	{
		console->Write("Error: File not found.\n");
		return;
	}
	char buffer[513];
	for (char& i : buffer)
	{
		i = 0;
	}

	ATADriver::ReadSectors(file->startLBA, 1, reinterpret_cast<uint16_t*>(buffer));
	buffer[512] = '\0';

	console->Write("\n");
	console->Write(buffer);
	console->Write("\n");
}