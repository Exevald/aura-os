#include "../../lib/fs/fs.h"
#include "commands.h"

void Command::Echo(const char* text, const char* filename, Console* console)
{
	const FileEntry* file = FS::FindFile(filename);
	if (!file)
	{
		console->Write("File not found. Use touch first.\n");
		return;
	}

	char buffer[512];
	for (char& bufferChar : buffer)
	{
		bufferChar = 0;
	}
	for (int i = 0; i < 511 && text[i] != '\0'; i++)
	{
		buffer[i] = text[i];
	}

	ATADriver::WriteSectors(file->startLBA, 1, reinterpret_cast<uint16_t*>(buffer));
}