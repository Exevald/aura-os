#include "../../lib/fs/fs.h"
#include "commands.h"

bool CheckStringContains(const char* haystack, const char* needle)
{
	if (!*needle)
	{
		return true;
	}
	for (const char* h = haystack; *h; ++h)
	{
		const char *haystackPtr = h, *needlePtr = needle;
		while (*haystackPtr && *needlePtr && *haystackPtr == *needlePtr)
		{
			haystackPtr++;
			needlePtr++;
		}
		if (!*needlePtr)
		{
			return true;
		}
	}
	return false;
}

void Command::Grep(const char* pattern, const char* filename, Console* console)
{
	const FileEntry* file = FS::FindFile(filename);
	if (!file)
	{
		console->Write("Error: File not found.\n");
		return;
	}

	char buffer[512];
	ATADriver::ReadSectors(file->startLBA, 1, reinterpret_cast<uint16_t*>(buffer));

	if (CheckStringContains(buffer, pattern))
	{
		console->Write("Match found in ");
		console->Write(filename);
		console->Write(":\n");
		console->Write(buffer);
		console->Write("\n");
	}
	else
	{
		console->Write("No match found.\n");
	}
}