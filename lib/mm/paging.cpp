#include "paging.h"
#include "../utils/utils.h"
#include "pmm.h"
#include "vmm.h"
#include "wsclock.h"

extern "C" void loadPageDirectory(uint32_t*);
extern "C" void enablePaging();

alignas(4096) PageDirectory kernelDirectory;
alignas(4096) PageTable firstPageTable;

void VirtualMemoryManager::Init()
{
	memset(&kernelDirectory, 0, sizeof(PageDirectory));
	memset(&firstPageTable, 0, sizeof(PageTable));

	for (int i = 0; i < 1024; i++)
	{
		firstPageTable.pages[i].frame = i;
		firstPageTable.pages[i].present = 1;
		firstPageTable.pages[i].rw = 1;
	}

	kernelDirectory.tables[0].table_addr = reinterpret_cast<uint32_t>(&firstPageTable) >> 12;
	kernelDirectory.tables[0].present = 1;
	kernelDirectory.tables[0].rw = 1;

	loadPageDirectory(reinterpret_cast<uint32_t*>(&kernelDirectory));
	enablePaging();
}

bool VirtualMemoryManager::MapPage(const uint32_t virtualAddress)
{
	const uint32_t pageDirectoryIndex = virtualAddress >> 22;
	const uint32_t pageTableIndex = (virtualAddress >> 12) & 0x03FF;

	if (pageDirectoryIndex != 0)
	{
		return false;
	}

	auto frame = PMM::AllocateFrame();
	if (frame == -1)
	{
		PerformPageReplacement();
		frame = PMM::AllocateFrame();
		if (frame == -1)
		{
			return false;
		}
	}

	firstPageTable.pages[pageTableIndex].frame = frame;
	firstPageTable.pages[pageTableIndex].present = 1;
	firstPageTable.pages[pageTableIndex].rw = 1;
	firstPageTable.pages[pageTableIndex].accessed = 1;
	firstPageTable.pages[pageTableIndex].dirty = 0;

	return true;
}

void VirtualMemoryManager::PerformPageReplacement()
{
	if (auto victim = WSClockManager::FindVictimPage(&firstPageTable); victim != -1)
	{
		PTE& page = firstPageTable.pages[victim];
		PMM::FreeFrame(page.frame);

		page.present = 0;
		page.available = 1;

		__asm__ volatile("mov %%cr3, %%eax; mov %%eax, %%cr3" ::: "eax", "memory");
	}
}