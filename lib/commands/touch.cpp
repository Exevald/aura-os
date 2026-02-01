#include "../../lib/fs/fs.h"
#include "commands.h"

void Command::Touch(const char* filename, Console* console)
{
	 if (FS::FindFile(filename))
	 {
	 	console->Write("File already exists.\n");
	 	return;
	 }

	 const uint32_t lba = FS::GetNextFreeLBA();
	 FS::CreateFile(filename, lba, 512);

	 char zero_buffer[512];
	 for (char& i : zero_buffer)
	 {
	 	i = 0;
	 }
	 ATADriver::WriteSectors(lba, 1, reinterpret_cast<uint16_t*>(zero_buffer));
}