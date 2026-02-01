#include "../../drivers/ata_driver.h"
#include "../fs/elf.h"
#include "../fs/fs.h"
#include "../utils/ports.h"
#include "../utils/utils.h"
#include "commands.h"

void Command::Run(int argc, char** argv, Console* console)
{
	if (argc < 1)
	{
		console->Write("\nUsage: run <filename> [args...]");
		return;
	}

	const char* filename = argv[0];
	FileEntry* file = FS::FindFile(filename);

	if (!file || file->active == 0)
	{
		console->Write("\nFile not found: ");
		console->Write(filename);
		console->Write("\n");
		return;
	}

	auto* fileLoadBuffer = (uint8_t*)0x300000;
	uint32_t sectorsToRead = (file->sizeBytes + 511) / 512;
	ATADriver::ReadSectors(file->startLBA, (uint8_t)sectorsToRead, (uint16_t*)fileLoadBuffer);
	auto* header = (Elf32_Ehdr*)fileLoadBuffer;

	if (header->e_ident[0] != 0x7F || header->e_ident[1] != 'E' || header->e_ident[2] != 'L' || header->e_ident[3] != 'F')
	{
		console->Write("\nInvalid ELF signature.\n");
		return;
	}

	auto* phdr = (Elf32_Phdr*)(fileLoadBuffer + header->e_phoff);

	for (int i = 0; i < header->e_phnum; i++)
	{
		if (phdr[i].p_type == 1)
		{
			memcpy((void*)phdr[i].p_vaddr, fileLoadBuffer + phdr[i].p_offset, phdr[i].p_filesz);

			if (phdr[i].p_memsz > phdr[i].p_filesz)
			{
				memset((void*)(phdr[i].p_vaddr + phdr[i].p_filesz), 0, phdr[i].p_memsz - phdr[i].p_filesz);
			}
		}
	}

	char* argDataArea = (char*)0x3F0000;
	char** argvPointers = (char**)0x3F1000;

	for (int i = 0; i < argc; i++)
	{
		int len = 0;
		while (argv[i][len] != '\0')
		{
			argDataArea[len] = argv[i][len];
			len++;
		}
		argDataArea[len] = '\0';

		argvPointers[i] = argDataArea;
		argDataArea += (len + 1);
	}

	typedef void (*entry_t)(int, char**);
	auto startApp = (entry_t)header->e_entry;

	Port::OutByte(0x20, 0x20);

	startApp(argc, argvPointers);
}